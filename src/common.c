#include "common.h"

unsigned char fileReadBytes (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned char*  readResults
) {
  unsigned char   buffer[MAXLENGTH_UNKNOWN_BUFFER];
  size_t          readActualCount;
  unsigned char   result;
  unsigned char   i;

  readActualCount = fread (buffer, 1, readCount, readFile);
  result          = (readCount == readActualCount);

  if (result) {
    for (i = 0; i < readCount; i++) {
      readResults[i] = buffer[i];
    };
  };

  return result;
}

unsigned char fileReadShorts (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned short* readResults
) {
  unsigned short  buffer[MAXLENGTH_UNKNOWN_BUFFER/2];
  size_t          readActualCount;
  unsigned char   result;
  unsigned char   i;

  readActualCount = fread (buffer, 2, readCount, readFile);
  result          = (readCount == readActualCount);

  if (result) {
    for (i = 0; i < readCount; i++) {
      readResults[i] = buffer[i];
    };
  };

  return result;
}

unsigned char pmdReadVersion (
  FILE*           readFile,
  unsigned char*  pmdFileVersion
) {
  unsigned char   buffer;
  unsigned char   result;

  result = fileReadBytes (readFile, 1, &buffer);

  if (result) {
    (*pmdFileVersion) = buffer;
  };

  return result;
}

unsigned char pmdReadTrackPointers (
  FILE*           readFile,
  enum_pmdChip    readChip,
  void*           pmdFileTrackPointers
) {
  unsigned char  result;

  switch (readChip) {
    case OPNA:
      result = pmdReadOpnaTrackPointers (
        readFile, pmdFileTrackPointers
      );
      break;
    default:
      /*
       * TODO
       * nicely propagate and log the error,
       * then abort somehow?
       */
      result = 1;
      break;
  };

  return result;
}

unsigned char pmdReadFmInstrumentPointer (
  FILE*           readFile,
  unsigned short* pmdFileFmInstrumentPointer
) {
  unsigned short  buffer;
  unsigned char   result;

  result = fileReadShorts (readFile, 1, &buffer);

  if (result) {
    (*pmdFileFmInstrumentPointer) = buffer;
  }

  return result;
}


