/*
 * main.c
 *
 * Test harness for ring buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuf.h"

#define ELEMENTSOF(arr) (sizeof(arr) / sizeof((arr)[0]))

void dequeue_and_print(ringbuf *r) {
	char c;

	if (ringbuf_dequeue(r, &c) < 0) {
		fputs("Underflow\n", stderr);
	} else {
		fprintf(stderr, "Got %c\n", c);
	}
}

void test_overflow(const char *test_name)
{
	fprintf(stderr, "Test %s\n", test_name);

	ringbuf r;
	datum ringbuf_arr[5];
	memset(&r, 0, sizeof(r));
	memset(ringbuf_arr, 0, sizeof(ringbuf_arr));

	ringbuf_init(&r, ELEMENTSOF(ringbuf_arr), ringbuf_arr);
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

	ringbuf_fini(&r);
}

void test_underflow(const char *test_name)
{
	fprintf(stderr, "Test %s\n", test_name);

	ringbuf r;
	datum ringbuf_arr[5];
	memset(&r, 0, sizeof(r));
	memset(ringbuf_arr, 0, sizeof(ringbuf_arr));

	ringbuf_init(&r, ELEMENTSOF(ringbuf_arr), ringbuf_arr);
	ringbuf_dump(&r);

	ringbuf_enqueue(&r, "0");
	ringbuf_enqueue(&r, "1");
	ringbuf_enqueue(&r, "2");
	ringbuf_dump(&r);

	dequeue_and_print(&r);
	dequeue_and_print(&r);
	dequeue_and_print(&r);
#if 1
	dequeue_and_print(&r);
#endif
	ringbuf_dump(&r);

	ringbuf_fini(&r);
}

int main(int argc, char **argv)
{
	test_underflow("underflow");
	test_overflow("overflow");
	return EXIT_SUCCESS;
}
