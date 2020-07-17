#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define ERRORMSG_LENGTH 256

void printError (char* strErrorMsg) {
	fprintf (stderr, "Error: %s\n", strErrorMsg);
}

void printHelp() {
	printf ("Usage: pmd_instrip [pmdFile] [pmdFile...]\n");
	printf ("\n");
	printf ("Caution: Not many format checks are being done on the passed files.\n");
	printf ("         If a specified file is not a PMD module, then garbage data may be \"ripped\".\n");
}

bool ripInstrument (FILE* fpPmdFile, char* strInstrumentError) {
	bool bOpParsingErrored = false;
	unsigned char ucOpOrderShim = 0;
	unsigned char ucReadLength;
	unsigned char bufInstrumentData[26];
	unsigned char ucOp;

	ucReadLength = fread (bufInstrumentData, 1, 26, fpPmdFile);
	if (ucReadLength != 26) {
		strncpy (strInstrumentError, "Failed to read all instrument data.", ERRORMSG_LENGTH - 1);
		return false;
	}

	/* Check global bits that should not be set, abort if they are (then our parsing is clearly broken!) */
	if ( (bufInstrumentData[25] & 0xC0) > 0) {
		strncpy (
			strInstrumentError,
			"Bits in Feedback / Algorithm byte found that should not be set, assuming incorrect parsing or broken PMD file.", ERRORMSG_LENGTH - 1
		);
		return false;
	}
	printf ("; NM  AG  FB\n");
	printf (
		"@%03d % 3d % 3d\n",
		bufInstrumentData[0], bufInstrumentData[25] & 0x07, (bufInstrumentData[25] >> 3) & 0x07
	);

	printf ("; AR  DR  SR  RR  SL  TL  KS  ML  DT AMS\n");
	/*
	 * Account for operator order (1-3-2-4)
	 * Please do not look at this code too closely, it's not slick
	 */
	for (ucOpOrderShim = 0; ucOpOrderShim < 4; ++ucOpOrderShim) {
		if (ucOpOrderShim == 1) {
			ucOp = 2;
		} else if (ucOpOrderShim == 2) {
			ucOp = 1;
		} else {
			ucOp = ucOpOrderShim;
		}

		/* Check bits that should not be set, abort if they are (then our parsing is clearly broken!) */
		if ( (bufInstrumentData[1 + ucOp] & 0x80) > 0
			|| (bufInstrumentData[9 + ucOp] & 0x20) > 0
			|| (bufInstrumentData[13 + ucOp] & 0x60) > 0
			|| (bufInstrumentData[17 + ucOp] & 0xE0) > 0
			|| (bufInstrumentData[5 + ucOp] & 0x80) > 0
			|| (bufInstrumentData[13 + ucOp] & 0x60) > 0
		 ) {
			strncpy (
				strInstrumentError,
				"Bits in operator data found that should not be set, assuming incorrect parsing or broken PMD file.", ERRORMSG_LENGTH - 1
			);
			bOpParsingErrored = true;
			break;
		}

		printf (
			" %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
			bufInstrumentData[9 + ucOp] & 0x1F,
			bufInstrumentData[13 + ucOp] & 0x1F,
			bufInstrumentData[17 + ucOp] & 0x1F,
			bufInstrumentData[21 + ucOp] & 0x0F,
			bufInstrumentData[21 + ucOp] >> 4,
			bufInstrumentData[5 + ucOp] & 0x7F,
			bufInstrumentData[9 + ucOp] >> 6,
			bufInstrumentData[1 + ucOp] & 0x0F,
			(bufInstrumentData[1 + ucOp] >> 4) & 0x07,
			bufInstrumentData[13 + ucOp] >> 7
		);
	}
	printf ("\n");
	return !bOpParsingErrored;
}

int main (int argc, char* argv[]) {
	FILE* fpPmdFile = NULL;
	unsigned int uiAbortedFiles = 0;
	int i;
	char* strFileName;
	char strInstrumentError[ERRORMSG_LENGTH];
	unsigned char bufSystemType[1];
	unsigned char ucFileOpsReturnVal;
	unsigned short ptrInstrumentSectionPointer[1];
	unsigned short usInstrumentSectionEndIndicator[1];
	bool bErrored;

	if (argc < 2) {
		printHelp();
		return 0;
	}

	for (i = 1; i < argc; ++i) {
		bErrored = false;
		strFileName = argv[i];
		fpPmdFile = fopen (strFileName, "rb");

		if (fpPmdFile == NULL) {
			printError (strerror (errno));
			bErrored = true;
			goto skip;
		}

		/* Try reading in general. Check first byte for system type */
		ucFileOpsReturnVal = fread (bufSystemType, 1, 1, fpPmdFile);
		if (ucFileOpsReturnVal != 1) {
			printError ("Failed to read file.");
			bErrored = true;
			goto skip;
		}
		/* Type 0x00: NEC PC */
		if (bufSystemType[0] != 0) {
			printError ("Wrong system type.");
			bErrored = true;
			goto skip;
		}

		/* Read offset pointer to instrument section */
		ucFileOpsReturnVal = fseek (fpPmdFile, 0x19, SEEK_SET);
		if (ucFileOpsReturnVal != 0) {
			printError ("Failed to seek to instrument data pointer.");
			bErrored = true;
			goto skip;
		}
		ucFileOpsReturnVal = fread (ptrInstrumentSectionPointer, 2, 1, fpPmdFile);
		if (ucFileOpsReturnVal != 1) {
			printError ("Failed to read instrument data pointer.");
			bErrored = true;
			goto skip;
		}

		/* Jump to instrument data (offset + 1) */
		ucFileOpsReturnVal = fseek (fpPmdFile, (long) ptrInstrumentSectionPointer[0] + 1, SEEK_SET);
		if (ucFileOpsReturnVal != 0) {
			printError ("Failed to seek to instrument data.");
			bErrored = true;
			goto skip;
		}

		/* Successfully jumped to instrument section. Print comment with filename now. */
		printf ("; %s\n\n", strFileName);

		/*
		 * Check if end of section reached yet, exit with success if so.
		 * If not, pass file pointer on to instrument dumping method. Get return value, abort if error occurred.
		 */
		ucFileOpsReturnVal = fread (usInstrumentSectionEndIndicator, 2, 1, fpPmdFile);
		if (ucFileOpsReturnVal != 1) {
			printError ("Failed to check for end of instrument data.");
			bErrored = true;
			goto skip;
		}
		while (usInstrumentSectionEndIndicator[0] != 0xFF00) {
			fseek (fpPmdFile, -2, SEEK_CUR);
			bErrored = !ripInstrument (fpPmdFile, strInstrumentError);
			if (bErrored) {
				printError (strInstrumentError);
				break;
			}
			/* duplicate code, too bad ¯\_(ツ)_/¯ */
			ucFileOpsReturnVal = fread (usInstrumentSectionEndIndicator, 2, 1, fpPmdFile);
			if (ucFileOpsReturnVal != 1) {
				printError ("Failed to check for end of instrument data.");
				bErrored = true;
				break;
			}
		}
		printf ("\n");

skip:

		if (bErrored) {
			if (fpPmdFile) {
				fclose (fpPmdFile);
			}
			fprintf (stderr, "Aborting %s due to errors.\n", strFileName);
			++uiAbortedFiles;
			continue;
		}
	}

	if (uiAbortedFiles > 0) {
		fprintf (stderr, "Some files were skipped due to errors.\n");
	}
	return uiAbortedFiles;
}
