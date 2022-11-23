#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* Read count * size bytes from src to dest */
static boolean ioRead (FILE* source, void* dest, unsigned int size, unsigned int count) {
	size_t res = fread (dest, size, count, source);
	if (res != count) return false;
	/* TODO apply endianness correction or read byte-by-byte & reassemble */
	return true;
}

/* Write count * size bytes from src to dest */
static boolean ioWrite (const void* source, FILE* const dest, unsigned int size, unsigned int count) {
	const void* realData;
	if (false) {
		/* TODO apply endianness correction or read byte-by-byte & reassemble */
	} else {
		realData = source;
	}
	return fwrite (realData, size, count, dest) == count;
}

/* Seek with mode to pos in src */
static boolean ioSeek (FILE* source, int mode, unsigned int size) {
	return fseek (source, size, mode) == 0;
}

/* Tell current position in src to pos */
static boolean ioPos (FILE* source, unsigned int* pos) {
	long ret = ftell (source);
	if (ret == -1) return false;
	*pos = (unsigned int) ret;
	return true;
}

pmd_io pmd_io_funcs = {
	ioRead,
	ioWrite,
	ioSeek,
	ioPos
};

/*
 * Helper for allocating a utility buffer
 *
 * -1 - Failed to allocate a buffer
 * >0 - Size of buffer
 */
long PMD_GetBuffer (void** dest) {
	long buf_size = PMD_BUFFERSIZE;

	while (buf_size > 0) {
		ALLOC_CHECK (*dest, buf_size) {
			buf_size /= 2;
		} else {
#ifdef DEBUG
			printf ("Got buffer of size %ld\n", buf_size);
#endif
			break;
		}
	}
	return buf_size != 0 ? buf_size : -1;
}

/*
 * Helper for reading data from a FILE* into a buffer
 *
 * -1 - dataLeft bogus (<=0)
 * -2 - Incomplete read
 * >0 - Count of read bytes
 */
long PMD_ReadBuffer (FILE* src, long dataLeft, void* dest, unsigned int destSize) {
	unsigned long readAmount = dataLeft;
	if (readAmount < 1) return -1;
	if (dataLeft > destSize) readAmount = destSize;

	READ_CHECK (dest, sizeof (char), readAmount) return -2;

	return readAmount;
}


/* Buffer for error feedback & functions for interacting with it */

static char* pmd_error = NULL;
static long pmd_errorlength = -1L;

/*
 * Initialise the error buffer, internally called as needed.
 *
 * 0 - OK
 * 1 - already initialised
 * 2 - alloc error
 */
static int pmd_init_error (void) {
	if (pmd_error != NULL) return 1;
	pmd_errorlength = PMD_GetBuffer ((void**)&pmd_error);
	if (pmd_errorlength <= 0) return 2;
	return 0;
}

/*
 * Writes formatted string to error buffer
 */
void PMD_SetError (const char* errorMsg, ...) {
	va_list fArgs;
	va_start (fArgs, errorMsg);

	if (pmd_error == NULL && pmd_init_error() != 0) {
		printf ("Error init failed\n");
		va_end (fArgs);
		return;
	}
	vsprintf (pmd_error, errorMsg, fArgs);
	va_end (fArgs);
}

/*
 * Const pointer to the error buffer
 */
const char* PMD_GetError (void) {
	return pmd_error;
}

/*
 * Prints contents of error buffer to stdout
 */
void PMD_PrintError (void) {
	printf ("%s\n", pmd_error);
}
