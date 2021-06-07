# libopenpmd

A WIP project to write an open-source C library capable of parsing both the MML and the compiled versions of the PMD sound driver's music files.

The aim is to write a stripped-down, cross-platform - Windows, macOS & Linux (and ideally down to DOS on actual PC-98s) - library for parsing and handling PMD files. This goal is being approached by doing a from-the-ground-up, hopefully clean effort to understand and document the binary format based on my own experiments with the official manual and compiler with the help of prior documentation efforts.

## Roadmap

- [ ] - Not started
- [D] - Not working
- [C] - Can fully parse existing file
- [B] - Can properly modify & overwrite existing file
- [A] - Can generate file from scratch, either empty or from supplied data
- [x] - Full support

---

* [ ] Handle PMD-related bank formats
  * [C] P86
  * [ ] PPS
  * [ ] PPC
  * [ ] PVI
  * [ ] PZI
  * [ ] FF
  * ... Other, non-PC-98 bank formats
* [ ] Handle PC-98 PMD modules
* [ ] Handle PC-98 MML files
* -> A PC-98 PMD compiler (reasonably accurate to the original one) & decompiler

### Further ideas

* Support for non-PC-98 module & MML formats?
  * IBM PC
  * FM Towns
  * X68000
  * PX
* An expanded PMD syntax & custom PMD version?
* A PMD module space optimiser? (e.g. loop analysis & unrolling)
* A new PMD player???

## Current status

I'm redoing the codebase. I'll start with smaller, more urgent goals like the various sample / patch bank formats to get
a better feel for the language & platform limitations.

## Credits

- [@kajaponn](https://twitter.com/kajaponn), for writing the original PMD driver and all of the original tools.
- [@ValleyBell](https://github.com/ValleyBell), for their [documentation of the PMD format](https://raw.githubusercontent.com/ValleyBell/MidiConverters/master/pmd_SeqFormat.txt).
- [@AnhanLi](https://twitter.com/AnhanLi), for inspiring me with their [format analysis earlier this year](https://lithcore.cn/2318) to persue this project.
- [@NoyemiK](https://github.com/NoyemiK), for her .M(2) file that I may include as an example.
- [@Blargzargo](https://www.youtube.com/channel/UCDZR3q3anQ9boE6IAvorz8Q), for his independent [documentation of PMD and MML](https://pastebin.com/raw/FP5q8zgC).
- [@Ravancloak](https://ravancloak.bandcamp.com/), for her example P86 bank.

