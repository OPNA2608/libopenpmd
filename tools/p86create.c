#include <stdlib.h>
#include "p86.h"

#define OUTPUT_DEFAULT "TEST.P86"

int main (int argc, char* argv[]) {
	size_t readAmount;
	int i;
	FILE* fileHandle;
	p86_struct* newBank;
	unsigned long sampleSize;
	signed char* sampleBuffer;

	if (argc < 2) {
		printf ("Usage: %s sample1.raw sample2.raw ...\n\n", argv[0]);
		printf ("Sample files must fit internal P86 format & size limitations:\n");
		printf ("\tEncoding: 8-Bit Signed\n");
		printf ("\tPolyphony: Mono\n");
		printf ("\tSample rate: 16540Hz (recommended)\n");
		printf ("\tMax size: 16775663 Bytes (all samples added together)\n");
		return 1;
	}

	newBank = P86_New ();
	if (newBank == NULL) {
		printf ("ERROR: %s.\n", PMD_GetError());
		return 1;
	}

	for (i = 1; i < argc; ++i) {
		printf ("%s...\n", argv[i]);
		fileHandle = fopen (argv[i], "rb");
		if (fileHandle == NULL) {
			printf ("ERROR: Could not open file %s.\n", argv[i]);
			P86_Free (newBank);
			return 1;
		}
		fseek (fileHandle, 0, SEEK_END);
		sampleSize = ftell (fileHandle);
		fseek (fileHandle, 0, SEEK_SET);

		MALLOC_CHECK (sampleBuffer, sampleSize) {
			PMD_SetError (pmd_error_malloc, argv[1], sampleSize);
			P86_Free (newBank);
		}

		readAmount = fread (sampleBuffer, sizeof (char), sampleSize, fileHandle);
		if (readAmount < sampleSize) {
			printf ("ERROR: Failed to read %lu bytes from %s.\n", sampleSize, argv[i]);
			free (sampleBuffer);
			fclose (fileHandle);
			P86_Free (newBank);
			return 1;
		}

		P86_AddSample (newBank, sampleSize, sampleBuffer);

		free (sampleBuffer);
		fclose (fileHandle);
	}

	printf ("Exporting to %s.\n", OUTPUT_DEFAULT);
	fileHandle = fopen (OUTPUT_DEFAULT, "wb");
	P86_ExportFile (newBank, fileHandle);
	fclose (fileHandle);

	P86_Free (newBank);

	return 0;
}
