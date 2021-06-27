#include "p86.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char P86_MAGIC[13] = "PCM86 DATA\n\0";
unsigned short P86_HEADERLENGTH = 0x0610;
unsigned long P86_LENGTHMAX = 0xFFFFFF;

/*
 * Start Helpers
 */

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

/*
 * End Helpers
 */

/*
 * TODO load file in chunks instead
 */
p86_struct P86_ImportFile (FILE* p86File) {
	unsigned int i, dontcare;
	long curpos;
	p86_sample* parsedSample;
	signed char* buffer;
	unsigned long sample_start = 0;
	unsigned long sample_length = 0;
	long startpos = ftell (p86File);
	p86_sample tempSample = { 0 };
	p86_struct parsedData = { 0 };

	fseek (p86File, 0xC, SEEK_CUR);
	parsedData.version = fgetc (p86File);

	/* We ignore the bank's total length field. */
	fseek (p86File, 3, SEEK_CUR);

	for (i = 0; i <= 255; ++i) {
		tempSample.id = i;

		dontcare = fread (&sample_start, 3, 1, p86File);
		dontcare = fread (&sample_length, 3, 1, p86File);
		sample_length = sample_length & P86_LENGTHMAX;
		tempSample.length = sample_length;

		if (sample_length > 0) {
			curpos = ftell (p86File);
			fseek (p86File, startpos + sample_start, SEEK_SET);
			MALLOC_CHECK (buffer, sample_length) {
				MALLOC_ERROR ("imported sample data", sample_length);
        return parsedData; /* TODO BAD! */
			}
			dontcare = fread (buffer, sample_length, sizeof (char), p86File);
			tempSample.data = (signed char*) buffer;
			fseek (p86File, curpos, SEEK_SET);
		}
		MALLOC_CHECK (parsedSample, sizeof (p86_sample)) {
			MALLOC_ERROR ("imported p86_sample instance", sizeof (p86_sample));
			return parsedData; /* TODO BAD! */
		}
		memcpy (parsedSample, &tempSample, sizeof (p86_sample));
		parsedData.samples[i] = parsedSample;
	}

	printf ("Blub: %u\n", dontcare);

	P86_Validate (&parsedData);
	P86_Print (&parsedData);

	return parsedData;
}

#define TRY_WRITE_GOTO_ERR(func, file, text) if (!func (file, text)) goto err;
#define WRITE_S(file, text) TRY_WRITE_GOTO_ERR(try_file_write_s, file, text)
#define WRITE_C(file, text) TRY_WRITE_GOTO_ERR(try_file_write_c, file, text)
#define WRITE_L(file, length) TRY_WRITE_GOTO_ERR(try_file_write_l, file, length)
#define WRITE_DAT(file, data, length) if (!try_file_write_dat (file, data, length)) goto err;
boolean P86_ExportFile (p86_struct* p86, FILE* p86File) {
	unsigned long start, length, startWrite, lengthWrite;
	unsigned int i;

	P86_Validate (p86);
	P86_Print (p86);

	WRITE_S (p86File, P86_MAGIC)
	WRITE_C (p86File, "\0")
	WRITE_C (p86File, "\x11")

	length = P86_GetTotalLength (p86);
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

p86_struct P86_New () {
	unsigned int i;
	p86_sample* newSample;
	p86_sample tempSample = { 0 };
	p86_struct newData = { 0 };

	newData.version = '\x11';

	for (i = 0; i <= 255; ++i) {
		tempSample.id = i;
		tempSample.length = 0;

		MALLOC_CHECK (newSample, sizeof (p86_sample)) {
			MALLOC_ERROR ("blank p86_sample instance", sizeof (p86_sample));
			return newData; /* deal with it */
		}
		memcpy (newSample, &tempSample, sizeof (p86_sample));
		newData.samples[i] = newSample;
	}

	P86_Validate (&newData);
	P86_Print (&newData);

	return newData;
}

boolean P86_Free (p86_struct* p86) {
	unsigned int i;

	for (i = 0; i <= 255; ++i) {
		if (P86_IsSet (p86, i)) {
			free (p86->samples[i]->data);
		}
		free (p86->samples[i]);
	}

	return true;
}

#define CHECK_VALIDITY() if (valid) {\
	printf ("Data failed validation check! Continuing.\n"); \
	valid = false; \
}
boolean P86_Validate (p86_struct* p86) {
	unsigned int i;
	unsigned long totalSize;
	boolean valid = true;

	printf ("Checking sample lengths.\n");
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > P86_LENGTHMAX) {
			printf ("Sample at ID #%03u (%luB) exceeds per-sample length limit (%luB)!\n",
					i, p86->samples[i]->length, P86_LENGTHMAX);
			CHECK_VALIDITY();
		}
	}

	printf ("Checking total length.\n");
	totalSize = P86_GetTotalLength (p86);
	if (totalSize > P86_LENGTHMAX) {
		printf ("Total length (%luB) exceeds maximum length limit (%luB)!\n", totalSize, P86_LENGTHMAX);
		CHECK_VALIDITY();
	}

	if (valid) {
		printf ("Data is valid!\n");
	} else {
		printf ("See above output for reason for failed validity check.\n");
	}
	return valid;
}
#undef CHECK_VALIDITY

boolean P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data) {
  signed char* buffer;
  p86_sample* newSample;
	p86_sample tempSample = { 0 };

  tempSample.id = id;
  tempSample.length = length;
  MALLOC_CHECK (buffer, length) {
		MALLOC_ERROR ("sample data buffer", length);
		return false;
	}
  memcpy (buffer, data, length);
  tempSample.data = buffer;

  MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		MALLOC_ERROR ("p86_sample instance", sizeof (p86_sample));
		if (buffer != NULL) {
			free (buffer);
		}
		return false;
	}
  memcpy (newSample, &tempSample, sizeof (p86_sample));
	if (P86_IsSet (p86, id)) {
	  free (p86->samples[id]->data);
	}
	free (p86->samples[id]);
  p86->samples[id] = newSample;

	P86_Validate (p86);
	P86_Print (p86);

	return true;
}

boolean P86_AddSample (p86_struct* p86, unsigned long length, signed char* data) {
	unsigned int i;

	for (i = 0; i <= 255; ++i) {
		if (!P86_IsSet (p86, i)) {
			printf ("Mapping sample to ID #%03u.\n", i);
			return P86_SetSample (p86, i, length, data);
		}
	}

	printf ("ERROR: Can't add sample to bank.\n");
	printf ("CAUSE: Bank is full (no ID with length 0).\n");
	return false;
}

boolean P86_UnsetSample (p86_struct* p86, unsigned char id) {
	if (P86_IsSet (p86, id)) {
		free (p86->samples[id]->data);
		p86->samples[id]->length = 0;
	}

	P86_Validate (p86);
	P86_Print (p86);

	return true;
}

boolean P86_RemoveSample (p86_struct* p86, unsigned char id) {
	unsigned int i;
	p86_sample* newSample;
	p86_sample tempSample = { 0 };

	if (!P86_UnsetSample (p86, id)) {
		printf ("Failed to unmap sample at ID #%03u.\n", id);
		return false;
	}

	for (i = id; i < 255; ++i) {
		p86->samples[i] = p86->samples[i+1];
		--p86->samples[i]->id;
	}

	tempSample.id = 255;
	tempSample.length = 0;
	tempSample.data = NULL;

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		MALLOC_ERROR ("new sample", sizeof (p86_sample));
		return false;
	}
	memcpy (newSample, &tempSample, sizeof (p86_sample));
	p86->samples[255] = newSample;

	P86_Validate (p86);
	P86_Print (p86);

	return true;
}

boolean P86_SwitchSamples (p86_struct* p86, unsigned char from, unsigned char to) {
	unsigned long tempLength;
	signed char* tempData;

	tempLength = p86->samples[to]->length;
	tempData = p86->samples[to]->data;

	p86->samples[to]->length = p86->samples[from]->length;
	p86->samples[to]->data = p86->samples[from]->data;

	p86->samples[from]->length = tempLength;
	p86->samples[from]->data = tempData;

	P86_Validate (p86);
	P86_Print (p86);

	return true;
}

boolean P86_IsSet (p86_struct* p86, unsigned char id) {
	return p86->samples[id]->length > 0;
}

void P86_Print (p86_struct* p86) {
	char versionString[6];
	unsigned int i;

	P86_GetVersionString (&p86->version, versionString);
	printf ("P86DRV version: %s\n", versionString);

	for (i = 0; i <= 255; ++i) {
		printf ("Sample #%03u ", i);
		if (P86_IsSet (p86, i)) {
			printf ("set, Length: %lu.\n", p86->samples[i]->length);
		} else {
			printf ("unset.\n");
		}
	}

	printf ("Total length: %lu.\n", P86_GetTotalLength (p86));
}

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
