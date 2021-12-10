#include "p86.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const char P86_MAGIC[12] = "PCM86 DATA\n\0";
const unsigned short P86_HEADERLENGTH = 0x0610;
const unsigned long P86_LENGTHMAX = 0xFFFFFF;

/*
 * TODO
 * * load file in chunks instead?
 */
#define READ_CHECK(var, elemsize, writecounter) \
	locReadCounter = fread (var, elemsize, writecounter, p86File); \
	if (locReadCounter != writecounter)
p86_struct* P86_ImportFile (FILE* p86File) {
	size_t locReadCounter;
	unsigned int i, j;
	long curpos;
	p86_struct* parsedData;
	p86_sample* parsedSample;
	signed char* buffer;
	unsigned long sample_start = 0;
	unsigned long sample_length = 0;
	unsigned long parsedLength = 0;
	long startpos = ftell (p86File);

	MALLOC_CHECK (parsedData, sizeof (p86_struct)) {
		PMD_SetError (pmd_error_malloc, "imported p86_struct", sizeof (p86_struct));
		return NULL;
	}

	/* TODO use a separate buffer? */
	MALLOC_CHECK (buffer, 13) {
		free (parsedData);
		PMD_SetError (pmd_error_malloc, "P86 file magic", 13);
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
		PMD_SetError ("P86 file header mismatch");
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
			PMD_SetError (pmd_error_malloc, "imported p86_sample struct", sizeof (p86_sample));
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
				PMD_SetError (pmd_error_malloc, "imported sample data", sample_length);
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
			parsedSample->type = MEM;
			parsedSample->typeData.mem.data = buffer;
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
		PMD_SetError ("Parsed total length does not match specified total length");
		return NULL;
	}
	P86_Print (parsedData);

	return parsedData;
}

p86_struct* P86_ImportFileSlim (FILE* p86File) {
	size_t locReadCounter;
	unsigned int i, j;
	char* buffer;
	p86_struct* parsedData;
	p86_sample* parsedSample;
	unsigned long sample_start = 0;
	unsigned long sample_length = 0;
	unsigned long parsedLength = 0;
	long startpos = ftell (p86File);

	MALLOC_CHECK (parsedData, sizeof (p86_struct)) {
		PMD_SetError (pmd_error_malloc, "imported p86_struct", sizeof (p86_struct));
		return NULL;
	}

	/* TODO use a separate buffer? */
	MALLOC_CHECK (buffer, 13) {
		free (parsedData);
		PMD_SetError (pmd_error_malloc, "P86 file magic", 13);
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
		PMD_SetError ("P86 file header mismatch");
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
			PMD_SetError (pmd_error_malloc, "imported p86_sample struct", sizeof (p86_sample));
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

		parsedSample->type = FIL;
		parsedSample->typeData.fil.file = p86File;
		parsedSample->typeData.fil.fileStart = startpos;
		parsedSample->typeData.fil.offset = sample_start;

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
		PMD_SetError ("Parsed total length does not match specified total length");
		return NULL;
	}
	P86_Print (parsedData);

	return parsedData;
}
#undef READ_CHECK

#define WRITE_CHECK(file, data, elemsize, writecounter) \
	fwrite (data, elemsize, writecounter, file); \
	if (ferror (file)) { \
		PMD_SetError (pmd_error_write); \
		return 1; \
	}
int P86_ExportFile (const p86_struct* p86, FILE* p86File) {
	unsigned long start, length, startWrite, lengthWrite;
	size_t filReadCounter, locReadCounter, readAmount;
	unsigned int i;
	signed char* buffer = NULL; /* Conditionally initialised */

	P86_Validate (p86);
	P86_Print (p86);

	WRITE_CHECK (p86File, P86_MAGIC, sizeof (char), 12);
	WRITE_CHECK (p86File, "\x11", sizeof (char), 1);

	length = P86_GetTotalLength (p86);
	WRITE_CHECK (p86File, &length, sizeof (char), 3);

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
		WRITE_CHECK (p86File, &startWrite, sizeof (char), 3);
		WRITE_CHECK (p86File, &lengthWrite, sizeof (char), 3);
	}
	for (i = 0; i <= 255; ++i) {
		length = p86->samples[i]->length;
		if (length > 0) {
			switch (p86->samples[i]->type) {
				case MEM:
					WRITE_CHECK (p86File, p86->samples[i]->typeData.mem.data, sizeof (char), length);
					break;
				case FIL:
					if (buffer == NULL) {
						MALLOC_CHECK (buffer, PMD_BUFFERSIZE) {
							PMD_SetError (pmd_error_malloc, "file read buffer");
							return 3;
						}
					}
					fseek (p86->samples[i]->typeData.fil.file,
						p86->samples[i]->typeData.fil.fileStart + p86->samples[i]->typeData.fil.offset, SEEK_SET);
					filReadCounter = length;
					while (filReadCounter != 0) {
						if (filReadCounter < PMD_BUFFERSIZE) {
							readAmount = filReadCounter;
						} else {
							readAmount = PMD_BUFFERSIZE;
						}
						locReadCounter = fread (buffer, sizeof (char), readAmount, p86->samples[i]->typeData.fil.file);
						if (locReadCounter != readAmount) {
							PMD_SetError (pmd_error_eof);
							return 2;
						}
						WRITE_CHECK (p86File, buffer, sizeof (char), locReadCounter);
						filReadCounter -= locReadCounter;
					}
					break;
			}
		}
	}

	if (buffer != NULL) {
		free (buffer);
	}

	return 0;
}
#undef WRITE_CHECK

p86_struct* P86_New () {
	unsigned int i, j;
	p86_sample* newSample;
	p86_struct* newData;

	MALLOC_CHECK (newData, sizeof (p86_struct)) {
		PMD_SetError (pmd_error_malloc, "blank p86_struct instance", sizeof (p86_struct));
		return NULL;
	}
	newData->version = '\x11';

	for (i = 0; i <= 255; ++i) {
		MALLOC_CHECK (newSample, sizeof (p86_sample)) {
			for (j = --i; j > i; --j) {
				P86_FreeSample (newData->samples[j]);
			}
			free (newData);
			PMD_SetError (pmd_error_malloc, "blank p86_sample instance", sizeof (p86_sample));
			return NULL;
		}
		newSample->id = i;
		newSample->type = MEM;
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
		if (sample->type == MEM) {
			free (sample->typeData.mem.data);
		} else {
			/* TODO handle FILE*s here */
		}
	}
	free (sample);
}

#define INVALID() \
	printf ("%s\n", p86_invalid); \
	return 1;
int P86_Validate (const p86_struct* p86) {
	const char* p86_error_length_sample = "Sample at ID #%03u (%luB) exceeds per-sample length limit (%luB)";
	const char* p86_error_length_total = "Total length (%luB) exceeds maximum length limit (%luB)";
	const char* p86_invalid = "Data failed validation check! Check PMD_GetError() for more details.";
	unsigned int i;
	unsigned long totalSize;

	printf ("Checking sample lengths.\n");
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > P86_LENGTHMAX) {
			PMD_SetError (p86_error_length_sample, i, p86->samples[i]->length, P86_LENGTHMAX);
			INVALID();
		}
	}

	printf ("Checking total length.\n");
	totalSize = P86_GetTotalLength (p86);
	if (totalSize > P86_LENGTHMAX) {
		PMD_SetError (p86_error_length_total, totalSize, P86_LENGTHMAX);
		INVALID();
	}

	printf ("Data is valid!\n");
	return 0;
}
#undef INVALID

int P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data) {
	signed char* buffer;
	p86_sample* newSample;

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		PMD_SetError (pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		return 2;
	}
	MALLOC_CHECK (buffer, length) {
		free (newSample);
		PMD_SetError (pmd_error_malloc, "sample data buffer", length);
		return 1;
	}

	newSample->id = id;
	newSample->length = length;

	newSample->type = MEM;
	memcpy (buffer, data, length);
	newSample->typeData.mem.data = buffer;

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
		if (p86->samples[id]->type == MEM) {
			free (p86->samples[id]->typeData.mem.data);
		} else {
			/* TODO handle FILE*s here */
		}
		p86->samples[id]->length = 0;
	}

	P86_Print (p86);

	return 0;
}

int P86_RemoveSample (p86_struct* p86, unsigned char id) {
	unsigned int i;
	p86_sample* newSample;

	P86_UnsetSample (p86, id);

	for (i = id; i < 255; ++i) {
		p86->samples[i] = p86->samples[i+1];
		--p86->samples[i]->id;
	}

	MALLOC_CHECK (newSample, sizeof (p86_sample)) {
		PMD_SetError (pmd_error_malloc, "p86_sample struct", sizeof (p86_sample));
		return 1;
	}
	newSample->id = 255;
	newSample->length = 0;
	newSample->type = MEM;
	newSample->typeData.mem.data = NULL;

	p86->samples[255] = newSample;

	P86_Print (p86);

	return 0;
}

int P86_SwitchSamples (p86_struct* p86, unsigned char from, unsigned char to) {
	unsigned long tempLength;
	p86_sample_type tempType;
	p86_sample_data tempData;

	tempLength = p86->samples[to]->length;
	tempType = p86->samples[to]->type;
	tempData = p86->samples[to]->typeData;

	p86->samples[to]->length = p86->samples[from]->length;
	p86->samples[to]->type = p86->samples[from]->type;
	p86->samples[to]->typeData = p86->samples[from]->typeData;

	p86->samples[from]->length = tempLength;
	p86->samples[from]->type = tempType;
	p86->samples[from]->typeData = tempData;

	P86_Print (p86);

	return 0;
}

int P86_IsSet (const p86_struct* p86, unsigned char id) {
	return (p86->samples[id]->length > 0) ? 0 : 1;
}

void P86_Print (const p86_struct* p86) {
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

int P86_GetVersionString (const unsigned char* version, char* buf) {
	return sprintf (buf, "%u.%u", *version >> 4, *version & 0x0F);
}

unsigned long P86_GetTotalLength (const p86_struct* p86) {
	const char* p86_error_length_overflow = "Overflow in total length calculation, current sum (%luB) "
		"+ current sample #%03u (%luB) exceeds data type limit (%luB)";
	int i;
	unsigned long sum = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i]->length > 0) {
			if (sum > (ULONG_MAX - p86->samples[i]->length)) {
				PMD_SetError (p86_error_length_overflow, sum, i, p86->samples[i]->length, ULONG_MAX);
				return 0;
			}
			sum += p86->samples[i]->length;
		}
	}
	return sum;
}
