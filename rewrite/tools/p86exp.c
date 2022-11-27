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

#define CLEANUP_OUTFILNAM\
	free (outFilnam);\
	CLEANUP_P86;

#define CLEANUP_IOBUF\
	free (iobuf);\
	CLEANUP_OUTFILNAM;

#define CLEANUP_FILEIN\
	fclose (fileIn);\
	CLEANUP_IOBUF;

#define CLEANUP_DEST\
	fclose (dest);\
	CLEANUP_FILEIN;

int main (void) {
	unsigned short id;
	char* outFilnam, * iobuf;
	int filnamLen, iobufLen;
	unsigned int sampleLen;
	int sampleRead;
	FILE* fileIn, * dest;
	p86_struct* p86 = P86_New();

	filnamLen = PMD_GetBuffer ((void**) &outFilnam);
	if (filnamLen <= 0) {
		printf ("Get filename buffer error\n");
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	iobufLen = PMD_GetBuffer ((void**) &iobuf);
	if (iobufLen <= 0) {
		printf ("Get I/O buffer error\n");
		CLEANUP_OUTFILNAM;
		return RETVAL_MAIN;
	}

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) {
		printf ("Open TEST.P86 error\n");
		CLEANUP_IOBUF;
		return RETVAL_MAIN;
	}

	if (P86_Read (p86, fileIn)) {
		printf ("Parse TEST.P86 error\n");
		CLEANUP_FILEIN;
		return RETVAL_LIB;
	}

	for (id = 0; id <= 255; ++id) {
		if (p86->samples[id] == NULL) continue;

		sprintf (outFilnam, "TEST-%03u.RAW", id);
		dest = fopen (outFilnam, "wb");
		if (dest == NULL) {
			perror("fopen() failed"); // works on dos w/ ow, mingw/msvc windows?
			printf ("Open %s error\n", outFilnam);
			CLEANUP_FILEIN;
			return RETVAL_MAIN;
		}

		sampleLen = p86->samples[id]->length;
		do {
			sampleRead = PMD_ReadBuffer (p86->samples[id]->src, sampleLen, iobuf, iobufLen);
			if (sampleRead == -1) {
				printf ("Read %s sample error\n", outFilnam);
				ERROR_READ ("P86 partial sample (into buffer)",
					(sampleLen > (unsigned long)iobufLen) ? (unsigned long)iobufLen : sampleLen
				);
				CLEANUP_DEST;
				return RETVAL_LIB;
			};

			WRITE_CHECK (iobuf, sizeof (char), sampleRead) {
				printf ("Write %s sample error\n", outFilnam);
				ERROR_WRITE ("P86 partial sample (from buffer)", sampleRead);
				CLEANUP_DEST;
				return RETVAL_LIB;
			}
			sampleLen -= sampleRead;
		} while (sampleLen != 0);

		fclose (dest);
	}

	CLEANUP_FILEIN;
	return RETVAL_OK;
}
