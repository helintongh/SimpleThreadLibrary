# SimpleThreadLibrary
Create my own Thread Library using Context Switching.

Detailed implementation:https://github.com/helintongh/hlt_note/blob/master/md/09.md

Three functions are implemented:

- threads_create: create a new thread.
- threads_exit: kill running thread.
- threads_join: connect two threads.

sample:
```C
/*demo.c*/
#include "threads.h"

#include <stdio.h>
#include <stdlib.h>


static void *thread1(void *arg)
{
    if ((unsigned long) arg % 2 == 0) {
	return arg;
    } else {
	for (;;) {
	}
    }
}


int main(void)
{
    puts("Hello, this is main().");

    int threads[8];

    for (unsigned long i = 0; i < 8; ++i) {
	void *arg = (void *) i;

	if ((threads[i] = threads_create(thread1, arg)) == -1) {
	    perror("threads_create");
	    exit(EXIT_FAILURE);
	}
    }

    for (int i = 0; i < 8; ++i) {
	int id = threads[i];

	while (1) {
	    void *res;

	    if (threads_join(id, &res) > 0) {
		printf("joined thread %d with result %p\n", id, res);
		break;
	    }
	}
    }
}

```