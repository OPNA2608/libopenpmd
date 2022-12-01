#ifndef PMD_IO_H
#define PMD_IO_H

#include "common.h"

#include <stdio.h>

/* I/O handlers */
/* Read count * size bytes from src to dest */
typedef boolean (*pmd_io_read) (void* src, void* dest, unsigned int size, unsigned int count);
/* Write count * size bytes from src to dest */
typedef boolean (*pmd_io_write) (const void* src, void* const dest, unsigned int size, unsigned int count);
/* Seek with mode to pos in src */
typedef boolean (*pmd_io_seek) (void* src, int mode, unsigned long loc);
/* Tell current position in src to pos */
typedef boolean (*pmd_io_pos) (void* src, unsigned long* pos);

typedef struct {
	pmd_io_read io_r;
	pmd_io_write io_w;
	pmd_io_seek io_s;
	pmd_io_pos io_p;
} pmd_io;

extern pmd_io pmd_io_file;
extern pmd_io pmd_io_buffer;

/* requires src */
#define READ_CHECK(var, elemsize, elemcount)\
	if (!pmd_io_file.io_r (src, var, elemsize, elemcount))

/* requires dest */
#define WRITE_CHECK(data, elemsize, elemcount)\
	if (!pmd_io_file.io_w (data, dest, elemsize, elemcount))

/* requires src */
#define SEEK_CHECK(mode, loc)\
	if (!pmd_io_file.io_s (src, mode, loc))

#define ERROR_READ(name, size)\
	PMD_SetError ("%s read (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define ERROR_WRITE(name, size)\
	PMD_SetError ("%s write (%dB) error", name, size);\
	PMD_PrintError();\
	RAISE_TRAP;

#define ERROR_SEEK(name, mode, pos)\
	PMD_SetError ("%s seek (mode %d, pos %d) error", name, mode, pos);\
	PMD_PrintError();\
	RAISE_TRAP;

#endif /* PMD_IO_H */
