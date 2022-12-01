#include "common.h"

#include "io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

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
 * Helper for allocating a utility buffer
 *
 * -1 - Failed to allocate a buffer
 * >0 - Size of buffer
 */
long PMD_GetBuffer (void** dest) {
	long buf_size = PMD_BUFFERSIZE;
	TRACE (("PMD_GetBuffer", "Asked to allocate a buffer, starting at %ld", buf_size));

	while (buf_size > 0) {
		TRACE (("PMD_GetBuffer", "Trying to allocate a buffer of size %ld...", buf_size));
		ALLOC_CHECK (*dest, (unsigned  long)buf_size) {
			buf_size /= 2;
		} else {
			TRACE (("PMD_GetBuffer", "Got buffer of size %ld!", buf_size));
			break;
		}
	}
	if (buf_size == 0) TRACE (("PMD_GetBuffer", "Failed to get a buffer."));

	return buf_size != 0 ? buf_size : -1L;
}

/*
 * Helper for reading data from a FILE* into a buffer
 *
 * -1 - dataLeft bogus (<=0)
 * -2 - Incomplete read
 * >0 - Count of read bytes
 */
long PMD_ReadBuffer (FILE* src, unsigned long dataLeft, void* dest, unsigned int destSize) {
	/* TODO truncation potential */
	unsigned int readAmount = (unsigned int)dataLeft;
	if (readAmount == 0) return -1;
	if (dataLeft > destSize) readAmount = destSize;

	TRACE (("PMD_ReadBuffer", "Reading %uB into buffer", readAmount));
	READ_CHECK (dest, sizeof (char), readAmount) return -2;

	return readAmount;
}


/*
 * Writes formatted string to error buffer
 */
void PMD_SetError (const char* errorMsg, ...) {
	va_list fArgs;
	va_start (fArgs, errorMsg);

	if (pmd_error == NULL && pmd_init_error() != 0) {
		TRACE (("PMD_SetError", "Error init failed"));
		va_end (fArgs);
		return;
	}
	vsprintf (pmd_error, errorMsg, fArgs);
	TRACE (("PMD_SetError", pmd_error));
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

/*
 * Prints a message for debugging
 */
void dbg_printf (const char* dbgSrc, const char* dbgMsg, ...) {
	va_list fArgs;
	va_start (fArgs, dbgMsg);
	/*fprintf (stderr, "[%s] ", dbgSrc);
	vfprintf (stderr, dbgMsg, fArgs);
	fprintf (stderr, "\n");*/
	fprintf (stdout, "[%s] ", dbgSrc);
	vfprintf (stdout, dbgMsg, fArgs);
	fprintf (stdout, "\n");
	va_end (fArgs);
}
