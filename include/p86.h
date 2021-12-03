#ifndef P86_H
#define P86_H

#include <stdio.h>
#include "common.h"

typedef enum {
	MEM, FIL
} p86_sample_type;

typedef struct {
	signed char* data;
} p86_sample_mem;

typedef struct {
	FILE* file;
	long fileStart;
	unsigned long offset;
} p86_sample_fil;

typedef union {
	p86_sample_mem mem;
	p86_sample_fil fil;
} p86_sample_data;

typedef struct {
	unsigned char id;
	p86_sample_type type;
	unsigned long length;
	p86_sample_data typeData;
} p86_sample;

typedef struct {
	unsigned char version;
	p86_sample* samples[256];
} p86_struct;

extern const char P86_MAGIC[12];
extern const unsigned short P86_HEADERLENGTH;
extern const unsigned long P86_LENGTHMAX;

/*
 * Reads a P86 file from FILE pointer p86File and parses its contents into a P86 struct.
 *
 * This function will load all samples into memory. On a system with minimal amounts of memory, P86_ImportFileSlim may
 * be preferred.
 */
p86_struct* P86_ImportFile (FILE* p86File);

/*
 * Reads a P86 file from FILE pointer p86File and parses its contents into a p86File-referencing P86 struct.
 *
 * This function will only parse the sample locations & store them as as FILE* + offset pairs.
 * You are responsible for keeping the handed-in FILE* opened!
 */
p86_struct* P86_ImportFileSlim (FILE* p86File);

/*
 * TODO Implement
 *
 * Reads a P86 file from void pointer p86Data and parses its contents into a P86 struct.
 */
p86_struct* P86_ImportData (void* p86Data);

/*
 * Writes a P86 file from p86 pointer p86 to FILE pointer p86File.
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - Failed to write to file
 * * 2 - Failed to read from file (for slim pmd_sample loading)
 * * 3 - file read buffer allocation failure
 */
int P86_ExportFile (const p86_struct* p86, FILE* p86File);

/*
 * TODO Implement
 *
 * Writes a P86 file from p86 pointer p86 to voidpointer p86Data.
 */
int P86_ExportData (p86_struct* p86, void* p86Data);

/*
 * Creates a new blank P86 bank in memory.
 */
p86_struct* P86_New ();

/*
 * Free a P86 struct.
 */
void P86_Free (p86_struct* p86);

/*
 * Free a P86 sample struct.
 */
void P86_FreeSample (p86_sample* sample);

/*
 * Validates a p86 struct.
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - Some error (TODO more specific error codes!)
 */
int P86_Validate (const p86_struct* p86);

/*
 * Map sample data to specified ID.
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - Sample data buffer allocation failure
 * * 2 - p86_sample struct allocation failure
 */
int P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data);

/*
 * Get a pointer to the sample data with the specified ID.
 * Returns NULL if ID is not set.
 */
const p86_sample* P86_GetSample (p86_struct* p86, unsigned char id);

/*
 * Map sample data to next free ID.
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - All bank IDs are already in use
 */
int P86_AddSample (p86_struct* p86, unsigned long length, signed char* data);

/*
 * Unmap sample data at specified ID.
 *
 * TODO Currently cannot fail, maybe make void instead?
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - Some error
 */
int P86_UnsetSample (p86_struct* p86, unsigned char id);

/*
 * Unmap sample data at specified ID and shift all IDs afterwards.
 *
 * Possible return values:
 * * 0 - No Error
 * * 1 - p86_sample struct allocation failure
 */
int P86_RemoveSample (p86_struct* p86, unsigned char id);

/*
 * Switches the sample data between two IDs.
 */
int P86_SwitchSample (p86_struct* p86, unsigned char from, unsigned char to);

/*
 * Checks if ID id is set (length != 0).
 */
int P86_IsSet (const p86_struct* p86, unsigned char id);

/*
 * Prints details about P86 data.
 */
void P86_Print (const p86_struct* p86);

/*
 * Prints the string representation ("major.minor") of a P86 version struct into the supplied buf variable.
 *
 * Returns the return value of sprintf.
 */
int P86_GetVersionString (const unsigned char* version, char* buf);

/*
 * Calculates the raw size needed for a p86_struct p86 if it were to be written to a file.
 * This may exceed actual P86 size limitations with future bank optimisations.
 * (Only data type limits - unsigned long - are checked against)
 */
unsigned long P86_GetTotalLength (const p86_struct* p86);

#endif /* P86_H */
