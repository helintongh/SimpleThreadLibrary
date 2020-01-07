#include "queue.h"


#include <errno.h>
#include <stdlib.h>

/*
    define node
    定义节点
*/
struct node {
    TCB *thread;
    struct node *next;
};

/*
    define queue
    定义队列
*/
struct queue {
    struct node *head;
    size_t size;
};


//在堆上创建一个新的初始化队列。
QUEUE *queue_new(void)
{
    QUEUE *new;

    if ((new = calloc(1, sizeof(QUEUE))) == NULL) {
	return NULL;
    }

    return new;
}

//销毁队列，释放与之相关的所有内存。
void queue_destroy(QUEUE *queue)
{
    struct node *prev = NULL;
    struct node *cursor = queue->head;

    while (cursor != NULL) {
	prev = cursor;
	cursor = cursor->next;

	tcb_destroy(prev->thread);
	free(prev);
    }

    free(queue);
}

//返回队列中的项数。即统计队列大小
size_t queue_size(const QUEUE *queue)
{
    return queue->size;
}

//将elem添加到队列的末尾。成功返回0，失败返回非0。入队
int queue_enqueue(QUEUE *queue, TCB *elem)
{
    // Create the new node创建新节点

    struct node *new;

    if ((new = malloc(sizeof(struct node))) == NULL) {
	return errno;
    }

    new->thread = elem;
    new->next = NULL;

    // Enqueue the new node为新节点排队

    if (queue->head == NULL) {
	queue->head = new;
    } else {
	struct node *parent = queue->head;
	while (parent->next != NULL) {
	    parent = parent->next;
	}
	parent->next = new;
    }

    queue->size += 1;
    return 0;
}

//出队操作
TCB *queue_dequeue(QUEUE *queue)
{
    struct node *old_head = queue->head;
    queue->head = queue->head->next;
    queue->size -= 1;

    TCB *retval = old_head->thread;
    free(old_head);

    return retval;
}

//通过id删除队列中的元素。
TCB *queue_remove_id(QUEUE *queue, int id)
{
    if (queue->head == NULL) {
	return NULL;
    }

    struct node *prev = NULL;
    struct node *cur = queue->head;

    while (cur != NULL) {
	if (cur->thread->id == id) {
	    if (prev == NULL) {
		queue->head = cur->next;
	    } else {
		prev->next = cur->next;
	    }

	    TCB *retval = cur->thread;
	    free(cur);
	    return retval;
	}

	prev = cur;
	cur = cur->next;
    }

    return NULL;
}
