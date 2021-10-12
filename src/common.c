#include "common.h"
#include <string.h>

static char pmd_error[PMD_ERRMAXSIZE];

/* TODO enum of error format strings? */
const char* pmd_error_malloc = "Failed to allocate memory for %s (length: %luB)";
const char* pmd_error_eof = "Reached end of file";

void PMD_SetError (const char* errorMsg) {
	strncpy (pmd_error, errorMsg, PMD_ERRMAXSIZE-1);
}

const char* PMD_GetError () {
	return pmd_error;
}
