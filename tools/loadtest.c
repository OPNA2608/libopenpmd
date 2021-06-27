#include <stdlib.h>
#include "p86.h"

#define READ_TEST "../examples/P86/RC1.P86"
#define SAMPLE_TEST_ID_ADD 0
#define SAMPLE_TEST_ID_SET 2
#define SAMPLE_TEST_LENGTH 16540
#define WRITE_TEST "test.P86"

int main (int argc, char* argv[]) {
	char* p86File;
	FILE* test;
	p86_struct parsedData;
	signed char* newSampleData;
	unsigned int i;

	UNUSED (parsedData);

	if (argc < 2) {
		p86File = READ_TEST;
	} else {
		p86File = argv[1];
	}

	test = fopen (p86File, "rb");
	if (test == NULL) {
		printf ("Could not open file %s.\n", p86File);
		return 1;
	}

	printf ("Testing importing of %s.\n", p86File);
	parsedData = P86_ImportFile (test);
	fclose (test);

	newSampleData = malloc (SAMPLE_TEST_LENGTH);
	for (i = 0; i < SAMPLE_TEST_LENGTH; ++i) {
		newSampleData[i] = (i % 2 == 0) ? 127 : -128;
	}

	printf ("Testing sample adding (ID #%03u with default bank).\n", SAMPLE_TEST_ID_ADD);
	if (!P86_AddSample (&parsedData, SAMPLE_TEST_LENGTH, newSampleData)) {
		printf ("Adding sample data failed.\n");
		return 1;
	}

	printf ("Testing sample setting (ID #%03u with default bank).\n", SAMPLE_TEST_ID_SET);
	if (!P86_SetSample (&parsedData, SAMPLE_TEST_ID_SET, SAMPLE_TEST_LENGTH, newSampleData)) {
		printf ("Setting sample data failed.\n");
		return 1;
	}

	free (newSampleData);

	printf ("Testing exporting to %s.\n", WRITE_TEST);
	test = fopen (WRITE_TEST, "wb");
	P86_ExportFile (&parsedData, test);
	fclose (test);

	P86_Free (&parsedData);

	return 0;
}
