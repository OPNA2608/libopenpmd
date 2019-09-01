#ifndef LIBOPENPMD_COMMON_H
#define LIBOPENPMD_COMMON_H

#include <stdio.h>
#include <string.h>

#define MAXLENGTH_UNKNOWN_BUFFER 256
#define UNUSED(x) (void)(x)

#include "opna.h"

typedef enum {
  OPNA
} enum_pmdChip;

typedef struct {
  unsigned char  version;
  void*          trackPointers;
  unsigned short fmInstrumentPointer;
} struct_pmdHeader;

unsigned char fileReadBytes (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned char*  readResults
);

unsigned char fileReadShorts (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned short* readResults
);

unsigned char pmdReadVersion (
  FILE*           readFile,
  unsigned char*  pmdFileVersion
);

unsigned char pmdReadTrackPointers (
  FILE*           readFile,
  enum_pmdChip    readChip,
  void*           pmdFileTrackPointers
);

unsigned char pmdReadFmInstrumentPointer (
  FILE*           readFile,
  unsigned short* pmdFileFmInstrumentPointer
);

#endif /* LIBOPENPMD_COMMON_H */
