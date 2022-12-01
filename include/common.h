#ifndef PMD_COMMON_H
#define PMD_COMMON_H

#include <stdio.h>
#include <stdlib.h>

/* Set starting buffer size (dynamic testing will start with this size) */
#define PMD_BUFFERSIZE 1024

/* Raise SIGTRAP on errors, not supported on DOS (and prolly others) */
#ifdef ENABLE_TRAP
	#include <signal.h>
	#define RAISE_TRAP raise(SIGTRAP)
#else
	#define RAISE_TRAP
#endif

/* Macro for dirty unused marking, mutes some warnings */
#ifdef DEBUG
	#define UNUSED(x) (void)(x);
#else
	#define UNUSED(x)
#endif


/* C89 bools */
typedef enum { false, true } boolean;

/*
 * Helper for allocating a utility buffer
 *
 * >0 - Size of buffer
 * -1 - Failed to allocate a buffer
 */
long PMD_GetBuffer (void** dest);

/*
 * Helper for reading data from a FILE* into a buffer
 *
 * -1 - dataLeft bogus (<=0)
 * -2 - Incomplete read
 * >0 - Count of read bytes
 */
long PMD_ReadBuffer (FILE* src, unsigned long dataLeft, void* dest, unsigned int destSize);

/*
 * Writes formatted string to error buffer
 */
void PMD_SetError (const char* printfMsg, ...);

/*
 * Const pointer to the error buffer
 */
const char* PMD_GetError (void);

/*
 * Prints contents of error buffer to stdout
 */
void PMD_PrintError (void);

/*
 * Prints a message for debugging
 */
void dbg_printf (const char* dbgSrc, const char* dbgMsg, ...);


/* Allocate & check for success */
#define ALLOC_CHECK(var, size)\
	var = calloc (size, sizeof (char));\
	if (var == NULL)

/* Signal allocation error */
#define ERROR_ALLOC(name, size)\
	PMD_SetError ("%s alloc (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

/* Macro for debug-only messages with varargs */
#ifdef DEBUG
	#define DO_MSG true
#else
	#define DO_MSG false
#endif
#define TRACE(x) do { if (DO_MSG) dbg_printf x; } while (0)

#endif /* PMD_COMMON_H */
