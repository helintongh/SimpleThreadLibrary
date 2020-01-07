/*
*	threads.h
*	Interface to a barebones user level thread library.
*	实现用户级线程 
*/

#ifndef THREADS_H
#define THREADS_H


/*
*	Create a new thread. func is the function that will be run once the
*   thread starts execution and arg is the argument for that
*   function. On success, returns an id equal or greater to 0. On
*   failure, errno is set and -1 is returned.
*   创建一个新线程。该函数开辟线程来执行函数，参数是arg。
*   如果创建成功，则返回一个等于或大于0的id。
*	如果失败，设置errno并返回-1。
*/
int threads_create(void *(*start_routine) (void *), void *arg);


/* 
*	Stop execution of the thread calling this function. 
*	停止执行调用此函数的线程。
*/
void threads_exit(void *result);


/* 
*	Wait for the thread with matching id to finish execution, that is,
*   for it to call threads_exit. On success, the threads result is
*   written into result and id is returned. If no completed thread with
*   matching id exists, 0 is returned. On error, -1 is returned and
*   errno is set. 
*	调用threads_join的作用是等待具有匹配id的线程完成执行，如果成功，线程的结果是
*	写入结果并返回执行该函数的线程id。
*	如果不存在线程匹配的id存在，返回0。
*	出错时，返回-1并设置errno。errno可以理解为错误码。
*/
int threads_join(int id, void **result);


#endif
