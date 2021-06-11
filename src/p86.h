#ifndef P86_H
#define P86_H

#include <stdio.h>
#include "common.h"

typedef struct {
	unsigned char id;
	unsigned long length;
	signed char* data;
} p86_sample;

typedef struct {
	unsigned char version;
	p86_sample* samples[256];
} p86_struct;

extern char P86_MAGIC[12];
extern unsigned short P86_HEADERLENGTH;
extern unsigned long P86_LENGTHMAX;

/*
 * Reads a P86 file from FILE pointer p86File and parses its contents into a P86 struct.
 */
p86_struct P86_ImportFile (FILE* p86File);

/*
 * TODO Implement
 *
 * Reads a P86 file from void pointer p86Data and parses its contents into a P86 struct.
 */
p86_struct P86_ImportData (void* p86Data);

/*
 * Writes a P86 file from p86 pointer p86 to FILE pointer p86File.
 */
boolean P86_ExportFile (p86_struct* p86, FILE* p86File);

/*
 * TODO Implement
 *
 * Writes a P86 file from p86 pointer p86 to voidpointer p86Data.
 */
boolean P86_ExportData (p86_struct* p86, void* p86Data);

/*
 * Free a P86 struct.
 */
boolean P86_Free (p86_struct* p86);

/*
 * Validates a p86 struct.
 */
boolean P86_Validate (p86_struct* p86);

/*
 * Map sample data to specified ID.
 */
boolean P86_SetSample (p86_struct* p86, unsigned char id, unsigned long length, signed char* data);

/*
 * Map sample data to next free ID.
 */
boolean P86_AddSample (p86_struct* p86, unsigned long length, signed char* data);

/*
 * Unmap sample data at specified ID.
 */
boolean P86_UnsetSample (p86_struct* p86, unsigned char id);

/*
 * Unmap sample data at specified ID and shift all IDs afterwards.
 */
boolean P86_RemoveSample (p86_struct* p86, unsigned char id);

/*
 * Switches the sample data between two IDs.
 */
boolean P86_SwitchSample (p86_struct* p86, unsigned char from, unsigned char to);

/*
 * Checks if ID id is set (length != 0).
 */
boolean P86_IsSet (p86_struct* p86, unsigned char id);

/*
 * Prints details about P86 data.
 */
void P86_Print (p86_struct* p86);

/*
 * Prints the string representation ("major.minor") of a P86 version struct into the supplied buf variable.
 *
 * Returns the return value of sprintf.
 */
int P86_GetVersionString (unsigned char* version, char* buf);

/*
 * Calculates the raw size needed for a p86_struct p86 if it were to be written to a file.
 * This may exceed actual P86 size limitations with future bank optimisations.
 * (Only data type limits - unsigned long - are checked against)
 */
unsigned long P86_GetTotalLength (p86_struct* p86);

#endif /* P86_H */
