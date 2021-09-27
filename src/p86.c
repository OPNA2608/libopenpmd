#include "p86.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const char P86_MAGIC[13] = "PCM86 DATA\n\0";
const unsigned short P86_HEADERLENGTH = 0x0610;
const unsigned long P86_LENGTHMAX = 0xFFFFFF;

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
	char errormsg[PMD_ERRMAXSIZE];
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
				snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "imported sample data", sample_length);
				PMD_SetError (errormsg);
        return parsedData; /* TODO BAD! */
			}
			dontcare = fread (buffer, sample_length, sizeof (char), p86File);
			tempSample.data = (signed char*) buffer;
			fseek (p86File, curpos, SEEK_SET);
		}
		MALLOC_CHECK (parsedSample, sizeof (p86_sample)) {
			snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "imported p86_sample struct", sizeof (p86_sample));
			PMD_SetError (errormsg);
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
int P86_ExportFile (p86_struct* p86, FILE* p86File) {
	unsigned long start, length, startWrite, lengthWrite;
	unsigned int i;

	P86_Validate (p86);
	P86_Print (p86);

	WRITE_S (p86File, (char*) P86_MAGIC)
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

	return 0;

err:
	printf ("Failed to write to file.\n");
	return 1;
}
#undef WRITE_DAT
#undef WRITE_L
#undef WRITE_C
#undef WRITE_S
#undef TRY_WRITE_GOTO_ERR

p86_struct P86_New () {
	unsigned int i;
	char errormsg[PMD_ERRMAXSIZE];
	p86_sample* newSample;
	p86_sample tempSample = { 0 };
	p86_struct newData = { 0 };

	newData.version = '\x11';

	for (i = 0; i <= 255; ++i) {
		tempSample.id = i;
		tempSample.length = 0;

		MALLOC_CHECK (newSample, sizeof (p86_sample)) {
			snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "blank p86_sample instance", sizeof (p86_sample));
			PMD_SetError (errormsg);
			return newData; /* TODO see header for problem with this */
		}
		memcpy (newSample, &tempSample, sizeof (p86_sample));
		newData.samples[i] = newSample;
	}

	P86_Validate (&newData);
	P86_Print (&newData);

	return newData;
}

void P86_Free (p86_struct* p86) {
	unsigned int i;

	for (i = 0; i <= 255; ++i) {
		P86_FreeSample (p86->samples[i]);
	}
}

void P86_FreeSample (p86_sample* sample) {
	if (sample->length > 0) {
		free (sample->data);
	}
	free (sample);
}

#define CHECK_VALIDITY() if (valid) {\
	printf ("Data failed validation check! Continuing.\n"); \
	valid = false; \
}
int P86_Validate (p86_struct* p86) {
	const char* p86_error_length_sample = "Sample at ID #%03u (%luB) exceeds per-sample length limit (%luB)";
	const char* p86_error_length_total = "Total length (%luB) exceeds maximum length limit (%luB)";
	char errormsg[PMD_ERRMAXSIZE];
	unsigned int i;
	unsigned long totalSize;
	boolean valid = true;

	/*
	 * FIXME Errors in this function overwrite each other since switching to global error functions!
	 */

	printf ("Checking sample lengths.\n");
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > P86_LENGTHMAX) {
			snprintf (errormsg, PMD_ERRMAXSIZE, p86_error_length_sample, i, p86->samples[i]->length, P86_LENGTHMAX);
			PMD_SetError (errormsg);
			CHECK_VALIDITY();
		}
	}

	printf ("Checking total length.\n");
	totalSize = P86_GetTotalLength (p86);
	if (totalSize > P86_LENGTHMAX) {
		snprintf (errormsg, PMD_ERRMAXSIZE, p86_error_length_total, totalSize, P86_LENGTHMAX);
		PMD_SetError (errormsg);
		CHECK_VALIDITY();
	}

	if (valid) {
		printf ("Data is valid!\n");
		return 0;
	} else {
		printf ("See PMD_GetError() for reason for failed validity check.\n");
		return 1;
	}
}
#undef CHECK_VALIDITY

int P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data) {
	char errormsg[PMD_ERRMAXSIZE];
	signed char* buffer;
	p86_sample* newSample;
	p86_sample tempSample = { 0 };

	tempSample.id = id;
	tempSample.length = length;

	MALLOC_CHECK (buffer, length) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "sample data buffer", length);
		PMD_SetError (errormsg);
		return 1;
	}
  memcpy (buffer, data, length);
  tempSample.data = buffer;

  MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		PMD_SetError (errormsg);
		if (buffer != NULL) {
			free (buffer);
		}
		return 2;
	}
  memcpy (newSample, &tempSample, sizeof (p86_sample));
	if (P86_IsSet (p86, id) == 0) {
	  free (p86->samples[id]->data);
	}
	free (p86->samples[id]);
  p86->samples[id] = newSample;

	P86_Validate (p86);
	P86_Print (p86);

	return 0;
}

p86_sample* P86_GetSample (p86_struct* p86, unsigned char id) {
	const char* p86_error_id_missing = "Requested sample ID is not set";
	char errormsg[PMD_ERRMAXSIZE];
	signed char* buffer;
	p86_sample* copiedSample;
	p86_sample tempSample = { 0 };

	if (P86_IsSet (p86, id) > 0) {
		PMD_SetError (p86_error_id_missing);
		return NULL;
	}

	tempSample.id = id;
	tempSample.length = p86->samples[id]->length;

	MALLOC_CHECK (buffer, tempSample.length) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "sample data buffer", tempSample.length);
		PMD_SetError (errormsg);
		return NULL;
	}
	memcpy (buffer, p86->samples[id]->data, tempSample.length);
	tempSample.data = buffer;

	MALLOC_CHECK (copiedSample, sizeof (p86_sample)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		PMD_SetError (errormsg);
		free (buffer);
		return NULL;
	}
	memcpy (copiedSample, &tempSample, sizeof (p86_sample));

	return copiedSample;
}

int P86_AddSample (p86_struct* p86, unsigned long length, signed char* data) {
	const char* p86_error_bank_full = "All bank IDs are already in use";
	unsigned int i;

	for (i = 0; i <= 255; ++i) {
		if (P86_IsSet (p86, i) > 0) {
			printf ("Mapping sample to ID #%03u.\n", i);
			return P86_SetSample (p86, i, length, data);
		}
	}

	PMD_SetError (p86_error_bank_full);
	return 1;
}

int P86_UnsetSample (p86_struct* p86, unsigned char id) {
	if (P86_IsSet (p86, id) == 0) {
		free (p86->samples[id]->data);
		p86->samples[id]->length = 0;
	}

	P86_Validate (p86);
	P86_Print (p86);

	return 0;
}

int P86_RemoveSample (p86_struct* p86, unsigned char id) {
	char errormsg[PMD_ERRMAXSIZE];
	unsigned int i;
	p86_sample* newSample;
	p86_sample tempSample = { 0 };

	P86_UnsetSample (p86, id);

	for (i = id; i < 255; ++i) {
		p86->samples[i] = p86->samples[i+1];
		--p86->samples[i]->id;
	}

	tempSample.id = 255;
	tempSample.length = 0;
	tempSample.data = NULL;

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		PMD_SetError (errormsg);
		return 1;
	}
	memcpy (newSample, &tempSample, sizeof (p86_sample));
	p86->samples[255] = newSample;

	P86_Validate (p86);
	P86_Print (p86);

	return 0;
}

int P86_SwitchSamples (p86_struct* p86, unsigned char from, unsigned char to) {
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

	return 0;
}

int P86_IsSet (p86_struct* p86, unsigned char id) {
	return (p86->samples[id]->length > 0) ? 0 : 1;
}

void P86_Print (p86_struct* p86) {
	char versionString[6];
	unsigned int i;

	P86_GetVersionString (&p86->version, versionString);
	printf ("P86DRV version: %s\n", versionString);

	for (i = 0; i <= 255; ++i) {
		if (P86_IsSet (p86, i) == 0) {
			printf ("Sample #%03u set, Length: %lu.\n", i, p86->samples[i]->length);
		}
	}

	printf ("Total length: %lu.\n", P86_GetTotalLength (p86));
}

int P86_GetVersionString (unsigned char* version, char* buf) {
	return sprintf (buf, "%u.%u", *version >> 4, *version & 0x0F);
}

unsigned long P86_GetTotalLength (p86_struct* p86) {
	const char* p86_error_length_overflow = "Overflow in total length calculation, current sum (%luB) "
		"+ current sample #%03u (%luB) exceeds data type limit (%luB)";
	char errormsg[PMD_ERRMAXSIZE];
	int i;
	unsigned long sum = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > 0) {
			if (sum > (ULONG_MAX - p86->samples[i]->length)) {
				snprintf (errormsg, PMD_ERRMAXSIZE, p86_error_length_overflow, sum, i, p86->samples[i]->length, ULONG_MAX);
				PMD_SetError (errormsg);
				return 0;
			}
			sum += p86->samples[i]->length;
		}
	}
	return sum;
}
