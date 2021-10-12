#include "p86.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const char P86_MAGIC[12] = "PCM86 DATA\n\0";
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
 * TODO
 * * load file in chunks instead
 */
#define READ_CHECK(var, elemsize, writecounter) \
	locReadCounter = fread (var, elemsize, writecounter, p86File); \
	if (locReadCounter != writecounter)
p86_struct* P86_ImportFile (FILE* p86File) {
	char errormsg[PMD_ERRMAXSIZE];
	unsigned int i, j, locReadCounter;
	long curpos;
	p86_struct* parsedData;
	p86_sample* parsedSample;
	signed char* buffer;
	unsigned long sample_start = 0;
	unsigned long sample_length = 0;
	unsigned long parsedLength = 0;
	long startpos = ftell (p86File);

	MALLOC_CHECK (parsedData, sizeof (p86_struct)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "imported p86_struct", sizeof (p86_struct));
		PMD_SetError (errormsg);
		return NULL;
	}

	/* TODO use a separate buffer? */
	MALLOC_CHECK (buffer, 13) {
		free (parsedData);
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "P86 file magic", 13);
		PMD_SetError (errormsg);
		return NULL;
	}
	READ_CHECK (buffer, sizeof (char), 12) {
		free (buffer);
		free (parsedData);
		PMD_SetError (pmd_error_eof);
		return NULL;
	}
	if (memcmp (P86_MAGIC, (char*)buffer, 12) != 0) {
		free (buffer);
		free (parsedData);
		snprintf (errormsg, PMD_ERRMAXSIZE, "P86 file header mismatch");
		PMD_SetError (errormsg);
		return NULL;
	}
	free (buffer);

	parsedData->version = fgetc (p86File);

	READ_CHECK (&parsedLength, 3, 1) {
		free (parsedData);
		PMD_SetError (pmd_error_eof);
		return NULL;
	}

	for (i = 0; i <= 255; ++i) {
		MALLOC_CHECK (parsedSample, sizeof (p86_sample)) {
			for (j = --i; j > i; --j) {
				P86_FreeSample (parsedData->samples[j]);
			}
			free (parsedData);
			snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "imported p86_sample struct", sizeof (p86_sample));
			PMD_SetError (errormsg);
			return NULL;
		}
		parsedSample->id = i;

		READ_CHECK (&sample_start, 3, 1) {
			free (parsedSample);
			for (j = --i; j > i; --j) {
				P86_FreeSample (parsedData->samples[j]);
			}
			free (parsedData);
			PMD_SetError (pmd_error_eof);
			return NULL;
		}
		READ_CHECK (&sample_length, 3, 1) {
			free (parsedSample);
			for (j = --i; j > i; --j) {
				P86_FreeSample (parsedData->samples[j]);
			}
			free (parsedData);
			PMD_SetError (pmd_error_eof);
			return NULL;
		}
		parsedSample->length = sample_length;

		if (sample_length > 0) {
			MALLOC_CHECK (buffer, sample_length) {
				free (parsedSample);
				for (j = --i; j > i; --j) {
					P86_FreeSample (parsedData->samples[j]);
				}
				free (parsedData);
				snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "imported sample data", sample_length);
				PMD_SetError (errormsg);
				return NULL;
			}
			curpos = ftell (p86File);
			fseek (p86File, startpos + sample_start, SEEK_SET);
			READ_CHECK (buffer, sizeof (char), sample_length) {
				free (parsedSample);
				for (j = --i; j > i; --j) {
					P86_FreeSample (parsedData->samples[j]);
				}
				free (parsedData);
				PMD_SetError (pmd_error_eof);
				return NULL;
			}
			parsedSample->data = buffer;
			fseek (p86File, curpos, SEEK_SET);
		}
		parsedData->samples[i] = parsedSample;
	}

	if (P86_Validate (parsedData) != 0) {
		P86_Free (parsedData);
		/* propagating validation error */
		return NULL;
	}
	/* extra validation */
	if (parsedLength != P86_GetTotalLength (parsedData)) {
		P86_Free (parsedData);
		snprintf (errormsg, PMD_ERRMAXSIZE, "Parsed total length does not match specified total length");
		PMD_SetError (errormsg);
		return NULL;
	}
	P86_Print (parsedData);

	return parsedData;
}
#undef READ_CHECK

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

p86_struct* P86_New () {
	unsigned int i, j;
	char errormsg[PMD_ERRMAXSIZE];
	p86_sample* newSample;
	p86_struct* newData;

	MALLOC_CHECK (newData, sizeof (p86_struct)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "blank p86_struct instance", sizeof (p86_struct));
		PMD_SetError (errormsg);
		return NULL;
	}
	newData->version = '\x11';

	for (i = 0; i <= 255; ++i) {
		MALLOC_CHECK (newSample, sizeof (p86_sample)) {
			for (j = --i; j > i; --j) {
				P86_FreeSample (newData->samples[j]);
			}
			free (newData);
			snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "blank p86_sample instance", sizeof (p86_sample));
			PMD_SetError (errormsg);
			return NULL;
		}
		newSample->id = i;
		newSample->length = 0;
		newData->samples[i] = newSample;
	}

	P86_Print (newData);

	return newData;
}

void P86_Free (p86_struct* p86) {
	unsigned int i;

	for (i = 0; i <= 255; ++i) {
		P86_FreeSample (p86->samples[i]);
	}
	free (p86);
}

void P86_FreeSample (p86_sample* sample) {
	if (sample->length > 0) {
		free (sample->data);
	}
	free (sample);
}

#define INVALID() \
	printf ("%s\n", p86_invalid); \
	return 1;
int P86_Validate (p86_struct* p86) {
	const char* p86_error_length_sample = "Sample at ID #%03u (%luB) exceeds per-sample length limit (%luB)";
	const char* p86_error_length_total = "Total length (%luB) exceeds maximum length limit (%luB)";
	const char* p86_invalid = "Data failed validation check! Check PMD_GetError() for more details.";
	char errormsg[PMD_ERRMAXSIZE];
	unsigned int i;
	unsigned long totalSize;

	printf ("Checking sample lengths.\n");
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > P86_LENGTHMAX) {
			snprintf (errormsg, PMD_ERRMAXSIZE, p86_error_length_sample, i, p86->samples[i]->length, P86_LENGTHMAX);
			PMD_SetError (errormsg);
			INVALID();
		}
	}

	printf ("Checking total length.\n");
	totalSize = P86_GetTotalLength (p86);
	if (totalSize > P86_LENGTHMAX) {
		snprintf (errormsg, PMD_ERRMAXSIZE, p86_error_length_total, totalSize, P86_LENGTHMAX);
		PMD_SetError (errormsg);
		INVALID();
	}

	printf ("Data is valid!\n");
	return 0;
}
#undef INVALID

int P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data) {
	char errormsg[PMD_ERRMAXSIZE];
	signed char* buffer;
	p86_sample* newSample;

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		PMD_SetError (errormsg);
		return 2;
	}
	newSample->id = id;
	newSample->length = length;

	MALLOC_CHECK (buffer, length) {
		free (newSample);
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "sample data buffer", length);
		PMD_SetError (errormsg);
		return 1;
	}
  memcpy (buffer, data, length);
  newSample->data = buffer;

	P86_FreeSample (p86->samples[id]);
  p86->samples[id] = newSample;

	P86_Print (p86);

	return 0;
}

const p86_sample* P86_GetSample (p86_struct* p86, unsigned char id) {
	const char* p86_error_id_missing = "Requested sample ID is not set";

	if (P86_IsSet (p86, id) > 0) {
		PMD_SetError (p86_error_id_missing);
		return NULL;
	}
	return p86->samples[id];
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

	P86_Print (p86);

	return 0;
}

int P86_RemoveSample (p86_struct* p86, unsigned char id) {
	char errormsg[PMD_ERRMAXSIZE];
	unsigned int i;
	p86_sample* newSample;

	P86_UnsetSample (p86, id);

	for (i = id; i < 255; ++i) {
		p86->samples[i] = p86->samples[i+1];
		--p86->samples[i]->id;
	}

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		snprintf (errormsg, PMD_ERRMAXSIZE, pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		PMD_SetError (errormsg);
		return 1;
	}
	newSample->id = 255;
	newSample->length = 0;
	newSample->data = NULL;

	p86->samples[255] = newSample;

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
