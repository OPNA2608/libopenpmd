#include "p86.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

/*
	0 - OK
	1 - main error
	2 - library error
*/

#define RETVAL_OK   0
#define RETVAL_MAIN 1
#define RETVAL_LIB  2

#define CLEANUP_P86\
	P86_Free (p86);

#define CLEANUP_INFILNAM\
	free (inFilnam);\
	CLEANUP_P86;

#define CLEANUP_FILEINS\
	do {\
		if (fileIn[id] != NULL) fclose (fileIn[id]);\
	} while (id-- > 0);\
	CLEANUP_INFILNAM;

#define CLEANUP_FILEINS_POST\
	--id;\
	CLEANUP_FILEINS;

#define CLEANUP_DEST\
	fclose (dest);\
	CLEANUP_FILEINS_POST;

int main (void) {
	unsigned short id;
	char* inFilnam;
	int filnamLen;
	unsigned int sampleLen;
	FILE* fileIn[256], * dest;
	p86_struct* p86 = P86_New();

	filnamLen = PMD_GetBuffer ((void**) &inFilnam);
	if (filnamLen <= 0) {
		CLEANUP_P86;
		return RETVAL_MAIN;
	}


	for (id = 0; id <= 255; ++id) {
		sprintf (inFilnam, "TEST-%03u.RAW", id);
		fileIn[id] = fopen (inFilnam, "rb");
		if (fileIn[id] == NULL) continue;

		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_END, 0)) {
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}
		if (!pmd_io_funcs.io_p (fileIn[id], &sampleLen)) {
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}
		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_SET, 0)) {
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}

		P86_SetSample (p86, id, fileIn[id], 0, sampleLen);
	}

	dest = fopen ("TEST__.P86", "wb");
	if (dest == NULL) {
		CLEANUP_FILEINS_POST;
		return RETVAL_LIB;
	}

	if (P86_Write (p86, dest)) {
		CLEANUP_DEST;
		return RETVAL_LIB;
	}

	CLEANUP_DEST;
	return RETVAL_OK;
}
