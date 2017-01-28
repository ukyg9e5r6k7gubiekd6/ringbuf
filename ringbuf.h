/*
 * ringbuf.h
 *
 * Circular ('ring') buffer.
 */

#ifndef RINGBUF_H_
#define RINGBUF_H_

#define ELEMENTSOF(arr) (sizeof(arr) / sizeof((arr)[0]))

#define RINGBUF_SIZE 5
#define RINGBUF_ASSIGN_DATA

struct ringbuf;
typedef struct ringbuf ringbuf;

typedef char datum;

struct ringbuf {
	datum *writepos;
	datum arr[RINGBUF_SIZE];
	datum *readpos;
};

void ringbuf_init(ringbuf *r);
int ringbuf_enqueue(ringbuf *r, const datum *d);
int ringbuf_dequeue(ringbuf *r, datum *d);
void ringbuf_dump(const ringbuf *r);

#endif /* RINGBUF_H_ */
