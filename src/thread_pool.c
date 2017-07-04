#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void *thread_routine(void *arg)
{
	struct timespec abstime;
	int timeout;
	printf("thread 0x%0x is strating\n", (int)pthread_self());
	threadpool_t *pool=(threadpool_t *)arg;
	while(1)
	{
		timeout=0;
		condition_lock(&pool->ready);
		pool->idel++;
		//waiting for task, threadpool destroy notification.
		while(pool->first==NULL && pool->quit)
		{
			
			printf("thread 0x%0x is waiting\n", (int)pthread_self());

			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += 2;
			int status = condition_timewait(&pool->ready, &abstime);
			if(status==ETIMEDOUT)
			{
				printf("thread 0x%0x is waiting\n", (int)pthread_self());
				timeout=1;
				break;
			}
		}
		pool->idel--;
		
		if(pool->first != NULL)//waited a task
		{
			//get task in list head.
			task_t *t=pool->first;
			pool->first=t->next;

			condition_unlock(&pool->ready);
			t->run(t->arg);
			free(t);
			condition_lock(&pool->ready);
		}
		//waited a destroy notification and task all finished
		if(pool->quit && pool->first==NULL)
		{
			pool->counter--;
			if(pool->counter==0)
				condition_signal(&pool->ready);
			condition_unlock(&pool->ready);
			break;
		}
		//timeout
		if(timeout && pool->first==NULL)
		{
			pool->counter--;
			condition_unlock(&pool->ready);
			break;
		}
		condition_unlock(&pool->ready);
	}

	printf("thread 0x%0x is destroying\n", (int)pthread_self());
	return NULL;
}

void threadpool_init(threadpool_t *pool, int threads)
{
	//init arg int pool.
	condition_init(&pool->ready);
	pool->first=NULL;
	pool->last=NULL;
	pool->counter=0;
	pool->idel=0;
	pool->max_threads=threads;
	pool->quit=0;
}

void threadpool_add_task(threadpool_t *pool, void*(*run)(void *arg), void *arg)
{
	// new task
	task_t *newtask=(task_t*)malloc(sizeof(task_t));
	newtask->run=run;
	newtask->arg=arg;
	newtask->next=NULL;

	condition_lock(&pool->ready);
	//add task to tail
	if(pool->first==NULL)
		pool->first=newtask;
	else
		pool->last->next=newtask;
	pool->last=newtask;
	
	if(pool->idel > 0)
		condition_signal(&pool->ready);
	else if(pool->counter < pool->max_threads)
	{
		// no wait thread and counter < max_threads, create a new thread
		pthread_t tid;
		pthread_create(&tid, NULL, thread_routine, pool);
		pool->counter++;
	}
	condition_unlock(&pool->ready);
}

void threadpool_destroy(threadpool_t *pool)
{
	if(pool->quit)
		return;
	
	condition_lock(&pool->ready);
	pool->quit=1;
	if(pool->counter > 0)
	{
		if(pool->idel > 0)
			condition_broadcast(&pool->ready);
		//wait for threads that are working.
		//thread pool must wait them for quiting.
		while(pool->counter > 0)
		{
			condition_wait(&pool->ready);
		}
	}
	condition_unlock(&pool->ready);
	condition_destroy(&pool->ready);
}
