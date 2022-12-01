#include "p86.h"
#include "common.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#define EXENAME "p86exp"

void help (void);

void help (void) {
	printf ("%s: Test functionality of P86 code by\n", EXENAME);
	printf ("- loading a P86 file\n");
	printf ("- exporting the parsed samples into new files based on a printf pattern\n");
	printf ("\n");
	printf ("Usage:\t%s in.p86 outpattern\n", EXENAME);
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

#define CLEANUP_OUTFILNAM\
	free (outFilnam);\
	CLEANUP_P86;

#define CLEANUP_IOBUF\
	free (iobuf);\
	CLEANUP_OUTFILNAM;

#define CLEANUP_FILEIN\
	fclose (fileIn);\
	CLEANUP_IOBUF;

#define CLEANUP_DEST\
	fclose (dest);\
	CLEANUP_FILEIN;

int main (int argc, char**argv) {
	unsigned short id;
	char* outFilnam, * iobuf;
	unsigned long sampleLen;
	long iobufLen, filnamLen, sampleRead;
	FILE* fileIn, * dest;
	p86_struct* p86 = P86_New();

	TRACE ((EXENAME, "Checking cmdline args"));
	if (argc != 3) {
		help();
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	TRACE ((EXENAME, "Requesting output filename buffer"));
	filnamLen = (int)PMD_GetBuffer ((void**) &outFilnam);
	if (filnamLen <= 0) {
		printf ("Get filename buffer error\n");
		CLEANUP_P86;
		return RETVAL_MAIN;
	}

	TRACE ((EXENAME, "Requesting I/O buffer"));
	iobufLen = PMD_GetBuffer ((void**) &iobuf);
	if (iobufLen <= 0) {
		printf ("Get I/O buffer error\n");
		CLEANUP_OUTFILNAM;
		return RETVAL_MAIN;
	}

	TRACE ((EXENAME, "Opening input file"));
	fileIn = fopen (argv[1], "rb");
	if (fileIn == NULL) {
		printf ("Open %s (in) error\n", argv[1]);
		CLEANUP_IOBUF;
		return RETVAL_MAIN;
	}

	TRACE ((EXENAME, "Parsing input file"));
	if (P86_Read (p86, fileIn)) {
		printf ("Parse %s (in) error\n", argv[1]);
		CLEANUP_FILEIN;
		return RETVAL_LIB;
	}

	TRACE ((EXENAME, "Looping over loaded samples, looking for set ones"));
	for (id = 0; id <= 255; ++id) {
		if (p86->samples[id] == NULL) continue;
		TRACE ((EXENAME, "Found set sample #%03u", id));

		TRACE ((EXENAME, "Building output name"));
		sprintf (outFilnam, argv[2], id);

		TRACE ((EXENAME, "Opening output file"));
		dest = fopen (outFilnam, "wb");
		if (dest == NULL) {
			perror("fopen() failed"); /* works on dos w/ ow, mingw/msvc windows? */
			printf ("Open %s (out) error\n", outFilnam);
			CLEANUP_FILEIN;
			return RETVAL_MAIN;
		}

		sampleLen = p86->samples[id]->length;
		TRACE ((EXENAME, "Writing output file (%luB total)", sampleLen));
		do {
			sampleRead = PMD_ReadBuffer (p86->samples[id]->src, sampleLen, iobuf, (unsigned int)iobufLen);
			if (sampleRead == -1) {
				printf ("Read %s (in) sample error\n", argv[1]);
				ERROR_READ ("P86 partial sample (into buffer)",
					(sampleLen > (unsigned long)iobufLen) ? (unsigned long)iobufLen : sampleLen
				);
				CLEANUP_DEST;
				return RETVAL_LIB;
			};

			WRITE_CHECK (iobuf, sizeof (char), (unsigned int)sampleRead) {
				printf ("Write %s (out) sample error\n", outFilnam);
				ERROR_WRITE ("P86 partial sample (from buffer)", sampleRead);
				CLEANUP_DEST;
				return RETVAL_LIB;
			}
			sampleLen -= (unsigned long)sampleRead;
		} while (sampleLen != 0);

		fclose (dest);
		TRACE ((EXENAME, "Done with sample #%03u", id));
	}

	CLEANUP_FILEIN;
	TRACE ((EXENAME, "Done!"));
	return RETVAL_OK;
}