/*
 * ringbuf.c
 *
 * Circular ('ring') buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuf.h"

void ringbuf_init(ringbuf *r)
{
	r->readpos = NULL;
	r->writepos = &r->arr[0];
}

static inline datum *wrap_ptr(const ringbuf *r, datum *d)
{
	if (d >= &r->arr[ELEMENTSOF(r->arr)]) {
		return d - ELEMENTSOF(r->arr);
	}
	return d;
}

int ringbuf_enqueue(ringbuf *r, const datum *d)
{
	int overflow;
	const datum *new_writepos;

	new_writepos = wrap_ptr(r, r->writepos + 1);
	if (r->readpos != NULL && new_writepos == r->readpos) {
		/* full - throw away the oldest entry */
		r->readpos = wrap_ptr(r, r->readpos + 1);
		overflow = -1;
	} else {
		overflow = 0;
	}
#ifdef RINGBUF_ASSIGN_DATA
	*(r->writepos) = *d;
#else /* ndef RINGBUF_ASSIGN_DATA */
	memcpy(r->writepos, d, sizeof(*d));
#endif /* ndef RINGBUF_ASSIGN_DATA */
	if (NULL == r->readpos) {
		/* no longer empty */
		r->readpos = r->writepos;
	}
	r->writepos = wrap_ptr(r, r->writepos + 1);

	return overflow;
}

int ringbuf_dequeue(ringbuf *r, datum *d)
{
	int underflow;

	if (NULL == r->readpos) {
		/* empty */
		underflow = -1;
	} else {
		underflow = 0;
#ifdef RINGBUF_ASSIGN_DATA
		*d = *(r->readpos);
#else /* ndef RINGBUF_ASSIGN_DATA */
		memcpy(d, r->readpos, sizeof(*d));
#endif /* ndef RINGBUF_ASSIGN_DATA */
		datum *new_readpos = wrap_ptr(r, r->readpos + 1);
		if (new_readpos == r->writepos) {
			/* now empty */
			r->readpos = NULL;
		} else {
			r->readpos = new_readpos;
		}
	}

	return underflow;
}

void ringbuf_dump(const ringbuf *r)
{
	const datum *p;

	fprintf(stderr,
		"r=%ld, w=%ld\n",
		(NULL == r->readpos) ? -1 : (r->readpos - &r->arr[0]),
		(NULL == r->writepos) ? -1 : (r->writepos - &r->arr[0])
	);
	fputc('|', stderr);
	for (p = &r->arr[0]; p < &r->arr[ELEMENTSOF(r->arr)]; p++) {
		fputc(*p ? *p : ' ', stderr);
		fputc('|', stderr);
	}
	fputc('\n', stderr);

	const datum *lim;
	if (r->readpos != NULL && r->writepos != NULL) {
		lim = (r->readpos < r->writepos) ? r->readpos : r->writepos;
	} else if (r->readpos != NULL) {
		lim = r->readpos;
	} else if (r->writepos != NULL) {
		lim = r->writepos;
	} else {
		lim = NULL;
	}
	if (lim) {
		fputc(' ', stderr);
		for (p = &r->arr[0]; p < lim; p++) {
			fputs("  ", stderr);
		}
	}
	if (p == r->readpos) {
		fputs("r ", stderr);
		lim = r->writepos;
	} else if (p == r->writepos) {
		fputs("w ", stderr);
		lim = r->readpos;
	}
	if (lim) {
		for (p++; p < lim; p++) {
			fputs("  ", stderr);
		}
		if (p == r->readpos) {
			fputc('r', stderr);
		} else if (p == r->writepos) {
			fputc('w', stderr);
		}
	}
	fputc('\n', stderr);
	fputc('\n', stderr);
}
