#include "p86.h"
#include <stdlib.h>

#define READ_TEST "VA_PCM__.P86"
#define WRITE_TEST "test.P86"

#define UNUSED(x) (void)(x)

int main() {
	FILE* test;
	p86_struct parsedData;

  test = fopen (READ_TEST, "rb");
	parsedData = P86_ImportFile (test);
	UNUSED (parsedData);
	fclose (test);

	test = fopen (WRITE_TEST, "wb");
	P86_ExportFile (&parsedData, test);
	fclose (test);

	return 0;
}
