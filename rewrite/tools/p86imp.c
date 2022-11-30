#include "p86.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#define EXENAME "p86imp"

void help (void);

void help (void) {
	printf ("%s: Test functionality of P86 code by\n", EXENAME);
	printf ("- loading raw sample files based on a printf pattern\n");
	printf ("- exporting the samples into new P86 file\n");
	printf ("\n");
	printf ("Usage:\t%s inpattern out.p86\n", EXENAME);
}

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

int main (int argc, char** argv) {
	unsigned short id;
	char* inFilnam;
	int filnamLen;
	unsigned int sampleLen;
	FILE* fileIn[256], * dest;
	p86_struct* p86 = P86_New();

	if (argc != 3) {
		help();
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	filnamLen = PMD_GetBuffer ((void**) &inFilnam);
	if (filnamLen <= 0) {
		printf ("Get filename buffer error\n");
		CLEANUP_P86;
		return RETVAL_MAIN;
	}


	for (id = 0; id <= 255; ++id) {
		sprintf (inFilnam, argv[1], id);
		fileIn[id] = fopen (inFilnam, "rb");
		if (fileIn[id] == NULL) continue;

		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_END, 0)) {
			printf ("Seek %s (in) end error\n", inFilnam);
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}
		if (!pmd_io_funcs.io_p (fileIn[id], &sampleLen)) {
			printf ("Pos %s (in) error\n", inFilnam);
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}
		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_SET, 0)) {
			printf ("Seek %s (in) start error\n", inFilnam);
			CLEANUP_FILEINS;
			return RETVAL_LIB;
		}

		P86_SetSample (p86, id, fileIn[id], 0, sampleLen);
	}

	dest = fopen (argv[2], "wb");
	if (dest == NULL) {
		printf ("Open %s (out) error\n", argv[2]);
		CLEANUP_FILEINS_POST;
		return RETVAL_LIB;
	}

	if (P86_Write (p86, dest)) {
		printf ("Write %s (out) error\n", argv[2]);
		CLEANUP_DEST;
		return RETVAL_LIB;
	}

	CLEANUP_DEST;
	return RETVAL_OK;
}
