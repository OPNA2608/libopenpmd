#include <stdlib.h>
#include "p86.h"

#define OUTPUT_DEFAULT "TEST.P86"

/* memory leak somewhere */
int main (int argc, char* argv[]) {
	int i, dontcare;
	FILE* fileHandle;
	p86_struct newBank;
	long sampleSize;
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

	for (i = 1; i < argc; ++i) {
		printf ("%s...\n", argv[i]);
	  fileHandle = fopen (argv[i], "rb");
	  if (fileHandle == NULL) {
		  printf ("Could not open file %s.\n", argv[i]);
			P86_Free (&newBank);
		  return 1;
		}
    fseek (fileHandle, 0, SEEK_END);
		sampleSize = ftell (fileHandle);
		fseek (fileHandle, 0, SEEK_SET);

		MALLOC_CHECK (sampleBuffer, sampleSize) {
			MALLOC_ERROR (argv[i], sampleSize);
			P86_Free (&newBank);
		}

    dontcare = fread (sampleBuffer, sampleSize, sizeof (char), fileHandle);

		P86_AddSample (&newBank, sampleSize, sampleBuffer);
		fclose (fileHandle);
	}

	printf ("Blub: %d\n", dontcare);

	printf ("Exporting to %s.\n", OUTPUT_DEFAULT);
	fileHandle = fopen (OUTPUT_DEFAULT, "wb");
	P86_ExportFile (&newBank, fileHandle);
	fclose (fileHandle);

	P86_Free (&newBank);

	return 0;
}
