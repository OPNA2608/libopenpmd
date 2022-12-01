#include "p86.h"

#include <stdio.h>

#define EXENAME "p86test"

void help (void);

void help (void) {
	printf ("%s: Test functionality of P86 code by\n", EXENAME);
	printf ("- loading a P86 file\n");
	printf ("- printing details about its contents\n");
	printf ("- exporting the parsed data into a new file with an identical file hash\n");
	printf ("\n");
	printf ("Usage:\t%s in.p86 out.p86\n", EXENAME);
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

#define CLEANUP_FILEIN\
	fclose (fileIn);\
	CLEANUP_P86;

#define CLEANUP_FILEOUT\
	fclose (fileOut);\
	CLEANUP_FILEIN;

int main (int argc, char** argv) {
	FILE* fileIn, * fileOut;
	p86_struct* p86 = P86_New();

	if (argc != 3) {
		CLEANUP_P86;
		help();
		return RETVAL_MAIN;
	}

	fileIn = fopen (argv[1], "rb");
	if (fileIn == NULL) {
		printf ("%s failed: Opening %s with read access!\n", EXENAME, argv[1]);
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	if (P86_Read (p86, fileIn)) {
		printf ("%s failed: Parsing %s!\n", EXENAME, argv[1]);
		CLEANUP_FILEIN;
		return RETVAL_LIB;
	}

	fileOut = fopen (argv[2], "wb");
	if (fileOut == NULL) {
		printf ("%s failed: Opening %s with write access!\n", EXENAME, argv[2]);
		CLEANUP_FILEIN;
		return RETVAL_MAIN;
	}

	if (P86_Write (p86, fileOut)) {
		printf ("%s failed: Writing parsed data to %s!\n", EXENAME, argv[2]);
		CLEANUP_FILEOUT;
		return RETVAL_LIB;
	}

	printf ("%s finished successfully!\n", EXENAME);
	CLEANUP_FILEOUT;
	return RETVAL_OK;
}
