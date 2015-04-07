Run with omdconvert.exe <filename>; the output will be a file of the same name with the extension .OMD. For *nix users (including Mac), this program should run under Wine (a compatibility layer for Windows programs under Unix-like operating systems).

You will need Improv (http://improv.sapp.org/doc/web/introduction/) and a C++ compiler to compile this program. Tested under Visual Studio but I see no reason why GCC would not also work.


I compiled it on a mac by going into the improv library and doing the following:
make library
LANG=C  g++  -O3 -Iinclude -DOSXPC   -o omdconvert ../omdconvert.cpp -Llib -limprov  -framework CoreMIDI -framework CoreServices -framework AudioUnit -framework AudioToolbox && strip omdconvert

