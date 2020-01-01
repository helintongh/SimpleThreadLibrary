/*
 * queue.h
 * Defines a queue to mange TCB elements.
 * 通过队列来管理	TCB线程控制块
 */

#ifndef QUEUE_H
#define QUEUE_H


#include "tcb.h"

#include <unistd.h>


typedef struct queue QUEUE;


/* 
Create a new initialized QUEUE on the heap. Returns a pointer to
the new block or NULL on error. 
在堆上创建一个新的初始化队列。返回指向
新的块或错误为空
*/
QUEUE *queue_new(void);


/* 
	Destroy queue, freeing all associated memory with it. It also frees
	all memory of the elements inside the queue. 
	销毁队列，释放与之相关的所有内存。它还使
	队列中所有元素的内存被释放。
*/
void queue_destroy(QUEUE *queue);


/* 
	Return the number of items in queue. 
	返回队列中的项数。即统计队列大小
*/
size_t queue_size(const QUEUE *queue);


/* 
	Add elem to the end of queue. Returns 0 on succes and non-zero on
	failure.
	将elem添加到队列的末尾。成功返回0，失败返回非0
*/
int queue_enqueue(QUEUE *queue, TCB *elem);


/* 
	Remove the first item from the queue and return it. The caller will
   	have to free the reuturned element. Returns NULL if the queue is
   	empty. 
	从队列中删除第一项并返回它。调用者将
	必须释放已出队的元素。如果队列为空，则返回NULL。
	这个即为出队操作
*/
TCB *queue_dequeue(QUEUE *queue);


/* 
	Remove element with matching id from the queue. Returns the removed
   	element or NULL if no such element exists. 
	通过id删除元素。返回删除
	元素，如果不存在此类元素则为空。
*/
TCB *queue_remove_id(QUEUE *queue, int id);


#endif
