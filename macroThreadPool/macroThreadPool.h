/* License Information at EOF */

#ifndef MACRO_THREAD_POOL_H
#define MACRO_THREAD_POOL_H

#include <stddef.h>  /* NULL, size_t */
#include <limits.h>  /* INT_MAX */
#include <pthread.h> /* lots, can use a windows wrapper */

#define MTP_BOOL    int
#define MTP_TRUE    1
#define MTP_FALSE   0

#ifdef MACRO_THREAD_POOL_CUSTOM_ALLOC
#if !defined(MTP_CALLOC) || !defined(MTP_FREE)
#error "Please define both MTP_{CALLOC,FREE} if using custom allocation"
#endif
#else
#include <stdlib.h>  
#define MTP_CALLOC calloc
#define MTP_FREE free
#endif

#define MTP_ENQUEUE_JOB(type, queue, in)                                     \
do                                                                           \
{                                                                            \
	pthread_mutex_lock(&((queue)->ring_mutex));                          \
	                                                                     \
	while (((queue)->read_curs == (queue)->write_curs)                   \
	&& ((queue)->jobs_waiting != 0))                                     \
	{                                                                    \
		pthread_cond_wait(&((queue)->has_room),                      \
			&((queue)->ring_mutex));                             \
	}                                                                    \
	                                                                     \
	((type *) (queue)->jobs)[(queue)->write_curs++] = *((type *) in);    \
	(queue)->write_curs %= (queue)->jobs_max;                            \
	(queue)->jobs_waiting++;                                             \
	                                                                     \
	pthread_cond_broadcast(&((queue)->has_jobs));                        \
	pthread_mutex_unlock(&((queue)->ring_mutex));                        \
} while (0)

#define MTP_DEQUEUE_JOB(type, queue, out)                                    \
do                                                                           \
{                                                                            \
	pthread_mutex_lock(&((queue)->ring_mutex));                          \
	                                                                     \
	while ((queue)->jobs_waiting == 0)                                   \
	{                                                                    \
		pthread_cond_wait(&((queue)->has_jobs),                      \
			&((queue)->ring_mutex));                             \
	}                                                                    \
	                                                                     \
	*((type *) out) = ((type *) (queue)->jobs)[(queue)->read_curs++];    \
	(queue)->read_curs %= (queue)->jobs_max;                             \
	(queue)->jobs_waiting--;                                             \
	pthread_cond_broadcast(&((queue)->has_room));                        \
	                                                                     \
	if ((queue)->jobs_waiting == 0)                                      \
	{                                                                    \
		pthread_cond_broadcast(&((queue)->is_empty));                \
	}                                                                    \
	                                                                     \
	pthread_mutex_unlock(&((queue)->ring_mutex));                        \
} while (0)

/* ----------------------------- MIND THE GAP ----------------------------- */

#define MACRO_THREAD_POOL_PROTOTYPES(NAME, ElmType)                          \
                                                                             \
struct NAME##ThreadArgs                                                      \
{                                                                            \
	MTP_BOOL terminate;                                                  \
	ElmType payload;                                                     \
};                                                                           \
                                                                             \
struct NAME##JobQueue                                                        \
{                                                                            \
	struct NAME##ThreadArgs *jobs;                                       \
	size_t  jobs_max;                                                    \
	size_t  jobs_waiting;                                                \
	size_t  jobs_working;                                                \
	size_t  write_curs;                                                  \
	size_t  read_curs;                                                   \
	pthread_cond_t has_jobs;                                             \
	pthread_cond_t has_room;                                             \
	pthread_cond_t is_empty;                                             \
	pthread_cond_t is_idle;                                              \
	pthread_mutex_t ring_mutex;                                          \
	pthread_mutex_t work_mutex;                                          \
};                                                                           \
                                                                             \
struct NAME##ThreadPool                                                      \
{                                                                            \
	pthread_t *threads;                                                  \
	size_t num_threads;                                                  \
	struct NAME##JobQueue *queue;                                        \
};                                                                           \
                                                                             \
void NAME##EnqueueJob(struct NAME##ThreadPool *pool, ElmType in);            \
void* NAME##ThreadRoutine(void *queue);                                      \
struct NAME##ThreadPool* NAME##NewThreadPool(const size_t num_threads,       \
	const size_t max_jobs);                                              \
void NAME##CleanupThreadPool(struct NAME##ThreadPool *pool);                 \
void NAME##WaitOnIdle(struct NAME##ThreadPool *pool);                        \
                                                                             \
enum {MTP_PROTOTYPE_DUMMY = 0}

/* ----------------------------- MIND THE GAP ----------------------------- */

#define MACRO_THREAD_POOL_DEFINITIONS(NAME, ElmType, ThreadFunc)             \
                                                                             \
static pthread_once_t NAME##_id_once = PTHREAD_ONCE_INIT;                    \
static pthread_key_t NAME##_id_key;                                          \
                                                                             \
static void NAME##IdDestroy(void *key)                                       \
{                                                                            \
	MTP_FREE(key);                                                       \
}                                                                            \
                                                                             \
static void NAME##IdKeyCreate(void)                                          \
{                                                                            \
	pthread_key_create(&(NAME##_id_key), NAME##IdDestroy);               \
}                                                                            \
                                                                             \
static void NAME##IdCreate(void)                                             \
{                                                                            \
	static pthread_mutex_t inc_mutex = PTHREAD_MUTEX_INITIALIZER;        \
	static volatile unsigned int runner = 0;                             \
	signed int *thread_id = MTP_CALLOC(1, sizeof(signed int));           \
	                                                                     \
	pthread_mutex_lock(&(inc_mutex));                                    \
	*thread_id = (runner <= INT_MAX) ? (signed int) runner++ : -1;       \
	pthread_mutex_unlock(&(inc_mutex));                                  \
	pthread_once(&(NAME##_id_once), NAME##IdKeyCreate);                  \
	pthread_setspecific(NAME##_id_key, thread_id);                       \
}                                                                            \
                                                                             \
int NAME##GetThreadId(void)                                                  \
{                                                                            \
	int * const ret = pthread_getspecific(NAME##_id_key);                \
	                                                                     \
	return (ret != NULL) ? (*ret) : (-1);                                \
}                                                                            \
                                                                             \
void NAME##EnqueueJob(struct NAME##ThreadPool *pool, ElmType in)             \
{                                                                            \
	struct NAME##ThreadArgs tmp;                                         \
	                                                                     \
	tmp.terminate = MTP_FALSE;                                           \
	tmp.payload   = in;                                                  \
                                                                             \
	MTP_ENQUEUE_JOB(struct NAME##ThreadArgs, pool->queue, &tmp);         \
}                                                                            \
                                                                             \
void* NAME##ThreadRoutine(void *queue)                                       \
{                                                                            \
	struct NAME##ThreadArgs args = {0};                                  \
	                                                                     \
	NAME##IdCreate();                                                    \
	                                                                     \
	for (;;)                                                             \
	{                                                                    \
		struct NAME##JobQueue * const tmp                            \
			= (struct NAME##JobQueue *) queue;                   \
		                                                             \
		MTP_DEQUEUE_JOB(struct NAME##ThreadArgs, tmp, &args);        \
		                                                             \
		if (args.terminate == MTP_TRUE)                              \
		{                                                            \
			pthread_exit(0);                                     \
		}                                                            \
		                                                             \
		pthread_mutex_lock(&(tmp->work_mutex));                      \
		tmp->jobs_working++;                                         \
		pthread_mutex_unlock(&(tmp->work_mutex));                    \
		                                                             \
		ThreadFunc(args.payload);                                    \
		                                                             \
		pthread_mutex_lock(&(tmp->work_mutex));                      \
		tmp->jobs_working--;                                         \
		                                                             \
		if (tmp->jobs_working == 0)                                  \
		{                                                            \
			pthread_cond_broadcast(&(tmp->is_idle));             \
		}                                                            \
		                                                             \
		pthread_mutex_unlock(&(tmp->work_mutex));                    \
	}                                                                    \
}                                                                            \
                                                                             \
struct NAME##ThreadPool* NAME##NewThreadPool(const size_t num_threads,       \
	const size_t max_jobs)                                               \
{                                                                            \
	struct NAME##ThreadPool *pool = NULL;                                \
	size_t i;                                                            \
	                                                                     \
	if ((pool = MTP_CALLOC(1, sizeof(struct NAME##ThreadPool))) == NULL) \
	{                                                                    \
		return NULL;                                                 \
	}                                                                    \
	                                                                     \
	if ((pool->threads = MTP_CALLOC(num_threads, sizeof(pthread_t)))     \
		== NULL)                                                     \
	{                                                                    \
		MTP_FREE(pool);                                              \
		                                                             \
		return NULL;                                                 \
	}                                                                    \
	                                                                     \
	if ((pool->queue = MTP_CALLOC(1, sizeof(struct NAME##JobQueue)))     \
		== NULL)                                                     \
	{                                                                    \
		MTP_FREE(pool->threads);                                     \
		MTP_FREE(pool);                                              \
		                                                             \
		return NULL;                                                 \
	}                                                                    \
	                                                                     \
	if ((pool->queue->jobs = MTP_CALLOC(max_jobs,                        \
		sizeof(struct NAME##ThreadArgs))) == NULL)                   \
	{                                                                    \
		MTP_FREE(pool->queue);                                       \
		MTP_FREE(pool->threads);                                     \
		MTP_FREE(pool);                                              \
		                                                             \
		return NULL;                                                 \
	}                                                                    \
	                                                                     \
	pool->queue->jobs_max = max_jobs;                                    \
	pthread_cond_init(&(pool->queue->has_jobs), NULL);                   \
	pthread_cond_init(&(pool->queue->has_room), NULL);                   \
	pthread_cond_init(&(pool->queue->is_empty), NULL);                   \
	pthread_cond_init(&(pool->queue->is_idle),  NULL);                   \
	pthread_mutex_init(&(pool->queue->ring_mutex), NULL);                \
	pthread_mutex_init(&(pool->queue->work_mutex), NULL);                \
	                                                                     \
	for (i = 0; i < num_threads; i++)                                    \
	{                                                                    \
		pthread_create(&pool->threads[i], NULL, NAME##ThreadRoutine, \
			pool->queue);                                        \
	}                                                                    \
	                                                                     \
	pool->num_threads = num_threads;                                     \
	                                                                     \
	return pool;                                                         \
}                                                                            \
                                                                             \
void NAME##CleanupThreadPool(struct NAME##ThreadPool *pool)                  \
{                                                                            \
	size_t i;                                                            \
	struct NAME##ThreadArgs arg = {0};                                   \
	                                                                     \
	arg.terminate = MTP_TRUE;                                            \
	                                                                     \
	if (pool == NULL)                                                    \
	{                                                                    \
		return;                                                      \
	}                                                                    \
	                                                                     \
	if (pool->threads != NULL)                                           \
	{                                                                    \
		if ((pool->queue != NULL)                                    \
		&& (pool->queue->jobs != NULL))                              \
		{                                                            \
			for (i = 0; i < pool->num_threads; i++)              \
			{                                                    \
				MTP_ENQUEUE_JOB(struct NAME##ThreadArgs,     \
					pool->queue, &arg);                  \
			}                                                    \
									     \
			for (i = 0; i < pool->num_threads; i++)              \
			{                                                    \
				pthread_join(pool->threads[i], NULL);        \
			}                                                    \
		}                                                            \
								             \
		MTP_FREE(pool->threads);                                     \
	}                                                                    \
								             \
	if (pool->queue != NULL)                                             \
	{                                                                    \
		if (pool->queue->jobs != NULL)                               \
		{                                                            \
			MTP_FREE(pool->queue->jobs);                         \
		}                                                            \
								             \
		MTP_FREE(pool->queue);                                       \
	}                                                                    \
		                                                             \
	MTP_FREE(pool);                                                      \
	pool = NULL;                                                         \
}                                                                            \
                                                                             \
void NAME##WaitOnIdle(struct NAME##ThreadPool *pool)                         \
{                                                                            \
	struct NAME##JobQueue *queue = pool->queue;                          \
                                                                             \
	pthread_mutex_lock(&(queue->ring_mutex));                            \
	                                                                     \
	while (queue->jobs_waiting != 0)                                     \
	{                                                                    \
		pthread_cond_wait(&(queue->is_empty), &(queue->ring_mutex)); \
	}                                                                    \
	                                                                     \
	pthread_mutex_unlock(&(queue->ring_mutex));                          \
	pthread_mutex_lock(&(queue->work_mutex));                            \
	                                                                     \
	while (queue->jobs_working != 0)                                     \
	{                                                                    \
		pthread_cond_wait(&(queue->is_idle), &(queue->work_mutex));  \
	}                                                                    \
	                                                                     \
	pthread_mutex_unlock(&(queue->work_mutex));                          \
}                                                                            \
                                                                             \
enum {MTP_DEFINITIONS_DUMMY = 0}

/* ----------------------------- MIND THE GAP ----------------------------- */

#define MACRO_THREAD_POOL_COMPLETE(NAME, TYPE, FUNC) \
MACRO_THREAD_POOL_PROTOTYPES(NAME, TYPE);            \
MACRO_THREAD_POOL_DEFINITIONS(NAME, TYPE, FUNC);     \
enum {MTP_COMPLETE_DUMMY = 0}

#endif /* MACRO_THREAD_POOL_H */

/*
BSD 4-Clause License
Copyright (c) 2024, grauho <grauho@proton.me> All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    All advertising materials mentioning features or use of this software must
    display the following acknowledgement: This product includes software
    developed by the <copyright holder>.

    Neither the name of the <copyright holder> nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> AS IS AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
