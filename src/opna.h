#ifndef LIBOPENPMD_OPNA_H
#define LIBOPENPMD_OPNA_H

#include "common.h"

typedef struct {
  unsigned short FM[6];
  unsigned short SSG[3];
  unsigned short ADPCM;
  unsigned short Rhythm;
  unsigned short RhythmPatternTablePointer;
} struct_pmdOpnaTrackPointers;

unsigned char pmdReadOpnaTrackPointers (
  FILE*           readFile,
  void*           pmdTrackPointers
);

#endif /* LIBOPENPMD_OPNA_H */
