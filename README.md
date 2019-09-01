# libopenpmd
A WIP project to write an open-source C library capable of parsing both the MML and the compiled versions of the PMD sound driver's music files.

The aim is to write a stripped-down, cross-platform - Windows, macOS & Linux (and ideally down to DOS on actual PC-98s) - library for parsing and handling PMD files. This goal is being approached by doing a from-the-ground-up, hopefully clean effort to understand and document the binary format based on my own experiments with the official manual and compiler with the help of prior documentation efforts.

## Roadmap

* [ ] Parse .M(2) files.
  * [ ] **Successfully parse a small selection of PC-88/PC-98, OPN(A)-targeting .M(2) files in their entirety.**
  * [ ] Expand the catalogue of understood commands by writing custom MML files, compiling them with the official compiler and analyzing the resulting files.
  * [ ] Restructure the code for higher legibility & split main parsing logic (->library part) from CLI/IO logic (-> extra binary linking against libopenpmd).
  
* [ ] Parse MML files.
  * [ ] (Almost all the same stuff as above)

* [ ] Build upon this foundation
  * [ ] Improve documentation of the .M(2) format's internals.
  * [ ] Open-Source MML -> .M(2) compiler compatible with the original applications.
  * [ ] Open-Source .M(2) -> MML decompiler capable of exceeding PMDRC.
  * [ ] Support for non-OPNA versions. (Manual lists IBM PC, FM Towns, X68000)
  * [ ] *\\( 'v')/*

## Current status
I'm pushing the state that I managed to get after ~1 week of working on it in the afternoon hours, so don't expect much here yet.

It currently crashes on .M(2) files that define Rhythm patterns, files that don't define any (like `examples/M/PAT1_P.M2`, thanks [@NoyemiK](https://github.com/NoyemiK)) appear to run through though.

Compilation on FreeDOS with Open Watcom (src/Makefile.dos) has been tested and updated previously, but not constantly since then

## Credits

- [@kajaponn](https://twitter.com/kajaponn), for writing the original PMD driver and all of the original tools.
- [@ValleyBell](https://github.com/ValleyBell), for their [documentation of the PMD format](https://raw.githubusercontent.com/ValleyBell/MidiConverters/master/pmd_SeqFormat.txt)
- [@AnhanLi](https://twitter.com/AnhanLi), for inspiring me with their [format analysis earlier this year](https://lithcore.cn/2318) to persue this project.
- [@NoyemiK](https://github.com/NoyemiK), for her .M(2) file that I may include as an example.

