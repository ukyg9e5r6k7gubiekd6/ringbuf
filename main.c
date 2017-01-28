/*
 * main.c
 *
 * Test harness for ring buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuf.h"

void dequeue_and_print(ringbuf *r) {
	char c;

	if (ringbuf_dequeue(r, &c) < 0) {
		fputs("Underflow\n", stderr);
	} else {
		fprintf(stderr, "Got %c\n", c);
	}
}

void test_overflow(void)
{
	ringbuf r;
	memset(&r, 0, sizeof(r));

	ringbuf_init(&r);
	ringbuf_dump(&r);

	ringbuf_enqueue(&r, "0");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "1");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "2");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "3");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "4");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "5");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "6");
	ringbuf_dump(&r);
	ringbuf_enqueue(&r, "7");
	ringbuf_dump(&r);

	dequeue_and_print(&r);
	ringbuf_dump(&r);
}

void test_underflow()
{
	ringbuf r;
	memset(&r, 0, sizeof(r));

	ringbuf_init(&r);
	ringbuf_dump(&r);

	ringbuf_enqueue(&r, "0");
	ringbuf_enqueue(&r, "1");
	ringbuf_enqueue(&r, "2");
	ringbuf_dump(&r);

	dequeue_and_print(&r);
	dequeue_and_print(&r);
	dequeue_and_print(&r);
#if 0
	dequeue_and_print(&r);
#endif
	ringbuf_dump(&r);
}

int main(int argc, char **argv)
{
	test_underflow();
	return EXIT_SUCCESS;
}
