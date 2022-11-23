#ifndef PMD_P86_H
#define PMD_P86_H

#include <stdio.h>

typedef struct {
	FILE* src;
	unsigned long pos;
	unsigned long length;
} p86_sample;

typedef struct {
	unsigned char version;
	p86_sample* samples[256];
} p86_struct;

/*
 * Initialises a new P86 struct
 *
 * not NULL - OK
 * NULL - structalloc failure
 */
p86_struct* P86_New (void);

/*
 * Free's a P86 struct
 *
 * Never fails
 */
void P86_Free (p86_struct* p86);

/*
 * Reads a P86 file into a struct
 *
 * 0 - OK
 * 1 - magic error
 * 2 - version error
 * 3 - size
 * 4 - sample error
 */
int P86_Read (p86_struct* p86, FILE* src);

/*
 * Writes a P86 struct to a file
 *
 * 0 - OK
 * 1 - I/O buffer alloc error
 * 2 - total size > format max
 * 3 - dest write error
 * 4 - source seek/read error
 */
int P86_Write (p86_struct* p86, FILE* dest);

/*
 * Sets a sample entry in a P86 struct
 *
 * 0 - OK
 * 1 - invalid p86_struct
 * 2 - sample alloc failure
 */
int P86_SetSample (p86_struct* p86, unsigned char id, FILE* src, unsigned long pos, unsigned long length);

/*
 * Unsets a sample entry in a P86 struct
 *
 * 0 - OK
 * 1 - invalid p86_struct
 */
int P86_UnsetSample (p86_struct* p86, unsigned char id);

#endif
