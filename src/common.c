#include "common.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static char pmd_error[PMD_ERRMAXSIZE];

/* TODO enum of error format strings? */
const char* pmd_error_malloc = "Failed to allocate memory for %s (length: %luB)";
const char* pmd_error_eof = "Reached end of file";
const char* pmd_error_write = "Failed to write to file";

void PMD_SetError (const char* errorMsg, ...) {
	va_list fArgs;
	va_start (fArgs, errorMsg);
	snprintf (pmd_error, PMD_ERRMAXSIZE - 1, errorMsg, fArgs);
	va_end (fArgs);
}

const char* PMD_GetError () {
	return pmd_error;
}
