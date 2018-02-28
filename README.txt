This is the source and binary distribution for the oneTeslaTS SD card interrupter. Most users do not need to download this file - this package is useful only if you are modifying or reflashing your ATMega firmware.

==Description==

The SD card interrupter is designed to be a full-featured interrupter with support for live MIDI streaming and playback of local files stored on an SD card. Great care was taken to ensure that all source remained Arduino-compatible, and that the entire firmware would run on a single ATMega328 microcontroller. It features:

*Streaming from MIDI devices, including computers and keyboards.
*Playback of MIDI-like files ("OMD") from a micro SD card.
*Fixed mode with precise timer-based frequency control down to 1Hz.
*Small firmware size using fixed-point math, no softfloat libraries or FPU needed.


==Building==

This firmware has been developed on Arduino 1.0.3. We do not recommend using other versions due to Arduino's occasional tendency to break backwards compatibility. You will need to copy the libraries in inc/ to arduino-1.0.3/libraries. After that, it should be as simple as opening src/main/main.ino and clicking "Verify/Compile" in the Arduino IDE.

==Compatibility==

The interrupter is compatible with MicroSD/SDHC cards formatted with a FAT16 or FAT32 filesystem. Long filenames up to 99 characters are supported, as well as support for one level of directory.

==File description==

src/main
    |------ constants.h: assorted user-adjustable compile-time settings
    |------ data.h: include file for global variables
    |------ datatypes.h: program data structures
    |------ lcd.h, lcd.cpp: abstraction layer for the display; currently just a wrapper around Arduino's libraries.
    |------ main.ino: the main program loops
    |------ player.h, player.cpp: MIDI event handlers
    |------ sdsource.h, sdsource.cpp: SD card MIDI source
    |------ serialsource.h, serialsource.cpp: Hardware MIDI source
    |------ system.h, system.cpp: nonportable ATMega-specific routines
    |------ timers.h, timers.cpp: nonportable ATMega-specific ISR's
    |------ util.h, util.cpp: various utility functions

inc/
    |------ LiquidCrystal: Hitachi LCD driver library
    |------ PFFS: Petit FatFS filesystem driver
    |------ SdFat: Full featured FAT32 library


bin/
    |------ SDinterrupter.hex: pre-compiled binary for your convenience

omd/
    |------ omdconvert/
    |            |------ improv/: Improv MIDI library
    |            |------ omdconvert.exe: MIDI to OMD converter
    |            |------ omdconvert: Mac version of the converter
    |            |------ omdconvert.cpp: Source for the converter
    |            |------ README.txt: README for the converter
    |
    |------ *.mid, *.OMD: sample files

README.txt: this file :)

==CREDITS==
Props to the writers of Improv, PFFS, and SdFAT for making this possible. Special mention to Stridera on our forums, who implemented SDHC support and a bunch of neat fixes.

==LICENSING==

The libraries in inc/ are licensed under their authors' respective licensing. You are free to modify and redistribute the source and binaries found in this package for NONCOMMERCIAL USE as long as credit is given and this notice is included. Please note that in doing so, you give oneTesla, LLC nonexclusive, royalty-free license to use any such modifications for any purpose.

(We're normally not a fan of no-commercial-use source licenses as it hinders the usefulness of the code, but here we believe it is reasonable since it prevents others from unfairly mooching off our hard work while still leaving the source available for the community to use. For commerical use please contact us for further licensing terms, and we can probably work something out.)

==LEGAL DISCLAIMER==

Disclaimer of Warranty.
THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

Limitation of Liability.
IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

If the disclaimer of warranty and limitation of liability provided above cannot be given local legal effect according to their terms, reviewing courts shall apply local law that most closely approximates an absolute waiver of all civil liability in connection with the Program, unless a warranty or assumption of liability accompanies a copy of the Program in return for a fee.



