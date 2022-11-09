#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* common */

/* Raise SIGTRAP on errors, not supported on DOS */
#if defined(TRAP) && !defined(DOS)
	#include <signal.h>
	#define RAISE_TRAP raise(SIGTRAP)
#else
	#define RAISE_TRAP
#endif

/* Enable unused marker to mute some warnings */
#ifdef DEBUG
	#define UNUSED(x) (void)(x);
#else
	#define UNUSED(x)
#endif

/* Set default buffer size */
#ifndef DOS
	#define PMD_BUFFERSIZE 4096
#else
	#define PMD_BUFFERSIZE 256
#endif

typedef enum {false, true} boolean;

typedef boolean (*pmd_io_read) (FILE* source, void* dest, unsigned int size, unsigned int count);
typedef boolean (*pmd_io_write) (const void* source, FILE* const dest, unsigned int size, unsigned int count);
typedef boolean (*pmd_io_seek) (FILE* source, int mode, unsigned int size);
typedef boolean (*pmd_io_pos) (FILE* source, unsigned int* pos);

typedef struct {
	pmd_io_read io_r;
	pmd_io_write io_w;
	pmd_io_seek io_s;
	pmd_io_pos io_p;
} pmd_io;

static boolean ioRead (FILE* source, void* dest, unsigned int size, unsigned int count) {
	size_t res = fread (dest, size, count, source);
	if (res != count) return false;
	/* TODO apply endianness correction or read byte-by-byte & reassemble */
	return true;
}
static boolean ioWrite (const void* source, FILE* const dest, unsigned int size, unsigned int count) {
	const void* realData;
	if (false) {
		/* TODO apply endianness correction or read byte-by-byte & reassemble */
	} else {
		realData = source;
	}
	return fwrite (realData, size, count, dest) == count;
}
static boolean ioSeek (FILE* source, int mode, unsigned int size) {
	return fseek (source, size, mode) == 0;
}
static boolean ioPos (FILE* source, unsigned int* pos) {
	long ret = ftell (source);
	if (ret == -1) return false;
	*pos = (unsigned int) ret;
	return true;
}

pmd_io pmd_io_funcs = {
	ioRead,
	ioWrite,
	ioSeek,
	ioPos
};

long PMD_GetBuffer (void** dest);
long PMD_ReadBuffer (FILE* src, long dataLeft, void* dest, unsigned int destSize);
void PMD_SetError (const char* errorMsg, ...);
const char* PMD_GetError (void);
void PMD_PrintError (void);

/* TODO factor these out */
#define CRY_ABOUT_IT {\
	RAISE_TRAP;\
	return 1;\
}

#define CRY_ABOUT_IT_NULL {\
	RAISE_TRAP;\
	return NULL;\
}

#define ALLOC_CHECK(var, size)\
	var = calloc (size, sizeof (char));\
	if (var == NULL)

#define ERROR_ALLOC(name, size)\
	PMD_SetError ("%s alloc (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define READ_CHECK(var, elemsize, elemcount)\
	if (!pmd_io_funcs.io_r (src, var, elemsize, elemcount))

#define ERROR_READ(name, size)\
	PMD_SetError ("%s read (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define WRITE_CHECK(data, elemsize, elemcount)\
	if (!pmd_io_funcs.io_w (data, dest, elemsize, elemcount))

#define ERROR_WRITE(name, size)\
	PMD_SetError ("%s write (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define SEEK_CHECK(mode, loc)\
	if (!pmd_io_funcs.io_s (src, mode, loc))

long PMD_GetBuffer (void** dest) {
	long buf_size = PMD_BUFFERSIZE;

	while (buf_size > 0) {
		ALLOC_CHECK (*dest, buf_size) {
			buf_size /= 2;
		} else {
#ifdef DEBUG
			printf ("Got buffer of size %ld\n", buf_size);
#endif
			break;
		}
	}
	return buf_size != 0 ? buf_size : -1;
}

long PMD_ReadBuffer (FILE* src, long dataLeft, void* dest, unsigned int destSize) {
	unsigned long readAmount = dataLeft;
	if (readAmount < 1) return -1;
	if (dataLeft > destSize) readAmount = destSize;

	READ_CHECK (dest, sizeof (char), readAmount) return -2;

	return readAmount;
}

static char* pmd_error = NULL;
static long pmd_errorlength = -1L;

static int pmd_init_error (void) {
	if (pmd_error != NULL) return 1;
	pmd_errorlength = PMD_GetBuffer ((void**)&pmd_error);
	if (pmd_errorlength == -1) return 2;
	return 0;
}

void PMD_SetError (const char* errorMsg, ...) {
	va_list fArgs;
	va_start (fArgs, errorMsg);

	if (pmd_error == NULL && pmd_init_error() != 0) {
		printf ("Error init failed\n");
		va_end (fArgs);
		return;
	}
	vsprintf (pmd_error, errorMsg, fArgs);
	va_end (fArgs);
}

const char* PMD_GetError (void) {
	return pmd_error;
}

void PMD_PrintError (void) {
	printf ("%s\n", pmd_error);
}

/* P86 */

typedef struct {
	FILE* source;
	unsigned long pos;
	unsigned long length;
} p86_sample;

typedef struct {
	unsigned char version;
	p86_sample* samples[256];
} p86_struct;

/* public funcs */
p86_struct* P86_New (void);
void P86_Free (p86_struct* p86);
int P86_Read (p86_struct* p86, FILE* src);
int P86_Write (p86_struct* p86, FILE* dest);
int P86_SetSample (p86_struct* p86, unsigned char id, FILE* src, unsigned long pos, unsigned long length);
int P86_UnsetSample (p86_struct* p86, unsigned char id);

const char P86_MAGIC[12] = "PCM86 DATA\n\0";
const unsigned short P86_HEADERLENGTH = 0x610;
const long P86_LENGTHMAX = 0xFFFFFF;

/*
	not NULL - OK
	NULL - struct alloc failure
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

void P86_Free (p86_struct* p86) {
	unsigned int i;

	if (p86 == NULL) return;

	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			P86_UnsetSample (p86, i);
		}
	}
	free (p86);
	p86 = NULL;
}

#define CLEANUP_BUFFER free (buffer);
/*
	0 - OK
	1 - magic error
	2 - version error
	3 - size error
	4 - sample error
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
		RAISE_TRAP;
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
			P86_SetSample (p86, i, src, sample_start, sample_length);
		}
	}

	return 0;
}

int P86_Write (p86_struct* p86, FILE* dest) {
	long length, length_read, buf_size, sample_start, sample_unset;
	unsigned int i;
	FILE* src;
	char* buffer;

	buf_size = PMD_GetBuffer ((void**)&buffer);
	if (buf_size == -1) CRY_ABOUT_IT;

	sample_unset = 0;

	length = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			length += p86->samples[i]->length;
		}
	}
	if (length > P86_LENGTHMAX) {CLEANUP_BUFFER CRY_ABOUT_IT};

	WRITE_CHECK (P86_MAGIC, sizeof (char), 12) {CLEANUP_BUFFER CRY_ABOUT_IT};
	WRITE_CHECK (&p86->version, sizeof (char), 1) {CLEANUP_BUFFER CRY_ABOUT_IT};
	WRITE_CHECK (&length, sizeof (char), 3) {CLEANUP_BUFFER CRY_ABOUT_IT};

	sample_start = P86_HEADERLENGTH;
	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			WRITE_CHECK (&sample_start, sizeof (char), 3) {CLEANUP_BUFFER CRY_ABOUT_IT};
			WRITE_CHECK (&p86->samples[i]->length, sizeof (char), 3) {CLEANUP_BUFFER CRY_ABOUT_IT};
			sample_start += p86->samples[i]->length;
		} else {
			WRITE_CHECK (&sample_unset, sizeof (char), 3) {CLEANUP_BUFFER CRY_ABOUT_IT};
			WRITE_CHECK (&sample_unset, sizeof (char), 3) {CLEANUP_BUFFER CRY_ABOUT_IT};
		}
	}

	for (i = 0; i <= 255; ++i) {
		if (p86->samples[i] != NULL) {
			src = p86->samples[i]->source;
			SEEK_CHECK (SEEK_SET, p86->samples[i]->pos) {CLEANUP_BUFFER CRY_ABOUT_IT};
			length = p86->samples[i]->length;
			do {
				length_read = PMD_ReadBuffer (src, length, buffer, buf_size);
				if (length_read == -1) {CLEANUP_BUFFER CRY_ABOUT_IT};

				WRITE_CHECK (buffer, sizeof (char), length_read) {CLEANUP_BUFFER CRY_ABOUT_IT};
				length -= length_read;
			} while (length != 0);
		}
	}

	CLEANUP_BUFFER;
	return 0;
}
#undef CLEANUP_BUFFER

/*
	0 - OK
	1 - invalid p86_struct
	2 - sample alloc failure
*/
int P86_SetSample (p86_struct* p86, unsigned char id, FILE* src, unsigned long pos, unsigned long length) {
	p86_sample* newSample;

	if (p86 == NULL) return 1;

	ALLOC_CHECK (newSample, sizeof (p86_sample)) {
		ERROR_ALLOC ("P86 sample struct", sizeof (p86_sample));
		return 2;
	}
	newSample->source = src;
	newSample->pos = pos;
	newSample->length = length;

	if (p86->samples[id] != NULL) {
		free (p86->samples[id]);
	}
	p86->samples[id] = newSample;

	return 0;
}

/*
	0 - OK
	1 - invalid p86_struct
*/
int P86_UnsetSample (p86_struct* p86, unsigned char id) {
	if (p86 == NULL) return 1;
	if (p86->samples[id] == NULL) return 0;

	free (p86->samples[id]);
	p86->samples[id] = NULL;
	return 0;
}

/*
	0 - OK
	1 - main error
	2 - library error
*/
int main (void) {
	FILE* fileIn, * fileOut;
	p86_struct* p86 = P86_New();

	fileIn = fopen ("TEST.P86", "rb");
	if (fileIn == NULL) return 1;

	if (P86_Read (p86, fileIn)) {
		fclose (fileIn);
		P86_Free (p86);
		return 2;
	}

	fileOut = fopen ("TEST_.P86", "wb");
	if (fileOut == NULL) return 1;

	if (P86_Write (p86, fileOut)) {
		fclose (fileOut);
		P86_Free (p86);
		fclose (fileIn);
		return 2;
	}

	fclose (fileOut);
	P86_Free (p86);
	fclose (fileIn);

	return 0;
}
