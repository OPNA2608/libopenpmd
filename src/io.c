#include "io.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Read count * size bytes from src to dest */
static boolean ioReadFile (void* source, void* dest, unsigned int size, unsigned int count) {
	size_t res = fread (dest, size, count, (FILE*)source);
	if (res != count) return false;
	/* TODO apply endianness correction or read byte-by-byte & reassemble */
	return true;
}

/* Write count * size bytes from src to dest */
static boolean ioWriteFile (const void* source, void* const dest, unsigned int size, unsigned int count) {
	const void* realData;
	if (false) {
		/* TODO apply endianness correction or read byte-by-byte & reassemble */
	} else {
		realData = source;
	}
	return fwrite (realData, size, count, (FILE* const)dest) == count;
}

/* Seek with mode to pos in src */
static boolean ioSeekFile (void* source, int mode, unsigned long size) {
	/* TODO truncation potential */
	return fseek ((FILE*)source, (long)size, mode) == 0;
}

/* Tell current position in src to pos */
static boolean ioPosFile (void* source, unsigned long* pos) {
	long ret = ftell ((FILE*)source);
	if (ret == -1) return false;
	*pos = (unsigned long) ret;
	return true;
}

pmd_io pmd_io_file = {
	ioReadFile,
	ioWriteFile,
	ioSeekFile,
	ioPosFile
};

/* TODO implement! */
pmd_io pmd_io_buffer = {
	ioReadFile,
	ioWriteFile,
	ioSeekFile,
	ioPosFile
};
