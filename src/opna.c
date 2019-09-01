#include "opna.h"

unsigned char pmdReadOpnaTrackPointers (
  FILE*           readFile,
  void*           pmdTrackPointers
) {
  unsigned char   result, i;

  struct_pmdOpnaTrackPointers* opnaTrackPointers =
    (struct_pmdOpnaTrackPointers*) pmdTrackPointers;

  unsigned short* map[12];
  for (i = 0; i < 6; i++) {
    map[i] = &(*opnaTrackPointers).FM[i];
  };
  for (i = i; i < 9; i++) {
    map[i] = &(*opnaTrackPointers).SSG[i-6];
  };
  map[i++] = &(*opnaTrackPointers).ADPCM;
  map[i++] = &(*opnaTrackPointers).Rhythm;
  map[i++] = &(*opnaTrackPointers).RhythmPatternTablePointer;

  for (i = 0; i < 12; i++) {
    result = fileReadShorts (readFile, 1, map[i]);
    if (!result) break;
  };

  return result;
}
