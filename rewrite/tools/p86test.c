#include "p86.h"

#include <stdio.h>

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

int main (void) {
	FILE* fileIn, * fileOut;
	p86_struct* p86 = P86_New();

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) {
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	if (P86_Read (p86, fileIn)) {
		CLEANUP_FILEIN;
		return RETVAL_LIB;
	}

	fileOut = fopen ("TEST_.P86", "wb");
	if (fileOut == NULL) {
		CLEANUP_FILEIN;
		return RETVAL_MAIN;
	}

	if (P86_Write (p86, fileOut)) {
		CLEANUP_FILEOUT;
		return RETVAL_LIB;
	}

	CLEANUP_FILEOUT;
	return RETVAL_OK;
}
