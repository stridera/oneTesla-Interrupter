Run with omdconvert.exe <filename>; the output will be a file of the same name with the extension .OMD. For *nix users (including Mac), this program should run under Wine (a compatibility layer for Windows programs under Unix-like operating systems).

You will need Improv (snapshot included) and a C++ compiler to compile this program. Tested under Visual Studio but I see no reason why GCC would not also work.

Added 4/7/2015:
===============
To compile with GCC on a Mac:

LANG=C  g++  -O3 -Iinclude -DOSXPC   -o omdconvert ../omdconvert.cpp -Llib -limprov  -framework CoreMIDI -framework CoreServices -framework AudioUnit -framework AudioToolbox && strip omdconvert

