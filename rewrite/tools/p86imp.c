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
	char* inFilnam, * iobuf;
	int filnamLen, iobufLen;
	unsigned int sampleLen;
	int sampleRead;
	FILE* fileIn, * dest;
	p86_struct* p86 = P86_New();

	filnamLen = PMD_GetBuffer ((void**) &inFilnam);
	if (filnamLen <= 0) {
		P86_Free (p86);
		return 1;
	}

	iobufLen = PMD_GetBuffer ((void**) &iobuf);
	if (iobufLen <= 0) {
		free (inFilnam);
		P86_Free (p86);
		return 1;
	}


/*
	if (P86_Read (p86, fileIn)) {
		fclose (fileIn);
		free (iobuf);
		free (inFilnam);
		P86_Free (p86);
		return 2;
	}
*/

	for (id = 0; id <= 255; ++id) {
		/*if (p86->samples[id] == NULL) continue;*/

		sprintf (inFilnam, "TEST-%03u.RAW", id);
		fileIn = fopen (inFilnam, "rb");
		if (fileIn == NULL) continue;

		dest = fopen (inFilnam, "wb");
		if (dest == NULL) {
			fclose (fileIn);
			free (iobuf);
			free (inFilnam);
			P86_Free (p86);
			return 1;
		}

		sampleLen = p86->samples[id]->length;
		do {
			sampleRead = PMD_ReadBuffer (p86->samples[id]->src, sampleLen, iobuf, iobufLen);
			if (sampleRead == -1) {
				fclose (dest);
				fclose (fileIn);
				free (iobuf);
				free (inFilnam);
				P86_Free (p86);
				ERROR_READ ("P86 partial sample (into buffer)",
					(sampleLen > (unsigned long)iobufLen) ? (unsigned long)iobufLen : sampleLen
				);

				return 2;
			};

			WRITE_CHECK (iobuf, sizeof (char), sampleRead) {
				fclose (dest);
				fclose (fileIn);
				free (iobuf);
				free (inFilnam);
				P86_Free (p86);
				ERROR_WRITE ("P86 partial sample (from buffer)", sampleRead);
			}
			sampleLen -= sampleRead;
		} while (sampleLen != 0);

		fclose (dest);
	}

	fclose (fileIn);
	free (iobuf);
	free (inFilnam);
	P86_Free (p86);

	return 0;
}
