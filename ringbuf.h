/*
 * ringbuf.h
 *
 * Circular ('ring') buffer.
 */

#ifndef RINGBUF_H_
#define RINGBUF_H_

#define RINGBUF_ASSIGN_DATA
#define RINGBUF_MUTEX

#ifdef RINGBUF_MUTEX
#include <pthread.h>
#endif /* RINGBUF_MUTEX */

struct ringbuf;
typedef struct ringbuf ringbuf;

typedef char datum;

struct ringbuf {
	datum *writepos;
	size_t size;
	datum *readpos;
#ifdef RINGBUF_MUTEX
	pthread_mutex_t mutex;
#endif /* RINGBUF_MUTEX */
	datum *arr;
};

void ringbuf_init(ringbuf *r, size_t size, datum *arr);
int ringbuf_enqueue(ringbuf *r, const datum *d);
int ringbuf_dequeue(ringbuf *r, datum *d);
void ringbuf_dump(const ringbuf *r);

#endif /* RINGBUF_H_ */
