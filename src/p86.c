#include "p86.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define UNUSED(x) (void)(x)

char P86_MAGIC[12] = "PCM86 DATA\n\0";
unsigned short P86_HEADERLENGTH = 0x0610;
unsigned long P86_LENGTHMAX = 0xFFFFFF;

boolean try_file_write_c (FILE* file, char* text) {
	fputc (*text, file);
	if (ferror (file)) {
		printf ("Error occurred while writing to file.\n");
    return false;
	}
	return true;
}

boolean try_file_write_s (FILE* file, char* text) {
  fputs (text, file);
	if (ferror (file)) {
		printf ("Error occurred while writing to file.\n");
		return false;
	}
	return true;
}

boolean try_file_write_l (FILE* file, unsigned long* length) {
	fwrite (length, 3, 1, file);
	if (ferror (file)) {
		printf ("Error occurred while writing to file.\n");
		return false;
	}
	return true;
}

boolean try_file_write_dat (FILE* file, signed char* data, unsigned long* length) {
	fwrite (data, *length, sizeof (char), file);
	if (ferror (file)) {
		printf ("Error occurred while writing to file.\n");
		return false;
	}
	return true;
}

p86_struct P86_ImportFile (FILE* p86File) {
	unsigned long sample_start, sample_length;
	int i, dontcare;
	void* buffer;
	long curpos;
	long startpos = ftell (p86File);
	p86_sample* parsedSample;
	p86_sample tempSample = { 0 };
	p86_struct parsedData = { 0 };

	UNUSED (dontcare);

	fseek (p86File, 0xC, SEEK_CUR);
	parsedData.version = fgetc (p86File);
	buffer = calloc (6, sizeof (char));
	P86_GetVersionString (&parsedData.version, (char*) buffer);
	printf ("P86 version: %s\n", (char*) buffer);

	fseek (p86File, 3, SEEK_CUR);
  for (i = 0; i <= 255; ++i) {
		tempSample.id = i;

	  dontcare = fread (&sample_start, 3, 1, p86File);
	  dontcare = fread (&sample_length, 3, 1, p86File);
		sample_length = sample_length & 0xFFFFFF;
		tempSample.length = sample_length;

		if (sample_length > 0) {
	    curpos = ftell (p86File);
			fseek (p86File, startpos + sample_start, SEEK_SET);
			buffer = malloc (sample_length);
			dontcare = fread (buffer, sample_length, sizeof (char), p86File);
			tempSample.data = (signed char*) buffer;
			fseek (p86File, curpos, SEEK_SET);

			printf ("Sample #%03u, ", i);
			printf ("Start: 0x%06lX, ", sample_start & 0xFFFFFF);
			printf ("Length: 0x%06lX\n", sample_length);
		} else {
			printf ("Sample #%03u unset.\n", i);
		}
		parsedSample = (p86_sample*) malloc (sizeof (p86_sample));
		memcpy (parsedSample, &tempSample, sizeof (p86_sample));
		parsedData.samples[i] = parsedSample;
	}

	return parsedData;
}

#define TRY_WRITE_GOTO_ERR(func, file, text) if (!func (file, text)) goto err;
#define WRITE_S(file, text) TRY_WRITE_GOTO_ERR(try_file_write_s, file, text)
#define WRITE_C(file, text) TRY_WRITE_GOTO_ERR(try_file_write_c, file, text)
#define WRITE_L(file, length) TRY_WRITE_GOTO_ERR(try_file_write_l, file, length)
#define WRITE_DAT(file, data, length) if (!try_file_write_dat (file, data, length)) goto err;

boolean P86_ExportFile (p86_struct* p86, FILE* p86File) {
	unsigned long start, length, startWrite, lengthWrite;
	int i;

	UNUSED (p86);

	WRITE_S (p86File, P86_MAGIC)
	WRITE_C (p86File, "\0")
	WRITE_C (p86File, "\x11")

	length = P86_GetTotalLength (p86);
	printf ("Total length: %lu\n", length);
	WRITE_L (p86File, &length);

	start = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		length = p86->samples[i]->length;
		if (length > 0) {
			startWrite = start;
			lengthWrite = length;
			start += length;
		} else {
			startWrite = 0;
			lengthWrite = 0;
		}
		WRITE_L (p86File, &startWrite);
		WRITE_L (p86File, &lengthWrite);
	}
	for (i = 0; i <= 255; ++i) {
		length = p86->samples[i]->length;
		if (length > 0) {
			WRITE_DAT (p86File, p86->samples[i]->data, &length);
		}
	}

	return true;

err:
	printf ("Failed to write to file.\n");
	return false;
}

#undef WRITE_DAT
#undef WRITE_L
#undef WRITE_C
#undef WRITE_S
#undef TRY_WRITE_GOTO_ERR

int P86_GetVersionString (unsigned char* version, char* buf) {

  return sprintf (buf, "%u.%u", *version >> 4, *version & 0x0F);
}

unsigned long P86_GetTotalLength (p86_struct* p86) {
	int i;
  unsigned long sum = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
	  if (p86->samples[i]->length > 0) {
			if (sum > (ULONG_MAX - p86->samples[i]->length)) {
				printf ("ERROR: Overflow in total length calculation detected.\n");
				printf ("CAUSE: current sum (%luB) + current sample #%03u (%luB) exceeds data type limit (%luB).\n",
					sum, i, p86->samples[i]->length, ULONG_MAX);
				return 0;
			}
			sum += p86->samples[i]->length;
		}
	}
	return sum;
}
