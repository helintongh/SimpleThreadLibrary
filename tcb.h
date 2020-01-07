/*
* filename:tcb.h
* purpose:定义线程控制块数据结构(pcb是进程控制块tcb自然就是线程控制块thread control block)
* wrote by: helintongh agh6399@gmail.com
* date time: 2019.11.23
*/

#ifndef TCB_H
#define TCB_H


#include <stdbool.h>
#include <stdint.h>
#include <ucontext.h>


typedef struct {
    int id;
    ucontext_t context;
    bool has_dynamic_stack;
    void *(*start_routine) (void *);
    void *argument;
    void *return_value;
} TCB;


/*
*	Create a new zeroed TCB on the heap. Returns a pointer to the new
*  	block or NULL on error.
*  	在堆上创建一个新的空的TCB线程控制块。返回指向新块的指针
*  	如果创建失败则返回NULL。
*/
TCB *tcb_new(void);

/*
*	Destroy block, freeing all associated memory with it
* 	销毁TCB线程控制块，并释放所有关联的内存
*/
void tcb_destroy(TCB *block);


#endif
