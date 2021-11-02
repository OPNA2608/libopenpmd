#include <stdlib.h>
#include "p86.h"

/*
 * Possible return values:
 * * 0 - No Error
 * * 1 - Could not open file <outPath>
 * * 2 - Export function failed, see PMD_GetError() for details
 */
int export (const p86_struct* data, const char* outPath) {
	FILE* outFile;

	printf ("Testing exporting to %s...\n", outPath);

	outFile = fopen (outPath, "wb");
	if (outFile == NULL) {
		printf ("ERROR: Could not open file %s.\n", outPath);
		return 1;
	}

	if (P86_ExportFile (data, outFile) != 0) {
		fclose (outFile);
		printf ("ERROR: %s\n", PMD_GetError());
		return 2;
	}

	fclose (outFile);
	return 0;
}

int main (int argc, char* argv[]) {
	char* p86FileInput;
	const char* p86FileOutputMem = "TEST_MEM.P86";
	const char* p86FileOutputFil = "TEST_FIL.P86";
	FILE* inpFile;
	p86_struct* parsedData;
	boolean error = false;

	if (argc < 2) {
		printf ("Usage: /path/to/input.86\n");
		return 1;
	} else {
		p86FileInput = argv[1];
	}

	inpFile = fopen (p86FileInput, "rb");
	if (inpFile == NULL) {
		printf ("ERROR: Could not open file %s.\n", p86FileInput);
		return 1;
	}

	/* P86_ImportFile */

	printf ("Testing memory-heavy importing of %s...\n", p86FileInput);
	parsedData = P86_ImportFile (inpFile);
	if (parsedData != NULL) {
		if (export (parsedData, p86FileOutputMem) == 0) {
			printf ("Success!\n");
		} else {
			printf ("Export failed: %s\n", PMD_GetError());
			error = true;
		}
		P86_Free (parsedData);
	} else {
		printf ("Import failed: %s\n", PMD_GetError());
		error = true;
	}

	fseek (inpFile, 0, SEEK_SET);

	/* P86_ImportFileSlim */

	printf ("Testing memory-light importing of %s...\n", p86FileInput);
	parsedData = P86_ImportFileSlim (inpFile);
	if (parsedData != NULL) {
		if (export (parsedData, p86FileOutputFil) == 0) {
			printf ("Success!\n");
		} else {
			printf ("Export failed: %s\n", PMD_GetError());
			error = true;
		}
		P86_Free (parsedData);
	} else {
		printf ("Import failed: %s\n", PMD_GetError());
		error = true;
	}

	fclose (inpFile);

	printf ("Loadtest done.\n");
	return !error ? 0 : 1;
}
