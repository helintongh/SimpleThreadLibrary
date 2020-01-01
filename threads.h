/*
*	threads.h
*	Interface to a barebones user level thread library.
*	实现用户级线程 
*/

#ifndef	THREADS_H
#define	THREADS_H

/*
	Create a new thread. func is the function that will be run once the
   	thread starts execution and arg is the argument for that
   	function. On success, returns an id equal or greater to 0. On
   	failure, errno is set and -1 is returned.
   	创建一个新线程。
*/