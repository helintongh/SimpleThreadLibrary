#include "threads.h"
#include "tcb.h"
#include "queue.h"
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <ucontext.h>


static QUEUE *ready, *completed;
static TCB *running;


static bool init_queues(void);
static bool init_first_context(void);
static bool init_profiling_timer(void);

static void handle_sigprof(int, siginfo_t *, void *);
static void handle_thread_start();

static bool malloc_stack(TCB *);

static void block_sigprof(void);
static void unblock_sigprof(void);


int threads_create(void *(*start_routine) (void *), void *arg)
{
    block_sigprof();

    // Init if necessary初始化

    static bool initialized;

    if (! initialized) {
	if (! init_queues()) {
	    abort();
	}

	if (! init_first_context()) {
	    abort();
	}

	if (! init_profiling_timer()) {
	    abort();
	}
	
	initialized = true;
    }

    // Create a thread control block for the newly created thread.
    // 为新创建的线程创建一个线程控制块
    TCB *new;

    if ((new = tcb_new()) == NULL) {
	return -1;
    }

    if (getcontext(&new->context) == -1) {
	tcb_destroy(new);
	return -1;
    }

    if (! malloc_stack(new)) {
	tcb_destroy(new);
	return -1;
    }

    makecontext(&new->context, handle_thread_start, 1, new->id);

    new->start_routine = start_routine;
    new->argument = arg;

    // Enqueue the newly created stack
    // 对新创建的空间进行排队
    if (queue_enqueue(ready, new) != 0) {
	tcb_destroy(new);
	return -1;
    }

    unblock_sigprof();
    return new->id;
}


void threads_exit(void *result)
{
    if (running == NULL) {
	exit(EXIT_SUCCESS);
    }

    block_sigprof();
    running->return_value = result;

    if (queue_enqueue(completed, running) != 0) {
	abort();
    }

    if ((running = queue_dequeue(ready)) == NULL) {
	exit(EXIT_SUCCESS);
    }

    setcontext(&running->context);  // 还是未解锁
}


int threads_join(int id, void **result)
{
    if (id < 0) {
	errno = EINVAL;
	return -1;
    }

    block_sigprof();
    TCB *block = queue_remove_id(completed, id);
    unblock_sigprof();

    if (block == NULL) {
	return 0;
    } else {
	*result = block->return_value;
	tcb_destroy(block);
	return id;
    }
}


static bool init_queues(void)
{
    if ((ready = queue_new()) == NULL) {
	return false;
    }

    if ((completed = queue_new()) == NULL) {
	queue_destroy(ready);
	return false;
    }

    return true;
}

static bool init_first_context(void)
{
    TCB *block;

    if ((block = tcb_new()) == NULL) {
	return false;
    }

    if (getcontext(&block->context) == -1) {
	tcb_destroy(block);
	return false;
    }

    running = block;
    return true;
}


static bool init_profiling_timer(void)
{
    // Install signal handler
    // 加载信号管理器
    sigset_t all;
    sigfillset(&all);

    const struct sigaction alarm = {
	.sa_sigaction = handle_sigprof,
	.sa_mask = all,
	.sa_flags = SA_SIGINFO | SA_RESTART
    };

    struct sigaction old;

    if (sigaction(SIGPROF, &alarm, &old) == -1) {
	perror("sigaction");
	abort();
    }

    const struct itimerval timer = {
	{ 0, 10000 },
	{ 0, 1 }  // arms the timer as soon as possible启动计时器
    };

    // Enable timer赋予计时器功能

    if (setitimer(ITIMER_PROF, &timer, NULL) == - 1) {
	if (sigaction(SIGPROF, &old, NULL) == -1) {
	    perror("sigaction");
	    abort();
	}

	return false;
    }

    return true;
}


static void handle_sigprof(int signum, siginfo_t *nfo, void *context)
{
    int old_errno = errno;

    if (running == NULL && queue_size(ready) == 0) {
	_exit(EXIT_SUCCESS);
    }

    // Backup the current context
    // 超时要切换占用cpu的线程了
    ucontext_t *stored = &running->context;
    ucontext_t *updated = (ucontext_t *) context;

    stored->uc_flags = updated->uc_flags;
    stored->uc_link = updated->uc_link;
    stored->uc_mcontext = updated->uc_mcontext;
    stored->uc_sigmask = updated->uc_sigmask;

    // Round robin 让running中线程的进入ready就绪队列剥夺它的cpu使用权

    if (queue_enqueue(ready, running) != 0) {
	abort();
    }
    //ready队首出队获得cpu
    if ((running = queue_dequeue(ready)) == NULL) {
	abort();
    }

    // Manually leave the signal handler手动退出信号处理器

    errno = old_errno;
    if (setcontext(&running->context) == -1) {
	abort();
    }
}


static void handle_thread_start(void)
{
    block_sigprof();
    TCB *this = running;
    unblock_sigprof();

    void *result = this->start_routine(this->argument);
    threads_exit(result);
}

	 
static bool malloc_stack(TCB *thread)
{
    // Get the stack size得到分配的栈的大小

    struct rlimit limit;

    if (getrlimit(RLIMIT_STACK, &limit) == -1) {
	return false;
    }

    // Allocate memory分配存储空间

    void *stack;

    if ((stack = malloc(limit.rlim_cur)) == NULL) {
	return false;
    }

    // Update the thread control bock 更新tcb线程控制块

    thread->context.uc_stack.ss_flags = 0;
    thread->context.uc_stack.ss_size = limit.rlim_cur;
    thread->context.uc_stack.ss_sp = stack;
    thread->has_dynamic_stack = true;

    return true;
}


static void block_sigprof(void)
{
    sigset_t sigprof;
    sigemptyset(&sigprof);
    sigaddset(&sigprof, SIGPROF);

    if (sigprocmask(SIG_BLOCK, &sigprof, NULL) == -1) {
	perror("sigprocmask");
	abort();
    }
}


static void unblock_sigprof(void)
{
    sigset_t sigprof;
    sigemptyset(&sigprof);
    sigaddset(&sigprof, SIGPROF);

    if (sigprocmask(SIG_UNBLOCK, &sigprof, NULL) == -1) {
	perror("sigprocmask");
	abort();
    }
}
