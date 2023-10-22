RB library (05/04/06)

partially platform independent console dev libs
Simple and small, intended to aid and speed up
the first development phases.

Copyright (c) 2004/2005 Thomas Fuchs / The Rockin'-B, www.rockin-b.de   



Note:
Please use it as a library(not sourcecode cut'n'paste) whenever you can.
This keeps modularity and you can easily upgrade to newer versions.
When you found or fixed a bug or even applied some additions,
please share with us.



Installataion:

Put folder RB under SGL/PROJECTS.
Either specify the include search path SGL/PROJECTS/RB/INC in the makefile or in some .BAT file as environment variable.
Either specify library search path RB/BUILD/Saturn in the makefile
or specify location of RB lib in OBJECTS file like ../RB/BUILD/Saturn/libRB.a



Platforms:

RB/BUILD/Dev-Cpp 	is for Windows
RB/BUILD/ProjectBuilder is for MacOS X, now is called XCode
RB/BUILD/Saturn 	is for SEGA Saturn

Platform independend is the TXT stuff:
TXT_SCROLL, TXT_MENU, TXT_MESSAGES, TXT_STRING
as well as the MAP and POL stuff
Others can be easily made platform independent by using the TXT stuff:
EmuStats, werehere
Others use platform specific stuff

The txt scroll can output using
SGL slPrint(),
SDL SFont, SDL KBF font,
shell standard output using printf() (last tested long ago)
It needs fonts with equally sized characters like courier new.
The supplied fonts don't always meet this(see RB_TEST for TXT_STRING )



Unfinished stuff:

TXT_STRING almost finished, very usefull for efficient text/string input using the pad

DUAL faaaaaar from being finished, no docs, all in my head



Samples:

lib		example look here
CartRAM		GameBoy, WonderSwan, NeoGeoPocket color emu
EmuStats	any of the 4 emus
MinCD		any of the 4 emus or SoundPlayer
TXT_MENU	SoundPlayer, RB_DRIVING
PCM		SoundPLayer
MAP/POL		RB_SaturnMAP, RB_DRIVING
