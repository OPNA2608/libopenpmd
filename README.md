# libopenpmd

A WIP project to write an open-source C library capable of parsing both the MML and the compiled versions of the PMD
sound driver's music files.

The aim is to write a stripped-down, cross-platform - Windows, macOS, Linux and DOS on actual PC-98s - library for
parsing and handling PMD files. This goal is being approached by doing a from-the-ground-up, hopefully clean effort to
understand and document the binary formats based on my own experiments with the official manual and compiler with the
help of prior documentation efforts.

## Roadmap

- [ ] - Not started
- [D] - Not working
- [C] - Can fully parse existing file
- [B] - Can properly modify & overwrite existing file
- [A] - Can generate file from scratch, either empty or from supplied data
- [x] - Implemented to a satisfactory degree

---

* [ ] Handle PMD-related bank formats
  * [A] P86
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

Currently writing code to parse, load, create & modify P86 banks. Figuring out what the API should look like as I go,
depending on what I need to implement everything to a satisfactory degree. From there it's just a matter of copy-pasting
function declarations & reimplementing them according to the formats, at least for the simple banks.

## Credits

- [@kajaponn], for writing the original PMD driver and all of the original tools.
- [@ValleyBell], for their [documentation of the PMD format].
- [@AnhanLi], for inspiring me with their [format analysis earlier this year] to pursue this project.
- [@NoyemiK], for her .M(2) file that I may include as an example.
- [@Blargzargo], for his independent [documentation of PMD and MML].
- [@Ravancloak], for her example P86 bank.

[@kajaponn]: https://twitter.com/kajaponn
[@ValleyBell]: https://github.com/ValleyBell
[@AnhanLi]: https://twitter.com/AnhanLi
[@NoyemiK]: https://github.com/NoyemiK
[@Blargzargo]: https://www.youtube.com/channel/UCDZR3q3anQ9boE6IAvorz8Q
[@Ravancloak]: https://ravancloak.bandcamp.com/

[documentation of the PMD format]: https://raw.githubusercontent.com/ValleyBell/MidiConverters/master/pmd_SeqFormat.txt
[format analysis earlier this year]: https://lithcore.cn/2318
[documentation of PMD and MML]: https://pastebin.com/raw/FP5q8zgC
