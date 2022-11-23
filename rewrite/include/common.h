#ifndef PMD_COMMON_H
#define PMD_COMMON_H

#include <stdio.h>
#include <stdlib.h>

/* Raise SIGTRAP on errors, not supported on DOS (and prolly others) */
#ifdef ENABLE_TRAP
	#include <signal.h>
	#define RAISE_TRAP raise(SIGTRAP)
#else
	#define RAISE_TRAP
#endif

/* Enable macro for dirty unused marking, mutes some warnings */
#ifdef DEBUG
	#define UNUSED(x) (void)(x);
#else
	#define UNUSED(x)
#endif

/* Set starting buffer size (dynamic testing will start with this size) */
#define PMD_BUFFERSIZE 1024


/* C89 bools */
typedef enum { false, true } boolean;

/* I/O handlers */
/* Read count * size bytes from src to dest */
typedef boolean (*pmd_io_read) (FILE* src, void* dest, unsigned int size, unsigned int count);
/* Write count * size bytes from src to dest */
typedef boolean (*pmd_io_write) (const void* src, FILE* const dest, unsigned int size, unsigned int count);
/* Seek with mode to pos in src */
typedef boolean (*pmd_io_seek) (FILE* src, int mode, unsigned int loc);
/* Tell current position in src to pos */
typedef boolean (*pmd_io_pos) (FILE* src, unsigned int* pos);

typedef struct {
	pmd_io_read io_r;
	pmd_io_write io_w;
	pmd_io_seek io_s;
	pmd_io_pos io_p;
} pmd_io;

extern pmd_io pmd_io_funcs;

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
long PMD_ReadBuffer (FILE* src, long dataLeft, void* dest, unsigned int destSize);

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

#define ALLOC_CHECK(var, size)\
	var = calloc (size, sizeof (char));\
	if (var == NULL)

/* requires src */
#define READ_CHECK(var, elemsize, elemcount)\
	if (!pmd_io_funcs.io_r (src, var, elemsize, elemcount))

/* requires dest */
#define WRITE_CHECK(data, elemsize, elemcount)\
	if (!pmd_io_funcs.io_w (data, dest, elemsize, elemcount))

/* requires src */
#define SEEK_CHECK(mode, loc)\
	if (!pmd_io_funcs.io_s (src, mode, loc))

#define ERROR_ALLOC(name, size)\
	PMD_SetError ("%s alloc (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define ERROR_READ(name, size)\
	PMD_SetError ("%s read (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define ERROR_WRITE(name, size)\
	PMD_SetError ("%s write (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define ERROR_SEEK(name, mode, pos)\
	PMD_SetError ("%s seek (mode %d, pos %d) error", name, mode, pos);\
	PMD_PrintError();\
	RAISE_TRAP;

#endif /* PMD_COMMON_H */
