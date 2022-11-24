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
	char* inFilnam;
	int filnamLen;
	unsigned int sampleLen;
	FILE* fileIn[256], * dest;
	p86_struct* p86 = P86_New();

	filnamLen = PMD_GetBuffer ((void**) &inFilnam);
	if (filnamLen <= 0) {
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
		sprintf (inFilnam, "TEST-%03u.RAW", id);
		fileIn[id] = fopen (inFilnam, "rb");
		if (fileIn[id] == NULL) continue;

		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_END, 0)) {
			do {
				if (fileIn[id] != NULL) fclose (fileIn[id]);
			} while (id-- > 0);
			free (inFilnam);
			P86_Free (p86);
			return 2;
		}
		if (!pmd_io_funcs.io_p (fileIn[id], &sampleLen)) {
			do {
				if (fileIn[id] != NULL) fclose (fileIn[id]);
			} while (id-- > 0);
			free (inFilnam);
			P86_Free (p86);
			return 2;
		}
		if (!pmd_io_funcs.io_s (fileIn[id], SEEK_SET, 0)) {
			do {
				if (fileIn[id] != NULL) fclose (fileIn[id]);
			} while (id-- > 0);
			free (inFilnam);
			P86_Free (p86);
			return 2;
		}

		P86_SetSample (p86, id, fileIn[id], 0, sampleLen);
	}

	dest = fopen ("TEST__.P86", "wb");
	if (dest == NULL) {
		--id;
		do {
			if (fileIn[id] != NULL) fclose (fileIn[id]);
		} while (id-- > 0);
		free (inFilnam);
		P86_Free (p86);
		return 2;
	}

	if (P86_Write (p86, dest)) {
		fclose (dest);
		--id;
		do {
			if (fileIn[id] != NULL) fclose (fileIn[id]);
		} while (id-- > 0);
		free (inFilnam);
		P86_Free (p86);
		return 2;
	}

	fclose (dest);
	--id;
	do {
		if (fileIn[id] != NULL) fclose (fileIn[id]);
	} while (id-- > 0);
	free (inFilnam);
	P86_Free (p86);

	return 0;
}
