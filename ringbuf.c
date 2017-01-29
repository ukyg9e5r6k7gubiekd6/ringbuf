/*
 * ringbuf.c
 *
 * Circular ('ring') buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuf.h"

void ringbuf_init(ringbuf *r, size_t size, datum *arr)
{
	r->size = size;
	r->readpos = NULL;
	r->arr = arr;
	r->writepos = &r->arr[0];
#ifdef RINGBUF_MUTEX
	pthread_mutex_init(&r->mutex, NULL);
#endif /* RINGBUF_MUTEX */
}

static inline datum *wrap_ptr(const ringbuf *r, datum *d)
{
	if (d >= &r->arr[r->size]) {
		return d - r->size;
	}
	return d;
}

int ringbuf_enqueue(ringbuf *r, const datum *d)
{
	int overflow;
	const datum *new_writepos;

#ifdef RINGBUF_MUTEX
	if (pthread_mutex_lock(&r->mutex)) {
		perror("pthread_mutex_lock");
		return -2;
	}
#endif /* RINGBUF_MUTEX */

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

#ifdef RINGBUF_MUTEX
	if (pthread_mutex_unlock(&r->mutex) < 0) {
		perror("pthread_mutex_unlock");
		return -2;
	}
#endif /* RINGBUF_MUTEX */

	return overflow;
}

int ringbuf_dequeue(ringbuf *r, datum *d)
{
	int underflow;

#ifdef RINGBUF_MUTEX
	if (pthread_mutex_lock(&r->mutex) < 0) {
		perror("pthread_mutex_lock");
		return -2;
	}
#endif /* RINGBUF_MUTEX */

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

#ifdef RINGBUF_MUTEX
	if (pthread_mutex_unlock(&r->mutex) < 0) {
		perror("pthread_mutex_unlock");
		return -2;
	}
#endif /* RINGBUF_MUTEX */

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
	for (p = &r->arr[0]; p < &r->arr[r->size]; p++) {
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
