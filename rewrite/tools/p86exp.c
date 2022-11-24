#include "p86.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

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

	filnamLen = PMD_GetBuffer ((void**) &outFilnam);
	if (filnamLen <= 0) {
		printf ("Get filename buffer error\n");
		P86_Free (p86);
		return 1;
	}

	iobufLen = PMD_GetBuffer ((void**) &iobuf);
	if (iobufLen <= 0) {
		printf ("Get I/O buffer error\n");
		free (outFilnam);
		P86_Free (p86);
		return 1;
	}

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) {
		printf ("Open TEST.P86 error\n");
		free (iobuf);
		free (outFilnam);
		P86_Free (p86);
		return 1;
	}

	if (P86_Read (p86, fileIn)) {
		printf ("Parse TEST.P86 error\n");
		fclose (fileIn);
		free (iobuf);
		free (outFilnam);
		P86_Free (p86);
		return 2;
	}

	for (id = 0; id <= 255; ++id) {
		if (p86->samples[id] == NULL) continue;

		sprintf (outFilnam, "TEST-%03u.RAW", id);
		dest = fopen (outFilnam, "wb");
		if (dest == NULL) {
			perror("fopen() failed");
			printf ("Open %s error\n", outFilnam);
			fclose (fileIn);
			free (iobuf);
			free (outFilnam);
			P86_Free (p86);
			return 1;
		}

		sampleLen = p86->samples[id]->length;
		do {
			sampleRead = PMD_ReadBuffer (p86->samples[id]->src, sampleLen, iobuf, iobufLen);
			if (sampleRead == -1) {
				printf ("Read %s sample error\n", outFilnam);
				fclose (dest);
				fclose (fileIn);
				free (iobuf);
				free (outFilnam);
				P86_Free (p86);
				ERROR_READ ("P86 partial sample (into buffer)",
					(sampleLen > (unsigned long)iobufLen) ? (unsigned long)iobufLen : sampleLen
				);

				return 2;
			};

			WRITE_CHECK (iobuf, sizeof (char), sampleRead) {
				printf ("Write %s sample error\n", outFilnam);
				fclose (dest);
				fclose (fileIn);
				free (iobuf);
				free (outFilnam);
				P86_Free (p86);
				ERROR_WRITE ("P86 partial sample (from buffer)", sampleRead);
			}
			sampleLen -= sampleRead;
		} while (sampleLen != 0);

		fclose (dest);
	}

	fclose (fileIn);
	free (iobuf);
	free (outFilnam);
	P86_Free (p86);
	printf ("Done?\n");

	return 0;
}
