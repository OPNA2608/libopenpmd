#include <stdlib.h>
#include "p86.h"

#define OUTPUT_PREFIX "TEST."

int main (int argc, char* argv[]) {
	int i;
	char* fileName;
	FILE* fileHandle;
	p86_struct* p86Bank;
	const p86_sample* p86Sample;

	if (argc < 2) {
		printf ("Usage: %s bank.P86\n\n", argv[0]);
		printf ("Extracted samples have the following raw format:\n");
		printf ("\tEncoding: 8-Bit Signed\n");
		printf ("\tPolyphony: Mono\n");
		printf ("\tSample rate: 16540Hz (usually)\n");
		return 1;
	}

	fileName = argv[1];
	fileHandle = fopen (fileName, "rb");
	if (fileHandle == NULL) {
		printf ("Could not open file %s.\n", fileName);
		return 1;
	}

	p86Bank = P86_ImportFile (fileHandle);
	fclose (fileHandle);

	if (p86Bank == NULL) {
		/* TODO print PMD_GetError() output */
		return 1;
	}

	for (i = 0; i < 255; ++i) {
		if (P86_IsSet (p86Bank, i) == 0) {
			printf ("%03d...\n", i);

			p86Sample = P86_GetSample (p86Bank, i);
			if (p86Sample == NULL) {
				printf ("Failed to acquire copy of sample data %03d.\n", i);
				P86_Free (p86Bank);
				return 1;
			}

			sprintf (fileName, "%s%03d.RAW", OUTPUT_PREFIX, i);
			fileHandle = fopen (fileName, "wb");
		  if (fileHandle == NULL) {
			  printf ("Could not open file %s.\n", fileName);
				P86_Free (p86Bank);
			  return 1;
			}

			fwrite (p86Sample->data, p86Sample->length, sizeof (char), fileHandle);
			if (ferror (fileHandle)) {
				printf ("Error occurred while writing to file.\n");
				fclose (fileHandle);
				P86_Free (p86Bank);
			  return 1;
			}

			fclose (fileHandle);
			printf ("Exported sample %s.\n", fileName);
		}
	}

	P86_Free (p86Bank);

	return 0;
}
