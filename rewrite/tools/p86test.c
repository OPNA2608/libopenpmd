#include "p86.h"

#include <stdio.h>

/*
	0 - OK
	1 - main error
	2 - library error
*/
int main (void) {
	FILE* fileIn, * fileOut;
	p86_struct* p86 = P86_New();

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) return 1;

	if (P86_Read (p86, fileIn)) {
		fclose (fileIn);
		P86_Free (p86);
		return 2;
	}

	fileOut = fopen ("TEST_.P86", "wb");
	if (fileOut == NULL) return 1;

	if (P86_Write (p86, fileOut)) {
		fclose (fileOut);
		P86_Free (p86);
		fclose (fileIn);
		return 2;
	}

	fclose (fileOut);
	P86_Free (p86);
	fclose (fileIn);

	return 0;
}
