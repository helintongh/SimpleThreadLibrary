//tcb.c

#include "tcb.h"

#include <stdlib.h>

TCB *tcb_new(void)
{
	static int next_id = 1;

	TCB *newT;

	if((newT = calloc(1,sizeof(TCB))) == NULL )
	{
		return NULL;
	}

	newT -> id = new_id++;
	return newT;
}

void tcb_destory(TCB *block)
{
	if (block -> has_dynamic_stack)
	{
		free(block->contex.uc_stack.ss_sp);
	}

	free(block);
}
