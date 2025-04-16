/* License information at EOF */
/* A dynamically definable vector data structure implimentation */
/* This file uses CEV as its namespace to avoid using the all too common 'VEC',
 * if this approach is good enough for SQLite it's good enough for this */

/* CEV: Capable Enough Vectors */

#ifndef CEV_MACRO_H
#define CEV_MACRO_H

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#endif

#include <stddef.h> /* For NULL */

#ifdef CEV_MACRO_CUSTOM_ALLOC
#if !defined(CEV_MALLOC) || !defined(CEV_CALLOC) || !defined(CEV_REALLOC) \
	|| !defined(CEV_FREE)
#error "CEV_MACRO_CUSTOM_ALLOC: Must define CEV_{MALLOC,CALLOC,REALLOC,FREE}"
#endif
#else
#include <stdlib.h>
#define CEV_MALLOC  malloc
#define CEV_CALLOC  calloc
#define CEV_REALLOC realloc
#define CEV_FREE    free
#endif

#ifdef CEV_MACRO_CUSTOM_MEMMOVE
#if !defined(CEV_MEMMOVE) || !defined(CEV_MEMCOPY)
#error "CEV_MACRO_CUSTOM_MEMMOVE: Must define CEV_{MEMMOVE,MEMCOPY}"
#endif
#else
#include <string.h>
#define CEV_MEMMOVE memmove 
#define CEV_MEMCOPY memcpy 
#endif

#ifdef CEV_MACRO_CUSTOM_USIZE
#if !defined(CEV_USIZE) || !defined(CEV_UMAX) || !defined(CEV_UESC)
#error "CEV_MACRO_CUSTOM_USIZE: Must define CEV_{USIZE,UMAX,UESC}"
#endif
#else
#include <limits.h>
#define CEV_USIZE unsigned int 
#define CEV_UMAX  UINT_MAX
#define CEV_UESC  "%u"
#endif

#define CEV_BOOL    unsigned char
#define CEV_TRUE    (1)
#define CEV_FALSE   (0)

#define CEV_STAT    signed char 
#define CEV_SUCCESS (0)
#define CEV_FAILURE (1)
#define CEV_BADARGS (2)
#define CEV_NEXISTS (3)
#define CEV_EXISTS  (4)
#define CEV_EMPTY   (5)
#define CEV_OOB     (6)
#define CEV_FULLUP  (7)
#define CEV_ERRMEM  (-1)
#define CEV_ERRGEN  (-2)

#ifdef CEV_LOCAL_ONLY
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define CEV_API static inline
#else
#define CEV_API static
#endif
#else
#define CEV_API
#endif

#define CEV_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CEV_MAX(x, y) (((x) > (y)) ? (x) : (y)) 
#define CEV_CLAMP(min, x, max) (CEV_MAX((min), CEV_MIN((x), (max))))
#define CEV_ABS(x) (((x) < (0)) ? (-(x)) : (x)) /* UNUSED */
#define CEV_IS_FATAL(err) (((err) < 0) ? CEV_TRUE : CEV_FALSE)
#define CEV_IS_EMPTY(vec) ((((vec) != NULL) && ((vec)->len == 0)) \
	? CEV_TRUE : CEV_FALSE)

/* Things that vectors are expected to be able to do:
 * 	push,     pushes a new element to the vector
 * 	pop,      returns the front element and removes it from the vector
 * 	peek,     returns the front element without removal 
 * 	pushBack, pushes a new element to the end of the vector
 * 	popBack,  returns the end element and removes it from the vector
 * 	peekBack, returns the end element without removal
 * 	insert,   inserts the element at a desired index 
 * 	replace,  replaces the value at a given index, optional callback
 * 	remove,   removes an element at a desired index
 * 	index,    returns an element at a desired index without removal
 * 	trim,     removes unused allocated data space 
 * 	free,     frees the array, optional callback
 *
 * 	There is some question about what 'front' and 'back' should refer to.
 * 	If one is going to allow the user to access the data field directly
 * 	than front should refer to 0..len while if its going to be abstracted
 * 	it is probably more efficient to have front refer to len..0 
 *
 * 	Might change language to be append and prepend. No need to keep
 * 	C++ conventions when they're needlessly vague
 *
 * 	TODO:
 *
 * 	* Some version of replace that also expands the vector if
 * 	  the range goes off of the end of the vector. Although the 
 * 	  index will still need to be in (0..len)
 */

/* Because this does not depend on type it only needs to be defined once. If
 * one wishes it to be available across multiple translation units a simple
 * wrapper can be written for it */
#ifndef CEV_ERR_TO_STR 
#define CEV_ERR_TO_STR
CEV_API const char* cevErrorToString(const int err_code)
{
	const size_t index = CEV_CLAMP(0, err_code + 2, CEV_FULLUP);
	static const char * const lookup[] = 
	{
		"FATAL! General fatal error",
		"FATAL! Memory allocation fatal error",
		"Success",
		"General failure",
		"Bad function arguments",
		"Target does not exist",
		"Target already exists",
		"Vector is empty",
		"Access is out of bounds",
		"Vector is full",
	};

	return lookup[index];
}
#endif /* CEV_ERR_TO_STR */

#define CEV_MACRO_PROTOTYPES(NAME, type)                                     \
                                                                             \
struct NAME##Vector                                                          \
{                                                                            \
	CEV_USIZE len;                                                       \
	CEV_USIZE max;                                                       \
	type *data;                                                          \
};                                                                           \
                                                                             \
CEV_API struct NAME##Vector* NAME##VectorInit(const CEV_USIZE init_len);     \
CEV_API struct NAME##Vector* NAME##VectorInitWithData(                       \
	const type * const data, const CEV_USIZE init_len);                  \
CEV_API CEV_STAT NAME##VectorPushMany(struct NAME##Vector * const vec,       \
	const type * const val_arr, const CEV_USIZE len);                    \
CEV_API CEV_STAT NAME##VectorPush(struct NAME##Vector * const vec,           \
	type val);                                                           \
CEV_API CEV_STAT NAME##VectorPushBackMany(struct NAME##Vector * const vec,   \
	const type * const val_arr, const CEV_USIZE len);                    \
CEV_API CEV_STAT NAME##VectorPushBack(struct NAME##Vector * const vec,       \
	type val);                                                           \
CEV_API CEV_STAT NAME##VectorInsertMany(struct NAME##Vector * const vec,     \
	const CEV_USIZE index, const type * const val_arr,                   \
	const CEV_USIZE len);                                                \
CEV_API CEV_STAT NAME##VectorInsert(struct NAME##Vector * const vec,         \
	const CEV_USIZE index, type val);                                    \
CEV_API CEV_STAT NAME##VectorReplaceWithCallback(                            \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	type new_val, void (*Callback)(type, void *), void *user_data);      \
CEV_API CEV_STAT NAME##VectorReplace(struct NAME##Vector * const vec,        \
	const CEV_USIZE index, type new_val);                                \
CEV_API CEV_STAT NAME##VectorIndex(struct NAME##Vector * const vec,          \
	const CEV_USIZE index, type * const out);                            \
CEV_API CEV_STAT NAME##VectorPop(struct NAME##Vector * const vec,            \
	type * const out);                                                   \
CEV_API CEV_STAT NAME##VectorPopBack(struct NAME##Vector * const vec,        \
	type * const out);                                                   \
CEV_API CEV_STAT NAME##VectorPeek(struct NAME##Vector * const vec,           \
	type * const out);                                                   \
CEV_API CEV_STAT NAME##VectorPeekBack(struct NAME##Vector * const vec,       \
	type * const out);                                                   \
CEV_API CEV_STAT NAME##VectorRemoveMany(struct NAME##Vector * const vec,     \
	const CEV_USIZE index, const CEV_USIZE num);                         \
CEV_API CEV_STAT NAME##VectorRemove(struct NAME##Vector * const vec,         \
	const CEV_USIZE index);                                              \
CEV_API CEV_STAT NAME##VectorRemoveManyWithCallback(                         \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	const CEV_USIZE num, void (*Callback)(type, void *),                 \
	void *user_data);                                                    \
CEV_API CEV_STAT NAME##VectorRemoveWithCallback(                             \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	void (*Callback)(type, void *), void *user_data);                    \
CEV_API CEV_STAT NAME##VectorLength(const struct NAME##Vector * const vec,   \
	CEV_USIZE * const out);                                              \
CEV_API CEV_STAT NAME##VectorTrim(struct NAME##Vector * const vec);          \
CEV_API void NAME##VectorFree(struct NAME##Vector * const vec);              \
CEV_API void NAME##VectorFreeWithCallback(struct NAME##Vector * const vec,   \
	void (*Callback)(type, void *), void *user_data);                    \
CEV_API CEV_STAT NAME##VectorExpand(struct NAME##Vector * const vec);        \
                                                                             \
enum {NAME##_CEV_MACRO_PROTOTYPE_DUMMY = 0} 

/* XXX -------------------------- MIND THE GAP ------------------------- XXX */

#define CEV_MACRO_DEFINITIONS(NAME, type)                                    \
                                                                             \
CEV_API struct NAME##Vector* NAME##VectorInit(const CEV_USIZE init_len)      \
{                                                                            \
	struct NAME##Vector *ret = CEV_MALLOC(sizeof(struct NAME##Vector));  \
	                                                                     \
	if (ret != NULL)                                                     \
	{                                                                    \
		ret->len = 0;                                                \
		ret->max = init_len;                                         \
		                                                             \
		if (init_len == 0)                                           \
		{                                                            \
			ret->data = NULL;                                    \
		}                                                            \
		else if ((ret->data = CEV_MALLOC(init_len * sizeof(type)))   \
			== NULL)                                             \
		{                                                            \
			CEV_FREE(ret);                                       \
			ret = NULL;                                          \
		}                                                            \
	}                                                                    \
	                                                                     \
	return ret;                                                          \
}                                                                            \
                                                                             \
CEV_API struct NAME##Vector* NAME##VectorInitWithData(                       \
	const type * const data, const CEV_USIZE init_len)                   \
{                                                                            \
	struct NAME##Vector *ret = NAME##VectorInit(init_len);               \
	                                                                     \
	if ((ret != NULL) && (data != NULL))                                 \
	{                                                                    \
		CEV_MEMCOPY(ret->data, data, init_len * sizeof(type));       \
		ret->len = init_len;                                         \
	}                                                                    \
	                                                                     \
	return ret;                                                          \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorExpand(struct NAME##Vector * const vec)         \
{                                                                            \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	else if (vec->len == CEV_UMAX)                                       \
	{                                                                    \
		return CEV_FULLUP;                                           \
	}                                                                    \
	else                                                                 \
	{                                                                    \
		const CEV_USIZE new_size = (vec->max > 1)                    \
			? CEV_MIN((vec->max * 3) >> 1, CEV_UMAX)             \
			: 2;                                                 \
		type * const tmp = CEV_REALLOC(vec->data,                    \
			new_size * sizeof(type));                            \
		                                                             \
		if (tmp == NULL)                                             \
		{                                                            \
			return CEV_ERRMEM;                                   \
		}                                                            \
		                                                             \
		vec->data = tmp;                                             \
		vec->max = new_size;                                         \
	}                                                                    \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorTrim(struct NAME##Vector * const vec)           \
{                                                                            \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	else                                                                 \
	{                                                                    \
		type * const tmp = CEV_REALLOC(vec->data,                    \
			vec->len * sizeof(type));                            \
		                                                             \
		if (tmp == NULL)                                             \
		{                                                            \
			return CEV_ERRMEM;                                   \
		}                                                            \
		                                                             \
		vec->data = tmp;                                             \
		vec->max = vec->len;                                         \
	}                                                                    \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorInsertMany(struct NAME##Vector * const vec,     \
	const CEV_USIZE index, const type * const val_arr,                   \
	const CEV_USIZE len)                                                 \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if ((vec == NULL) || (val_arr == NULL))                              \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (index > vec->len)                                                \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	while (vec->max < vec->len + len)                                    \
	{                                                                    \
		if ((ret = NAME##VectorExpand(vec)) != CEV_SUCCESS)          \
		{                                                            \
			return ret;                                          \
		}                                                            \
	}                                                                    \
	                                                                     \
	if (index < vec->len)                                                \
	{                                                                    \
		CEV_MEMMOVE(&(vec->data[index + len]), &(vec->data[index]),  \
			(vec->len - index) * sizeof(type));                  \
	}                                                                    \
	                                                                     \
	CEV_MEMCOPY(&(vec->data[index]), val_arr, len * sizeof(type));       \
	vec->len += len;                                                     \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorInsert(struct NAME##Vector * const vec,         \
	const CEV_USIZE index, type val)                                     \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (index > vec->len)                                                \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	if ((vec->len == vec->max)                                           \
	&& ((ret = NAME##VectorExpand(vec)) != CEV_SUCCESS))                 \
	{                                                                    \
		return ret;                                                  \
	}                                                                    \
	                                                                     \
	if (index < vec->len)                                                \
	{                                                                    \
		CEV_MEMMOVE(&(vec->data[index + 1]), &(vec->data[index]),    \
			(vec->len - index) * sizeof(type));                  \
	}                                                                    \
	                                                                     \
	vec->data[index] = val;                                              \
	vec->len++;                                                          \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorReplaceWithCallback(                            \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	type new_val, void (*Callback)(type, void *), void *user_data)       \
{                                                                            \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (vec->len == 0)                                                   \
	{                                                                    \
		return CEV_EMPTY;                                            \
	}                                                                    \
	                                                                     \
	if (index >= vec->len)                                               \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	if (Callback != NULL)                                                \
	{                                                                    \
		Callback(vec->data[index], user_data);                       \
	}                                                                    \
	                                                                     \
	vec->data[index] = new_val;                                          \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorReplace(struct NAME##Vector * const vec,        \
	const CEV_USIZE index, type new_val)                                 \
{                                                                            \
	return NAME##VectorReplaceWithCallback(vec, index, new_val, NULL,    \
		NULL);                                                       \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPush(struct NAME##Vector * const vec,           \
	type val)                                                            \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if ((vec->len == vec->max)                                           \
	&& ((ret = NAME##VectorExpand(vec)) != CEV_SUCCESS))                 \
	{                                                                    \
		return ret;                                                  \
	}                                                                    \
	                                                                     \
	vec->data[vec->len++] = val;                                         \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPushMany(struct NAME##Vector * const vec,       \
	const type * const val_arr, const CEV_USIZE len)                     \
{                                                                            \
	return NAME##VectorInsertMany(vec, vec->len, val_arr, len);          \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPushBack(struct NAME##Vector * const vec,       \
	type val)                                                            \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if ((vec->len == vec->max)                                           \
	&& ((ret = NAME##VectorExpand(vec)) != CEV_SUCCESS))                 \
	{                                                                    \
		return ret;                                                  \
	}                                                                    \
	                                                                     \
	CEV_MEMMOVE(&(vec->data[1]), &(vec->data[0]),                        \
		vec->len * sizeof(type));                                    \
	vec->data[0] = val;                                                  \
	vec->len++;                                                          \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPushBackMany(struct NAME##Vector * const vec,   \
	const type * const val_arr, const CEV_USIZE len)                     \
{                                                                            \
	return NAME##VectorInsertMany(vec, 0, val_arr, len);                 \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorIndex(struct NAME##Vector * const vec,          \
	const CEV_USIZE index, type * const out)                             \
{                                                                            \
	if ((vec == NULL) || (vec->data == NULL) || (out == NULL))           \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (vec->len == 0)                                                   \
	{                                                                    \
		return CEV_EMPTY;                                            \
	}                                                                    \
	                                                                     \
	if (index >= vec->len)                                               \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	*out = vec->data[index];                                             \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPeek(struct NAME##Vector * const vec,           \
	type * const out)                                                    \
{                                                                            \
	if ((vec == NULL) && (vec->len == 0))                                \
	{                                                                    \
		return CEV_EMPTY;                                            \
	}                                                                    \
                                                                             \
	return NAME##VectorIndex(vec, vec->len - 1, out);                    \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPeekBack(struct NAME##Vector * const vec,       \
	type * const out)                                                    \
{                                                                            \
	return NAME##VectorIndex(vec, 0, out);                               \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorPopBack(struct NAME##Vector * const vec,        \
	type * const out)                                                    \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if ((ret = NAME##VectorPeekBack(vec, out)) == CEV_SUCCESS)           \
	{                                                                    \
		vec->len--;                                                  \
		CEV_MEMMOVE(&(vec->data[0]), &(vec->data[1]),                \
			vec->len * sizeof(type));                            \
	}                                                                    \
	                                                                     \
	return ret;                                                          \
}                                                                            \
                                                                             \
                                                                             \
CEV_API CEV_STAT NAME##VectorPop(struct NAME##Vector * const vec,            \
	type * const out)                                                    \
{                                                                            \
	CEV_STAT ret;                                                        \
	                                                                     \
	if ((ret = NAME##VectorPeek(vec, out)) == CEV_SUCCESS)               \
	{                                                                    \
		vec->len--;                                                  \
	}                                                                    \
	                                                                     \
	return ret;                                                          \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorRemoveMany(struct NAME##Vector * const vec,     \
	const CEV_USIZE index, const CEV_USIZE num)                          \
{                                                                            \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (vec->len == 0)                                                   \
	{                                                                    \
		return CEV_EMPTY;                                            \
	}                                                                    \
	                                                                     \
	if ((index >= vec->len) || (index + num > vec->len))                 \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	if (index + num < vec->len)                                          \
	{                                                                    \
		CEV_MEMMOVE(&(vec->data[index]), &(vec->data[index + num]),  \
			(vec->len - index - num) * sizeof(type));            \
	}                                                                    \
	                                                                     \
	vec->len -= num;                                                     \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorRemove(struct NAME##Vector * const vec,         \
	const CEV_USIZE index)                                               \
{                                                                            \
	return NAME##VectorRemoveMany(vec, index, 1);                        \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorRemoveManyWithCallback(                         \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	const CEV_USIZE num, void (*Callback)(type, void *),                 \
	void *user_data)                                                     \
{                                                                            \
	if (vec == NULL)                                                     \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	if (vec->len == 0)                                                   \
	{                                                                    \
		return CEV_EMPTY;                                            \
	}                                                                    \
	                                                                     \
	if ((index >= vec->len) || (index + num > vec->len))                 \
	{                                                                    \
		return CEV_OOB;                                              \
	}                                                                    \
	                                                                     \
	if (Callback != NULL)                                                \
	{                                                                    \
		CEV_USIZE i;                                                 \
									     \
		for (i = 0; i < num; i++)                                    \
		{                                                            \
			Callback(vec->data[index + i], user_data);           \
		}                                                            \
	}                                                                    \
	                                                                     \
	if (index + num < vec->len)                                          \
	{                                                                    \
		CEV_MEMMOVE(&(vec->data[index]), &(vec->data[index + num]),  \
			(vec->len - index - num) * sizeof(type));            \
	}                                                                    \
	                                                                     \
	vec->len -= num;                                                     \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorRemoveWithCallback(                             \
	struct NAME##Vector * const vec, const CEV_USIZE index,              \
	void (*Callback)(type, void *), void *user_data)                     \
{                                                                            \
	return NAME##VectorRemoveManyWithCallback(vec, index, 1, Callback,   \
		user_data);                                                  \
}                                                                            \
                                                                             \
CEV_API CEV_STAT NAME##VectorLength(const struct NAME##Vector * const vec,   \
	CEV_USIZE * const out)                                               \
{                                                                            \
	if ((vec == NULL) || (out == NULL))                                  \
	{                                                                    \
		return CEV_BADARGS;                                          \
	}                                                                    \
	                                                                     \
	*out = vec->len;                                                     \
	                                                                     \
	return CEV_SUCCESS;                                                  \
}                                                                            \
                                                                             \
CEV_API void NAME##VectorFree(struct NAME##Vector * const vec)               \
{                                                                            \
	if (vec != NULL)                                                     \
	{                                                                    \
		if (vec->data != NULL)                                       \
		{                                                            \
			CEV_FREE(vec->data);                                 \
		}                                                            \
		                                                             \
		CEV_FREE(vec);                                               \
	}                                                                    \
}                                                                            \
                                                                             \
CEV_API void NAME##VectorFreeWithCallback(struct NAME##Vector * const vec,   \
	void (*Callback)(type, void *), void *user_data)                     \
{                                                                            \
	if (vec != NULL)                                                     \
	{                                                                    \
		if (vec->data != NULL)                                       \
		{                                                            \
			if (Callback != NULL)                                \
			{                                                    \
				CEV_USIZE i;                                 \
				                                             \
				for (i = 0; i < vec->len; i++)               \
				{                                            \
					Callback(vec->data[i], user_data);   \
				}                                            \
			}                                                    \
			                                                     \
			CEV_FREE(vec->data);                                 \
		}                                                            \
		                                                             \
		CEV_FREE(vec);                                               \
	}                                                                    \
}                                                                            \
                                                                             \
enum {NAME##_CEV_MACRO_DEFINITION_DUMMY = 0} 

/* XXX -------------------------- MIND THE GAP ------------------------- XXX */

#define CEV_MACRO_COMPLETE(NAME, type)     \
CEV_MACRO_PROTOTYPES(NAME, type);          \
CEV_MACRO_DEFINITIONS(NAME, type);         \
enum {NAME##_CEV_MACRO_COMPLETE_DUMMY = 0}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#else
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif

#endif /* CEV_MACRO_H */

/*
BSD 4-Clause License
Copyright (c) 2025, grauho <grauho@proton.me> All rights reserved.

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
