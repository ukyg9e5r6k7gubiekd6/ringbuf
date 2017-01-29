/*
 * ringbuf.h
 *
 * Circular ('ring') buffer.
 */

#ifndef RINGBUF_H_
#define RINGBUF_H_

#define RINGBUF_ASSIGN_DATA

struct ringbuf;
typedef struct ringbuf ringbuf;

typedef char datum;

struct ringbuf {
	datum *writepos;
	size_t size;
	datum *readpos;
	datum *arr;
};

void ringbuf_init(ringbuf *r, size_t size, datum *arr);
int ringbuf_enqueue(ringbuf *r, const datum *d);
int ringbuf_dequeue(ringbuf *r, datum *d);
void ringbuf_dump(const ringbuf *r);

#endif /* RINGBUF_H_ */
