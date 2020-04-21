#ifndef CVECTOR_long_H
#define CVECTOR_long_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Data structure for long vector. */
typedef struct cvector_long
{
	long* a;           /**< Array. */
	size_t size;       /**< Current size (amount you use when manipulating array directly). */
	size_t capacity;   /**< Allocated size of array; always >= size. */
} cvector_long;



extern size_t CVEC_long_SZ;

int cvec_long(cvector_long* vec, size_t size, size_t capacity);
int cvec_init_long(cvector_long* vec, long* vals, size_t num);

cvector_long* cvec_long_heap(size_t size, size_t capacity);
cvector_long* cvec_init_long_heap(long* vals, size_t num);

void cvec_long_copy(void* dest, void* src);

int cvec_push_long(cvector_long* vec, long a);
long cvec_pop_long(cvector_long* vec);

int cvec_extend_long(cvector_long* vec, size_t num);
int cvec_insert_long(cvector_long* vec, size_t i, long a);
int cvec_insert_array_long(cvector_long* vec, size_t i, long* a, size_t num);
long cvec_replace_long(cvector_long* vec, size_t i, long a);
void cvec_erase_long(cvector_long* vec, size_t start, size_t end);
int cvec_reserve_long(cvector_long* vec, size_t size);
int cvec_set_cap_long(cvector_long* vec, size_t size);
void cvec_set_val_sz_long(cvector_long* vec, long val);
void cvec_set_val_cap_long(cvector_long* vec, long val);

long* cvec_back_long(cvector_long* vec);

void cvec_clear_long(cvector_long* vec);
void cvec_free_long_heap(void* vec);
void cvec_free_long(void* vec);

#ifdef __cplusplus
}
#endif

/* CVECTOR_long_H */
#endif


#ifdef CVECTOR_long_IMPLEMENTATION

size_t CVEC_long_SZ = 50;

#define CVEC_long_ALLOCATOR(x) ((x+1) * 2)


#if defined(CVEC_MALLOC) && defined(CVEC_FREE) && defined(CVEC_REALLOC)
/* ok */
#elif !defined(CVEC_MALLOC) && !defined(CVEC_FREE) && !defined(CVEC_REALLOC)
/* ok */
#else
#error "Must define all or none of CVEC_MALLOC, CVEC_FREE, and CVEC_REALLOC."
#endif

#ifndef CVEC_MALLOC
#define CVEC_MALLOC(sz)      malloc(sz)
#define CVEC_REALLOC(p, sz)  realloc(p, sz)
#define CVEC_FREE(p)         free(p)
#endif

#ifndef CVEC_MEMMOVE
#include <string.h>
#define CVEC_MEMMOVE(dst, src, sz)  memmove(dst, src, sz)
#endif

#ifndef CVEC_ASSERT
#include <assert.h>
#define CVEC_ASSERT(x)       assert(x)
#endif

cvector_long* cvec_long_heap(size_t size, size_t capacity)
{
	cvector_long* vec;
	if (!(vec = (cvector_long*)malloc(sizeof(cvector_long)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_long_SZ;

	if (!(vec->a = (long*)malloc(vec->capacity*sizeof(long)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	return vec;
}

cvector_long* cvec_init_long_heap(long* vals, size_t num)
{
	cvector_long* vec;
	
	if (!(vec = (cvector_long*)malloc(sizeof(cvector_long)))) {
		assert(vec != NULL);
		return NULL;
	}

	vec->capacity = num + CVEC_long_SZ;
	vec->size = num;
	if (!(vec->a = (long*)malloc(vec->capacity*sizeof(long)))) {
		assert(vec->a != NULL);
		free(vec);
		return NULL;
	}

	memmove(vec->a, vals, sizeof(long)*num);

	return vec;
}

int cvec_long(cvector_long* vec, size_t size, size_t capacity)
{
	vec->size = size;
	vec->capacity = (capacity > vec->size || (vec->size && capacity == vec->size)) ? capacity : vec->size + CVEC_long_SZ;

	if (!(vec->a = (long*)malloc(vec->capacity*sizeof(long)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	return 1;
}

int cvec_init_long(cvector_long* vec, long* vals, size_t num)
{
	vec->capacity = num + CVEC_long_SZ;
	vec->size = num;
	if (!(vec->a = (long*)malloc(vec->capacity*sizeof(long)))) {
		assert(vec->a != NULL);
		vec->size = vec->capacity = 0;
		return 0;
	}

	memmove(vec->a, vals, sizeof(long)*num);

	return 1;
}

void cvec_long_copy(void* dest, void* src)
{
	cvector_long* vec1 = (cvector_long*)dest;
	cvector_long* vec2 = (cvector_long*)src;
	
	vec1->size = 0;
	vec1->capacity = 0;
	
	/*not much else we can do here*/
	if (!(vec1->a = (long*)malloc(vec2->capacity*sizeof(long)))) {
		assert(vec1->a != NULL);
		return;
	}
	
	memmove(vec1->a, vec2->a, vec2->size*sizeof(long));
	vec1->size = vec2->size;
	vec1->capacity = vec2->capacity;
}


int cvec_push_long(cvector_long* vec, long a)
{
	long* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		vec->a[vec->size++] = a;
	} else {
		tmp_sz = CVEC_long_ALLOCATOR(vec->capacity);
		if (!(tmp = (long*)realloc(vec->a, sizeof(long)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->a[vec->size++] = a;
		vec->capacity = tmp_sz;
	}
	return 1;
}

long cvec_pop_long(cvector_long* vec)
{
	return vec->a[--vec->size];
}

long* cvec_back_long(cvector_long* vec)
{
	return &vec->a[vec->size-1];
}

int cvec_extend_long(cvector_long* vec, size_t num)
{
	long* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_long_SZ;
		if (!(tmp = (long*)realloc(vec->a, sizeof(long)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	vec->size += num;
	return 1;
}

int cvec_insert_long(cvector_long* vec, size_t i, long a)
{
	long* tmp;
	size_t tmp_sz;
	if (vec->capacity > vec->size) {
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(long));
		vec->a[i] = a;
	} else {
		tmp_sz = CVEC_long_ALLOCATOR(vec->capacity);
		if (!(tmp = (long*)realloc(vec->a, sizeof(long)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		memmove(&vec->a[i+1], &vec->a[i], (vec->size-i)*sizeof(long));
		vec->a[i] = a;
		vec->capacity = tmp_sz;
	}

	vec->size++;
	return 1;
}

int cvec_insert_array_long(cvector_long* vec, size_t i, long* a, size_t num)
{
	long* tmp;
	size_t tmp_sz;
	if (vec->capacity < vec->size + num) {
		tmp_sz = vec->capacity + num + CVEC_long_SZ;
		if (!(tmp = (long*)realloc(vec->a, sizeof(long)*tmp_sz))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = tmp_sz;
	}

	memmove(&vec->a[i+num], &vec->a[i], (vec->size-i)*sizeof(long));
	memmove(&vec->a[i], a, num*sizeof(long));
	vec->size += num;
	return 1;
}

long cvec_replace_long(cvector_long* vec, size_t i, long a)
{
	long tmp = vec->a[i];
	vec->a[i] = a;
	return tmp;
}

void cvec_erase_long(cvector_long* vec, size_t start, size_t end)
{
	size_t d = end - start + 1;
	memmove(&vec->a[start], &vec->a[end+1], (vec->size-1-end)*sizeof(long));
	vec->size -= d;
}


int cvec_reserve_long(cvector_long* vec, size_t size)
{
	long* tmp;
	if (vec->capacity < size) {
		if (!(tmp = (long*)realloc(vec->a, sizeof(long)*(size+CVEC_long_SZ)))) {
			assert(tmp != NULL);
			return 0;
		}
		vec->a = tmp;
		vec->capacity = size + CVEC_long_SZ;
	}
	return 1;
}

int cvec_set_cap_long(cvector_long* vec, size_t size)
{
	long* tmp;
	if (size < vec->size) {
		vec->size = size;
	}

	if (!(tmp = (long*)realloc(vec->a, sizeof(long)*size))) {
		assert(tmp != NULL);
		return 0;
	}
	vec->a = tmp;
	vec->capacity = size;
	return 1;
}

void cvec_set_val_sz_long(cvector_long* vec, long val)
{
	size_t i;
	for (i=0; i<vec->size; i++) {
		vec->a[i] = val;
	}
}

void cvec_set_val_cap_long(cvector_long* vec, long val)
{
	size_t i;
	for (i=0; i<vec->capacity; i++) {
		vec->a[i] = val;
	}
}

void cvec_clear_long(cvector_long* vec) { vec->size = 0; }

void cvec_free_long_heap(void* vec)
{
	cvector_long* tmp = (cvector_long*)vec;
	if (!tmp) return;
	free(tmp->a);
	free(tmp);
}

void cvec_free_long(void* vec)
{
	cvector_long* tmp = (cvector_long*)vec;
	free(tmp->a);
	tmp->size = 0;
	tmp->capacity = 0;
}

#endif
