#ifndef PPC_H
#define PPC_H

#include <stdio.h>
#include "common.h"

typedef struct {
	unsigned char id;
	unsigned long length;
	adpcm_stream* data;
} ppc_sample;

typedef struct {
	/* quasi-fixed version */
	ppc_sample* samples[256];
} ppc_struct;

extern char PPC_MAGIC[31];
extern unsigned short PPC_HEADERLENGTH;
extern unsigned long PPC_LENGTHMAX;

/*
 * Reads a PPC file from FILE pointer ppcFile and parses its contents into a PPC struct.
 */
ppc_struct PPC_ImportFile (FILE* ppcFile);

/*
 * TODO Implement
 *
 * Reads a PPC file from void pointer ppcData and parses its contents into a PPC struct.
 */
ppc_struct PPC_ImportData (void* ppcData);

/*
 * Writes a PPC file from ppc pointer ppc to FILE pointer ppcFile.
 */
int PPC_ExportFile (ppc_struct* ppc, FILE* ppcFile);

/*
 * TODO Implement
 *
 * Writes a PPC file from ppc pointer ppc to voidpointer ppcData.
 */
int PPC_ExportData (ppc_struct* ppc, void* ppcData);

/*
 * Free a PPC struct.
 */
int PPC_Free (ppc_struct* ppc);

/*
 * Validates a ppc struct.
 */
int PPC_Validate (ppc_struct* ppc);

/*
 * Map sample data to specified ID.
 */
int PPC_SetSample (ppc_struct* ppc, unsigned char id, unsigned long length, signed char* data);

/*
 * Map sample data to next free ID.
 */
int PPC_AddSample (ppc_struct* ppc, unsigned long length, signed char* data);

/*
 * Unmap sample data at specified ID.
 */
int PPC_UnsetSample (ppc_struct* ppc, unsigned char id);

/*
 * Unmap sample data at specified ID and shift all IDs afterwards.
 */
int PPC_RemoveSample (ppc_struct* ppc, unsigned char id);

/*
 * Switches the sample data between two IDs.
 */
int PPC_SwitchSample (ppc_struct* ppc, unsigned char from, unsigned char to);

/*
 * Checks if ID id is set (length != 0).
 */
int PPC_IsSet (ppc_struct* ppc, unsigned char id);

/*
 * Prints details about PPC data.
 */
void PPC_Print (ppc_struct* ppc);

/*
 * Prints the string representation ("major.minor") of a PPC version struct into the supplied buf variable.
 * The value returned by this is fixed to 4.4, see PPC_MAGIC.
 *
 * Returns the return value of sprintf.
 */
int PPC_GetVersionString (unsigned char* version, char* buf);

/*
 * Calculates the total size needed for a ppc_struct ppc if it were to be written to a file.
 */
unsigned long PPC_GetTotalLength (ppc_struct* ppc);

#endif /* PPC_H */
