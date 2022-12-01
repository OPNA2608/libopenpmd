#include "p86.h"

#include "common.h"
#include "io.h"

/* calloc & free */
#include <stdlib.h>
/* memcmp */
#include <string.h>
/*
#include <stdarg.h>
*/

const char P86_MAGIC[12] = "PCM86 DATA\n\0";
const unsigned short P86_HEADERLENGTH = 0x610;
const unsigned long P86_LENGTHMAX = 0xFFFFFF;

/*
 * Initialises a new P86 struct
 *
 * not NULL - OK
 * NULL - struct alloc failure
 */
p86_struct* P86_New (void) {
	p86_struct* newData;

	ALLOC_CHECK (newData, sizeof (p86_struct)) {
		ERROR_ALLOC ("P86 struct", sizeof (p86_struct));
		return NULL;
	}
	newData->version = '\x11';

	return newData;
}

/*
 * Free's a P86 struct
 *
 * Never fails
 */
void P86_Free (p86_struct* p86) {
	unsigned int i;

	if (p86 == NULL) return;

	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			P86_UnsetSample (p86, (unsigned char)i);
		}
	}
	free (p86);
	p86 = NULL;
}

#define CLEANUP_BUFFER free (buffer);
/*
 * Reads a P86 file into a struct
 *
 * 0 - OK
 * 1 - magic error
 * 2 - version error
 * 3 - size error
 * 4 - sample error
 */
int P86_Read (p86_struct* p86, FILE* src) {
	void* buffer;
	unsigned int i;
	unsigned long sample_start = 0;
	unsigned long sample_length = 0;

	ALLOC_CHECK (buffer, sizeof (char) * 12) {
		ERROR_ALLOC ("P86 magic check buffer", sizeof (char) * 12);
		return 1;
	};
	READ_CHECK (buffer, sizeof (char), 12) {
		CLEANUP_BUFFER;
		ERROR_READ ("P86 magic", sizeof (char) * 12);
		return 1;
	};
	if (memcmp (P86_MAGIC, (char*)buffer, 12) != 0) {
		CLEANUP_BUFFER;
		PMD_SetError ("P86 magic mismatch (%s vs %s) error", P86_MAGIC, (char*)buffer);
		PMD_PrintError();
		RAISE_TRAP;
		CLEANUP_BUFFER;
		return 1;
	};
	CLEANUP_BUFFER;

	READ_CHECK (&p86->version, sizeof (char), 1) {
		ERROR_READ ("P86 version", sizeof (char) * 1);
		return 2;
	};
	SEEK_CHECK (SEEK_CUR, sizeof (char) * 3) {
		PMD_SetError ("P86 size skip error");
		return 3;
	};

	for (i = 0; i <= 255; ++i) {
		READ_CHECK (&sample_start, sizeof (char) * 3, 1) {
			ERROR_READ ("P86 sample start", sizeof (char) * 3);
			return 4;
		};
		READ_CHECK (&sample_length, sizeof (char) * 3, 1) {
			ERROR_READ ("P86 sample length", sizeof (char) * 3);
			return 4;
		};
		if (sample_length != 0) {
			P86_SetSample (p86, (unsigned char)i, src, sample_start, sample_length);
		}
	}

	return 0;
}

#define WRAP_WRITE(src, name, elemsize, elemcount)\
	WRITE_CHECK (src, elemsize, elemcount) {\
		CLEANUP_BUFFER;\
		ERROR_WRITE (name, elemsize * elemcount);\
		return 3;\
	}

/*
 * Writes a P86 struct to a file
 *
 * 0 - OK
 * 1 - I/O buffer alloc error
 * 2 - total size > format max
 * 3 - dest write error
 * 4 - source seek/read error
 */
int P86_Write (p86_struct* p86, FILE* dest) {
	unsigned long length, sample_start, sample_unset;
  long length_read, buf_size;
	unsigned int i;
	FILE* src;
	char* buffer;

	sample_unset = 0;

	buf_size = PMD_GetBuffer ((void**)&buffer);
	if (buf_size == -1) {
		ERROR_ALLOC ("P86 I/O buffer", PMD_BUFFERSIZE);
		return 1;
	};

	length = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			length += p86->samples[i]->length;
		}
	}
	if (length > P86_LENGTHMAX) {
		CLEANUP_BUFFER;
		PMD_SetError ("P86 total size (%udB) > format max (%udB)", length, P86_LENGTHMAX);
		PMD_PrintError();
		RAISE_TRAP;
		return 2;
	};

	WRAP_WRITE (P86_MAGIC, "P86 magic", sizeof (char), 12);
	WRAP_WRITE (&p86->version, "P86 version", sizeof (char), 1);
	WRAP_WRITE (&length, "P86 total length", sizeof (char), 3);

	sample_start = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			WRAP_WRITE (&sample_start, "P86 sample start", sizeof (char), 3);
			WRAP_WRITE (&p86->samples[i]->length, "P86 sample length", sizeof (char), 3);
			sample_start += p86->samples[i]->length;
		} else {
			WRAP_WRITE (&sample_unset, "P86 unset sample start", sizeof (char), 3);
			WRAP_WRITE (&sample_unset, "P86 unset sample length", sizeof (char), 3);
		}
	}

	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			src = p86->samples[i]->src;
			SEEK_CHECK (SEEK_SET, p86->samples[i]->pos) {
				CLEANUP_BUFFER;
				ERROR_SEEK ("P86 sample source", SEEK_SET, p86->samples[i]->pos);
				return 4;
			};
			length = p86->samples[i]->length;
			do {
				length_read = PMD_ReadBuffer (src, length, buffer, (unsigned int)buf_size);
				if (length_read == -1) {
					CLEANUP_BUFFER;
					ERROR_READ ("P86 partial sample (into buffer)", (length > (unsigned long)buf_size) ? (unsigned long)buf_size : length);
					return 4;
				};

				WRAP_WRITE (buffer, "P86 partial sample (from buffer)", sizeof (char), (unsigned int)length_read);
				length -= (unsigned int)length_read;
			} while (length != 0);
		}
	}

	CLEANUP_BUFFER;
	return 0;
}
#undef WRAP_WRITE
#undef CLEANUP_BUFFER

/*
 * Sets a sample entry in a P86 struct
 *
 * 0 - OK
 * 1 - invalid (NULL) p86_struct
 * 2 - length > P86_LENGTHMAX
 * 3 - sample alloc failure
 */
int P86_SetSample (p86_struct* p86, unsigned char id, FILE* src, unsigned long pos, unsigned long length) {
	p86_sample* newSample;

	TRACE (("P86_SetSample", "Setting sample %3u to pos %lu, length %lu", id, pos, length));
	if (p86 == NULL) {
		PMD_SetError ("Invalid (NULL) p86 struct");
		return 1;
	}
	if (length > P86_LENGTHMAX) {
		PMD_SetError ("Sample length %luB exceeds maximum P86 sample size (%luB)", length,  P86_LENGTHMAX);
		return 2;
	}

	ALLOC_CHECK (newSample, sizeof (p86_sample)) {
		ERROR_ALLOC ("P86 sample struct", sizeof (p86_sample));
		return 3;
	}
	newSample->src = src;
	newSample->pos = pos;
	newSample->length = length;

	if (p86->samples[id] != NULL) {
		TRACE (("P86_SetSample", "Sample %3u is already set, unsetting existing one first", id));
		P86_UnsetSample (p86, id);
	}
	p86->samples[id] = newSample;

	return 0;
}

/*
 * Unsets a sample entry in a P86 struct
 *
 * 0 - OK
 * 1 - invalid (NULL) p86_struct
 */
int P86_UnsetSample (p86_struct* p86, unsigned char id) {
	TRACE (("P86_UnsetSample", "Unsetting sample %3u", id));

	if (p86 == NULL) {
		PMD_SetError ("Invalid (NULL) p86 struct");
		return 1;
	}
	if (p86->samples[id] == NULL) {
		TRACE (("P86_UnsetSample", "Sample %3u already not set, ignoring call", id));
		return 0;
	}

	free (p86->samples[id]);
	p86->samples[id] = NULL;
	return 0;
}
