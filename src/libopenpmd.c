#include <stdio.h>
#include <string.h>

#include "common.h"

#define DEBUG_PMD_FILE "RAVETUNE.M2"

/*
 * dummy, might be populated when there's a use case?
 */
unsigned char pmdReadSequenceNoChannel (FILE* pmdFile) {
  /*
   * pmdFile has to be carried through for function signature reasons
   * manually "mark" it as unused like this so compiler is happy
   */
  UNUSED (pmdFile);
  return 1;
}

unsigned char pmdReadSequenceFM (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  argBytes[4];
  unsigned short argShorts[1];
  unsigned char  resultCmd, resultArg;
  const char*    notes[] = {"c ", "c#", "d ", "d#", "e ", "f ", "f#", "g ", "g#", "a ", "a#", "b "};

  printf ("0x%04X: ", (unsigned short) ftell (pmdFile));

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    switch (cmd[0]) {
      case 0xFF:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set instrument: %hu\n", cmd[0], argBytes[0], argBytes[0]);
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFD:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set volume: %hu\n", cmd[0], argBytes[0], argBytes[0]);
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFE:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set sound cut timeout: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          switch (argBytes[0]) {
            case 0xFF:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
                printf ("command 0x%02X   0x%02X 0x%02X             - set Ticks / Quarter: %hu\n",
                  cmd[0], argBytes[0], argBytes[1],
                  argBytes[1]
                );
              } else {
                resultCmd = resultArg;
              };
              break;
            case 0xFE:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
                printf ("command 0x%02X   0x%02X 0x%02X             - add Tempo: %hu\n",
                  cmd[0], argBytes[0], argBytes[1],
                  argBytes[1]
                );
              } else {
                resultCmd = resultArg;
              };
              break;
            case 0xFD:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
                printf ("command 0x%02X   0x%02X 0x%02X             - add Ticks / Quarter: %hu\n",
                  cmd[0], argBytes[0], argBytes[1],
                  argBytes[1]
                );
              } else {
                resultCmd = resultArg;
              };
              break;
            default:
              printf ("command 0x%02X   0x%02X                  - set TimerB value (Tempo): %hu\n",
                cmd[0], argBytes[0],
                argBytes[0]
              );
              break;
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFB:
        printf ("command 0x%02X                         - tie\n", cmd[0]);
        break;
      case 0xFA:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          /*
           * TODO
           * signed short here
           */
          printf ("command 0x%02X 0x%04X                  - set Detune: %hu\n",
            cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF9:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          printf ("command 0x%02X 0x%04X                  - start loop, stop at: 0x%04X\n",
            cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF8:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          resultArg = fileReadShorts (pmdFile, 1, argShorts);
          if (resultArg) {
            printf ("command 0x%02X   0x%02X 0x%02X 0x%04X      - stop  loop, amount of loops: %hu, loop counter space: %hu, start at: 0x%04X\n",
              cmd[0], argBytes[0], argBytes[1], argShorts[0],
              argBytes[0], argBytes[1], argShorts[0]
            );
          } else {
            resultCmd = resultArg;
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF7:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          printf ("command 0x%02X 0x%04X                  - exit  loop, jump to: 0x%04X\n",
            cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF6:
        printf ("command 0x%02X                         - Master Loop Start\n", cmd[0]);
        break;
      case 0xF5:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - transpose: %hu\n", cmd[0], argBytes[0], argBytes[0]);
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF4:
        printf ("command 0x%02X                         - volume up\n", cmd[0]);
        break;
      case 0xF3:
        printf ("command 0x%02X                         - volume down\n", cmd[0]);
        break;
      case 0xF2:
        resultArg = fileReadBytes (pmdFile, 4, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X 0x%02X   0x%02X 0x%02X - set Software LFO, delay: %hu, speed: %hu, step: %hu, tick count: %hu\n",
            cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3],
            argBytes[0], argBytes[1], argBytes[2], argBytes[3]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF1:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Software LFO, modulation mask: %hu\n",
            cmd[0], argBytes[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF0:
        resultArg = fileReadBytes (pmdFile, 4, argBytes);
        if (resultArg) {
          /*
           * TODO
           * signing again
           */
          printf ("command 0x%02X   0x%02X 0x%02X   0x%02X 0x%02X - set SSG Software Envelope, AR: %hu, DR: %hu, SR: %hu, RR: %hu\n",
            cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3],
            argBytes[0], argBytes[1], argBytes[2], argBytes[3]
          );
        };
        break;
      case 0xEC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set panning: %hu\n",
            cmd[0], argBytes[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE8:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set OPNA Rhythm Master Value: %hu\n",
            cmd[0], argBytes[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE7:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - transpose, add: %hi\n",
            cmd[0], argBytes[0], (char) argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE6:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - OPNA Rhythm Master Volume, add: %hi\n",
            cmd[0], argBytes[0], (char) argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE3:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - raw volume up: %hu\n",
            cmd[0], argBytes[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE2:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - raw volume down: %hu\n",
            cmd[0], argBytes[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDA:
        resultArg = fileReadBytes (pmdFile, 3, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X 0x%02X   0x%02X      - portamento from note: %s, octave: %hu to note: %s, octave: %hu - length: %hu\n",
            cmd[0], argBytes[0], argBytes[1], argBytes[2],
            notes[argBytes[0] & 0x0F], argBytes[0] >> 4, notes[argBytes[1] & 0x0F], argBytes[1] >> 4, argBytes[2]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD6:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X 0x%02X             - Set Software LFO, change speed: %hu, temporal depth: %hu\n",
            cmd[0], argBytes[0], argBytes[1],
            argBytes[0], argBytes[1]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Extend Mode Bit 0: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCB:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Software LFO, waveform: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCA:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Extend Mode Bit 1: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xC1:
        printf ("command 0x%02X                         - slur\n", cmd[0]);
        break;
      case 0xC0:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          /*
           * TODO
           * verify how argBytes[1] is signed here
           * refer to PMDMML.MAN, 2.22
           * (prolly correct like this though)
           */
          switch (argBytes[0]) {
            case 0xFE:
              printf ("command 0x%02X   0x%02X 0x%02X             - #Volumedown F+%hi\n",
                cmd[0], argBytes[0], argBytes[1],
                (char) argBytes[1]
              );
              break;
            case 0xFC:
              printf ("command 0x%02X   0x%02X 0x%02X             - #Volumedown S+%hi\n",
                cmd[0], argBytes[0], argBytes[1],
                (char) argBytes[1]
              );
              break;
            case 0xF8:
              printf ("command 0x%02X   0x%02X 0x%02X             - #Volumedown R+%hi\n",
                cmd[0], argBytes[0], argBytes[1],
                (char) argBytes[1]
              );
              break;
            case 0xF7:
              printf ("command 0x%02X   0x%02X 0x%02X             - #Volumedown P+%hi\n",
                cmd[0], argBytes[0], argBytes[1],
                (char) argBytes[1]
              );
              break;
            default:
              printf ("command 0x%02X   0x%02X 0x%02X             - UNKNOWN COMMAND\n",
                cmd[0], argBytes[0], argBytes[1]
              );
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBB:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Extend Mode Bit 5: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xB7:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - set Software LFO, temporal depth count: %hu (UNKNOWN)\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0x80:
        printf ("command 0x%02X                         - Master Loop End\n", cmd[0]);
        break;
      case 0x00: case 0x10: case 0x20: case 0x30: case 0x40: case 0x50: case 0x60: case 0x70:
      case 0x01: case 0x11: case 0x21: case 0x31: case 0x41: case 0x51: case 0x61: case 0x71:
      case 0x02: case 0x12: case 0x22: case 0x32: case 0x42: case 0x52: case 0x62: case 0x72:
      case 0x03: case 0x13: case 0x23: case 0x33: case 0x43: case 0x53: case 0x63: case 0x73:
      case 0x04: case 0x14: case 0x24: case 0x34: case 0x44: case 0x54: case 0x64: case 0x74:
      case 0x05: case 0x15: case 0x25: case 0x35: case 0x45: case 0x55: case 0x65: case 0x75:
      case 0x06: case 0x16: case 0x26: case 0x36: case 0x46: case 0x56: case 0x66: case 0x76:
      case 0x07: case 0x17: case 0x27: case 0x37: case 0x47: case 0x57: case 0x67: case 0x77:
      case 0x08: case 0x18: case 0x28: case 0x38: case 0x48: case 0x58: case 0x68: case 0x78:
      case 0x09: case 0x19: case 0x29: case 0x39: case 0x49: case 0x59: case 0x69: case 0x79:
      case 0x0A: case 0x1A: case 0x2A: case 0x3A: case 0x4A: case 0x5A: case 0x6A: case 0x7A:
      case 0x0B: case 0x1B: case 0x2B: case 0x3B: case 0x4B: case 0x5B: case 0x6B: case 0x7B:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - play note: %s, octave: %hu, length: %hu\n",
            cmd[0], argBytes[0],
            notes[cmd[0] & 0x0F], cmd[0] >> 4, argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0x0F: case 0x1F: case 0x2F: case 0x3F: case 0x4F: case 0x5F: case 0x6F: case 0x7F:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          printf ("command 0x%02X   0x%02X                  - play rest,                length: %hu\n",
            cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      default:
        printf ("command 0x%02X                         - UNKNOWN COMMAND\n", cmd[0]);
        break;
    };
  };
  return resultCmd;
}

unsigned char pmdReadSequenceRhythm (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  resultCmd;

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    /*
     * 00-7F: subroutines
     * 80-FF: sequence commands
     *
     * TODO
     * verify behaviour
     */
    if (cmd[0] < 0x80) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      printf ("command 0x%02X                         - execute Rhythm pattern: %hu\n",
        cmd[0], cmd[0]
      );
    } else {
      fseek (pmdFile, -1, SEEK_CUR);
      resultCmd = pmdReadSequenceFM (pmdFile);
    };
  };

  return resultCmd;
}

unsigned char pmdReadRhythmSubroutineCommand (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  arg[2];
  unsigned char  resultCmd;
  unsigned char  resultArg;

  unsigned char  bitCounter;
  const char*    rhythmSampleBitMap[] = {
    "Bass",
    "Snare Drum 1",
    "Low Tom",
    "Middle Tom",
    "High Tom",
    "Rim Shot",
    "Snare Drum 2",
    "Hi-Hat Close",
    "Hi-Hat Open",
    "Crash Cymbal",
    "Ride Cymbal",
    "PPS #1",
    "PPS #2",
    "PPS #3"
  };

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    if (cmd[0] == 0x0F) {
      resultArg = fileReadBytes (pmdFile, 1, arg);
      if (resultArg) {
        printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 2);
        printf ("command 0x%02X   0x%02X                  - play rest,                length: %hu\n",
          cmd[0], arg[0], arg[0]
        );
      } else {
        resultCmd = resultArg;
      }
    } else if (cmd[0] >= 0x80 && cmd[0] < 0xC0) {
      resultArg = fileReadBytes (pmdFile, 2, arg);
      if (resultArg) {
        printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 3);
        printf ("command 0x%02X   0x%02X 0x%02X             - play ", /*(command * 0x100 + bb) & 0x3FFF, length: %hu\n", */
          cmd[0], arg[0], arg[1]
        );
        for (bitCounter=0; bitCounter<8; ++bitCounter) {
          if ((arg[0] >> bitCounter) & 0x01) {
            printf ("[%s] ", rhythmSampleBitMap[bitCounter]);
          }
        }
        for (bitCounter=0; bitCounter<6; ++bitCounter) {
          if ((cmd[0] >> bitCounter) & 0x01) {
            printf ("[%s] ", rhythmSampleBitMap[bitCounter+8]);
          };
        };
        printf ("for: %hu\n", arg[1]);
      } else {
        resultCmd = resultArg;
      }
    } else if (cmd[0] == 0xFF) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      printf ("command 0x%02X                         - Subroutine Return\n", cmd[0]);
    } else if (cmd[0] > 0x00 && cmd[0] < 0x80) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      printf ("command 0x%02X                         - UNKNOWN COMMAND\n", cmd[0]);
      resultCmd = 1;
    } else {
      fseek (pmdFile, -1, SEEK_CUR);
      resultCmd = pmdReadSequenceFM (pmdFile);
    };
  };

  return resultCmd;
}

unsigned char pmdReadInstrumentSectionGetOpNumber (unsigned char i) {
  switch (i%4) {
    case 0:
      return 1;
      break;
    case 1:
      return 3;
      break;
    case 2:
      return 2;
      break;
    case 3:
      return 4;
      break;
    default:
      return 255;
      break;
  };
}

unsigned char pmdReadInstrumentSection (FILE* pmdFile) {
  unsigned char  dataID;
  unsigned char  dataValue;
  unsigned char  i;
  unsigned char  result;

  result = fileReadBytes (pmdFile, 1, &dataID);

  if (result) {
    printf ("Instrument ID: %hu\n", dataID);
    for (i=0; i<25; ++i) {
      result = fileReadBytes (pmdFile, 1, &dataValue);
      if (result) {
        printf ("Instrument Data[%02hu]: %hu%hu%hu%hu%hu%hu%hu%hu\n", i,
          (dataValue >> 7) & 0x01,
          (dataValue >> 6) & 0x01,
          (dataValue >> 5) & 0x01,
          (dataValue >> 4) & 0x01,
          (dataValue >> 3) & 0x01,
          (dataValue >> 2) & 0x01,
          (dataValue >> 1) & 0x01,
          (dataValue >> 0) & 0x01
        );
        switch (i/4) {
          case 0:
            printf ("OP[%hu]  - DT: %hu, ML: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              (dataValue >> 4) & 0x07,
              dataValue & 0x0F
            );
            break;
          case 1:
            printf ("OP[%hu]  - TL: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue & 0x7F
            );
            break;
          case 2:
            printf ("OP[%hu]  - KS: %hu, AR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              (dataValue >> 6) & 0x03,
              dataValue & 0x1F
            );
            break;
          case 3:
            printf ("OP[%hu]  - AM: %hu, DR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue >> 7,
              dataValue & 0x1F
            );
            break;
          case 4:
            printf ("OP[%hu]  - SR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue & 0x1F
            );
            break;
          case 5:
            printf ("OP[%hu]  - SL: %hu, RR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue >> 4,
              dataValue & 0x0F
            );
            break;
          case 6:
            printf ("Common - FB: %hu, AL: %hu\n",
              (dataValue >> 3) & 0x07,
              dataValue & 0x07
            );
            break;
        };
      } else {
        break;
      };
    };
  };

  return result;
}

unsigned char pmdReadSequence (FILE* pmdFile, unsigned char pmdSection) {
  unsigned char (* pmdReadSequenceTypes[5]) (FILE* x);

  pmdReadSequenceTypes[0] = pmdReadSequenceNoChannel;
  pmdReadSequenceTypes[1] = pmdReadSequenceFM;
  pmdReadSequenceTypes[2] = pmdReadSequenceFM;
  pmdReadSequenceTypes[3] = pmdReadSequenceFM;
  pmdReadSequenceTypes[4] = pmdReadSequenceRhythm;

  return (*pmdReadSequenceTypes[pmdSection]) (pmdFile);
}

/*
 * TODO
 * check if section exists before attempting to parse it
 * (compare ftell against the instrumentPointer?)
 */
unsigned char pmdReadRhythmData (FILE* pmdFile) {
  unsigned short lookupTable[MAXLENGTH_UNKNOWN_BUFFER];
  unsigned char  result;
  unsigned char  resultReturnRead;
  unsigned char  bufferReturnRead;
  unsigned short extraDataPointer;
  unsigned char  extraDataFlags[2];
  unsigned short i;
  unsigned char  lookupTableSize = 0;

  printf ("Look-up Table\n-------------\n");
  for (i=0; i<MAXLENGTH_UNKNOWN_BUFFER; ++i) {
    result = fileReadShorts (pmdFile, 1, &lookupTable[i]);
    printf ("%02hu -> 0x%04X\n", i, lookupTable[i]);
    if (ftell (pmdFile) > (long int) lookupTable[0] + 1) {
      result = 0;
    };
    if (!result || ftell (pmdFile) == (long int) lookupTable[0] + 1) {
      lookupTableSize = (unsigned char) i;
      break;
    };
  }

  if (result) {
    for (i=0; i<lookupTableSize; ++i) {
      /*
       * TODO
       * try parsing the subroutine section with the lookupTable
       */
      while (ftell (pmdFile) < lookupTable[i+1])  {
        pmdReadRhythmSubroutineCommand (pmdFile);
      };
      if (ftell (pmdFile) == lookupTable[i+1]) {
        resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
        if (resultReturnRead && bufferReturnRead == 0xFF) {
          fseek (pmdFile, -1, SEEK_CUR);
          pmdReadRhythmSubroutineCommand (pmdFile);
        } else {
          printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
          result = 0;
          break;
        };
      } else {
        printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
        result = 0;
        break;
      };
    };
    resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
    while (resultReturnRead && bufferReturnRead != 0xFF)  {
      fseek (pmdFile, -1, SEEK_CUR);
      pmdReadRhythmSubroutineCommand (pmdFile);
      resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
    };
    if (bufferReturnRead == 0xFF) {
      fseek (pmdFile, -1, SEEK_CUR);
      pmdReadRhythmSubroutineCommand (pmdFile);
    } else {
      printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
      result = 0;
    };
  };

  if (result) {
    /*
     * TODO
     * ~ 8 mystery bytes ~
     * figure out what they're for? :(
     */
    for (i=0; i<8; ++i) {
      resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
      if (!resultReturnRead) {
        result = 0;
        break;
      };
    };
  };

  if (result) {
    resultReturnRead = fileReadShorts (pmdFile, 1, &extraDataPointer);
    if (!resultReturnRead) {
      result = 0;
    };
  };

  if (result) {
    resultReturnRead = fileReadBytes (pmdFile, 2, extraDataFlags);
    if (resultReturnRead) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 4);
      printf ("Extra Data Pointer: 0x%04X\n", extraDataPointer);
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 2);
      printf ("Extra Data Type:    0x%02X\n", extraDataFlags[0]);
      if (extraDataFlags[1] != 0xFE) {
        result = 0;
      };
    };
  };

  return result;
}

unsigned char pmdLoad (char* pmdFilename) {
  struct_pmdHeader pmdHeader;
  struct_pmdOpnaTrackPointers opnaTrackPointers;
  unsigned short   i, j;
  unsigned char    bufferReturnRead;
  unsigned short   extraDataPointer;
  unsigned short   extraDataTable[256];
  unsigned char    extraDataBuffer[256];
  unsigned char    extraDataFlags[2];
  unsigned short   junk;
  unsigned char    pmdReadSuccess = 0;
  FILE*            fp = NULL;
  unsigned char    pmdSection = 0;

  /*
   * TODO
   * load based on version / whatever instead of always OPNA
   * see PMDMML_EN.MAN:
   * 1.1.3. Correspondence Between Channel Notation and Sound Sources
   */
  pmdHeader.trackPointers = (void*) &opnaTrackPointers;

  fp = fopen (pmdFilename, "rb");

  /*
   * TODO
   * this is really ugly, clean up
   * (array of function pointers in ANSI C?)
   */
  if (fp != NULL) {
    while (1) {
      pmdReadSuccess = pmdReadVersion (fp, &pmdHeader.version);
      if (!pmdReadSuccess) break;

      pmdReadSuccess = pmdReadTrackPointers (fp, OPNA, pmdHeader.trackPointers);
      if (!pmdReadSuccess) break;
      /*
      pmdReadSuccess = pmdReadRhythmSubroutineTablePointer (fp, &pmdHeader.rhythmSubroutineTablePointer);
      if (!pmdReadSuccess) break;
      */

      pmdReadSuccess = pmdReadFmInstrumentPointer (fp, &pmdHeader.fmInstrumentPointer);
      break;
    }
    if (!pmdReadSuccess) {
      printf ("Failed to read header data.\n");
    };
  } else {
    printf ("Failed to open file '%s'.\n", pmdFilename);
    pmdReadSuccess = 0;
  };

  while (1) {
    if (!pmdReadSuccess) {
      break;
    };

    printf ("HEADER\n------\n");
    printf ("VERSION ID\t0x%02x\n", pmdHeader.version);
    for (i=0; i<=5; ++i) {
      printf ("FM%d    SEQUENCE\t0x%04hX\n", i+1, opnaTrackPointers.FM[i]);
    };
    for (i=i; i<=8; ++i) {
      printf ("SSG%d   SEQUENCE\t0x%04hX\n", i-5, ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[i-6]);
    };
    printf ("ADPCM  SEQUENCE\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM);
    printf ("Rhythm SEQUENCE\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm);
    printf ("Rhythm DATA\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer);
    printf ("FM INSTRUMENTS\t0x%04hX\n", pmdHeader.fmInstrumentPointer);

    /*
    fseek (fp, pmdHeader.trackPointers.FM[0] + 1, SEEK_SET);
    */
    ++pmdSection;
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[1] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[1] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[2] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[2] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[3] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[3] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[4] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[4] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[5] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[5] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[0] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[0] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    ++pmdSection;
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[1] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[1] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[2] + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[2] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    ++pmdSection;
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    /*
     * TODO
     * CLEAN THIS MESS UP AAAAAAAAAA
     * move all this Rhythm stuff into a more general function
     */
    ++pmdSection;
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer + 1) {
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    if (ftell (fp) + 12 < pmdHeader.fmInstrumentPointer) {
      pmdReadSuccess = pmdReadRhythmData (fp);
      if (!pmdReadSuccess) {
        break;
      };
    } else {
      printf ("No Rhythm subroutines found.\n");
      /*
       * TODO
       * ~ 8 mystery bytes ~
       * figure out what they're for? :(
       */
      for (i=0; i<8; ++i) {
        pmdReadSuccess = fileReadBytes (fp, 1, &bufferReturnRead);
        if (!pmdReadSuccess) {
          break;
        };
      };

      if (pmdReadSuccess) {
        pmdReadSuccess = fileReadShorts (fp, 1, &extraDataPointer);
      };

      if (pmdReadSuccess) {
        pmdReadSuccess = fileReadBytes (fp, 2, extraDataFlags);
        if (pmdReadSuccess) {
          printf ("0x%04X: ", (unsigned short) ftell (fp) - 4);
          printf ("Extra Data Pointer: 0x%04X\n", extraDataPointer);
          printf ("0x%04X: ", (unsigned short) ftell (fp) - 2);
          printf ("Extra Data Type:    0x%02X\n", extraDataFlags[0]);
          if (extraDataFlags[1] != 0xFE) {
            pmdReadSuccess = 0;
          };
        };
      };
    };

    if (pmdReadSuccess) {
      pmdReadSuccess = fileReadShorts (fp, 1, &junk);
      while (pmdReadSuccess && junk != 0xFF00) {
        fseek (fp, -2, SEEK_CUR);
        pmdReadSuccess = pmdReadInstrumentSection (fp);
        if (!pmdReadSuccess) break;
        pmdReadSuccess = fileReadShorts (fp, 1, &junk);
        if (!pmdReadSuccess) break;
      };
    };

    if (pmdReadSuccess) {
      fseek (fp, extraDataPointer+1, SEEK_SET);
      /*
       * TODO
       * pmdReadExtraData (fp);
       */
      for (i=0; i<256; ++i) {
        pmdReadSuccess = fileReadShorts (fp, 1, &extraDataTable[i]);
        if (!pmdReadSuccess) break;
        if (extraDataTable[i] == 0x0000) break;
      };
    };

    if (pmdReadSuccess) {
      i=0;
      while (extraDataTable[i] != 0x0000) {
        fseek (fp, extraDataTable[i]+1, SEEK_SET);
        j=0;
        pmdReadSuccess = fileReadBytes (fp, 1, &extraDataBuffer[j]);
        if (!pmdReadSuccess) break;
        while (extraDataBuffer[j++] != 0x00) {
          pmdReadSuccess = fileReadBytes (fp, 1, &extraDataBuffer[j]);
          if (!pmdReadSuccess) break;
        };
        switch (i) {
          case 0:
            printf ("PPZ: %s\n", extraDataBuffer);
            break;
          case 1:
            printf ("PPS: %s\n", extraDataBuffer);
            break;
          case 2:
            printf ("PPC/P86: %s\n", extraDataBuffer);
            break;
          case 3:
            printf ("Title: %s\n", extraDataBuffer);
            break;
          case 4:
            printf ("Composer: %s\n", extraDataBuffer);
            break;
          case 5:
            printf ("Arranger: %s\n", extraDataBuffer);
            break;
          default:
            printf ("Memo: %s\n", extraDataBuffer);
            break;
        };
        ++i;
      };
    };

    break;
  }
  if (!pmdReadSuccess) {
    printf ("Error occurred.\n");
  };

  if (fp != NULL) {
    fclose (fp);
  };

  return pmdReadSuccess;
}

unsigned char pmdLoadDebug (void) {
  return pmdLoad (DEBUG_PMD_FILE);
}

int main (int argc, char* argv[]) {
  if (argc == 2) {
    return pmdLoad (argv[1]);
  } else if (argc > 2) {
    printf ("Too many arguments, sorry ;-;\n");
    return 1;
  } else {
    printf ("Hello! This program will attempt to parse any PMD file (.M/.M2) you pass it. \n\n");
    printf ("If you encounter any errors or oddities, or see the message \"UNKNOWN COMMAND\" \n");
    printf ("somewhere, I'd by happy to be informed about it on Discord. Major thankies if \n");
    printf ("you can send the failing .M(2) file, and my deepest gratitute if you can supply \n");
    printf ("the MML file as well. <3\n\n");
    printf ("This software is WIP, incomplete and unprofessional as frick.\n");
    printf ("Have fun. \\( 'v')/\n");
    return 0;
  };
}
