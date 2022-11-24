#include "p86.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#define RETVAL_OK   0
#define RETVAL_MAIN 1
#define RETVAL_LIB  2
/*
	0 - OK
	1 - main error
	2 - library error
*/
int main (void) {
	unsigned short id;
	char* outFilnam, * iobuf;
	int filnamLen, iobufLen;
	unsigned int sampleLen;
	int sampleRead;
	FILE* fileIn, * dest;
	p86_struct* p86 = P86_New();

	int retval = RETVAL_OK;

	filnamLen = PMD_GetBuffer ((void**) &outFilnam);
	if (filnamLen <= 0) {
		printf ("Get filename buffer error\n");
		retval = RETVAL_MAIN;
		goto cleanup_p86;
	}

	iobufLen = PMD_GetBuffer ((void**) &iobuf);
	if (iobufLen <= 0) {
		printf ("Get I/O buffer error\n");
		retval = RETVAL_MAIN;
		goto cleanup_outFilnam;
	}

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) {
		printf ("Open TEST.P86 error\n");
		retval = RETVAL_MAIN;
		goto cleanup_iobuf;
	}

	if (P86_Read (p86, fileIn)) {
		printf ("Parse TEST.P86 error\n");
		retval = RETVAL_LIB;
		goto cleanup_fileIn;
	}

	for (id = 0; id <= 255; ++id) {
		if (p86->samples[id] == NULL) continue;

		sprintf (outFilnam, "TEST-%03u.RAW", id);
		dest = fopen (outFilnam, "wb");
		if (dest == NULL) {
			perror("fopen() failed");
			printf ("Open %s error\n", outFilnam);
			retval = RETVAL_MAIN;
			goto cleanup_fileIn;
		}

		sampleLen = p86->samples[id]->length;
		do {
			sampleRead = PMD_ReadBuffer (p86->samples[id]->src, sampleLen, iobuf, iobufLen);
			if (sampleRead == -1) {
				printf ("Read %s sample error\n", outFilnam);
				ERROR_READ ("P86 partial sample (into buffer)",
					(sampleLen > (unsigned long)iobufLen) ? (unsigned long)iobufLen : sampleLen
				);
				fclose (dest);
				retval = RETVAL_LIB;
				goto cleanup_fileIn;
			};

			WRITE_CHECK (iobuf, sizeof (char), sampleRead) {
				printf ("Write %s sample error\n", outFilnam);
				ERROR_WRITE ("P86 partial sample (from buffer)", sampleRead);
				fclose (dest);
				retval = RETVAL_LIB;
				goto cleanup_fileIn;
			}
			sampleLen -= sampleRead;
		} while (sampleLen != 0);

		fclose (dest);
	}

cleanup_fileIn:
	fclose (fileIn);
cleanup_iobuf:
	free (iobuf);
cleanup_outFilnam:
	free (outFilnam);
cleanup_p86:
	P86_Free (p86);

	return retval;
}
