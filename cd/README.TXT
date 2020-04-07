Rockin'-B's Sound Player for SEGA Saturn 

by The Rockin'-B (version 05/03/11)
http://www.Rockin-B.de
For Bugs and feedback,
please contact/PM me at the segaXtreme forum!


Read carefully to avoid problems!

known bugs:
---------------------------------------------------------------------
 - avoid selecting repeat for high workRAM sound slot
   (it may freeze for 20s or even crash)  
 - avoid playing the file slot and a memory slot simultaneously 
   (it'll disable file slot for this run)
 - wave files in high work RAM memory slot got a glitch at start   
 - CD swap not automatically recognized, need to press rescan manually
 - may need to rescan swapped CD for PCM player and CDDA player seperately
 - leaving PCM player when playing the file sound slot makes the sound loop untill returned to the PCM player
       


(missing) features:
---------------------------------------------------------------------
    + plays CDDA
    + plays .AIF and .ADP (ADPCM compressed) files directly from CD
        (file playback)
        (find them on Sonic Jam, E0, ..)
    + plays .WAV and raw PCM from Saturn game CD's
        (memory playback)
        (find them on Burning Rangers, Panzer Dragoon Saga, ...)
    + extended CD swap support:
        (by CD stop and rescan buttons)
        (!! even without pressing the CD door switch !!)
        - swap-in Saturn game CD
        - swap-in audio CD
        - swap-in CD-R
    + playing multiple sounds at once:
        with new sound slot system:
        - 1 file slot
        - 2 memory slots(1 MByte and 256 kByte)
        additionally CDDA(untested)
    + sound attribute adjustments:
        - samplerate(and OCT/FNS to generate pitch value for devvers)
        - pan
        - mono/stereo
        - 8/16 bit resolution
    + sound file playing controls:
        - play / repeat / stop
    + CDDA additional playing control:
        - fast forward, rewind, pause, track change
    + realtime gouraud 3D model
    ? hidden 3D debug menu (activated by holding a certain button at startup)
    + improved CD browser with CD swap support
    - only wave files with PCM chunk supported
      no sound compression/encoding playable


burning instructions:
---------------------------------------------------------------------
NOTE: if you don't want to add custom sound files,
      take the included SoundPlayer.iso and go to step 3.
0.  get mkisofs.exe and Cygwin1.dll for ISO creation from
    my website or from elsewhere. Unrar and put them into the
    same directory where the cd folder is
1.  Put all your sound files(max 512kB each) in any (sub)directory
    you may create under folder cd/
    (NOTE: only the first 100 items(98 files and folders)
    of a subdirectory can be seen and used)
!!! ensure that filenames are in 8.3 format:
		filename:	<= 8 characters
		extension:	<= 3 characters
!!! extensions are case sensitive,
    only .wav .WAV .pcm .PCM .snd and .SND supported
    -> rename them if it is .wAv .wAV or similar
2.  Hit makeisofs.bat
3.  Burn the SoundPlayer.iso file on CD
    Burn slow using DiscJuggler for example:
     Mode:     Mode 1
     Block:    2048
     TOC:	CD-DA
    (I use DeepBurner, it's free and faster, than trial versions)



IMPORTANT: How to run the stuff on Saturn
---------------------------------------------------------------------
Notes on CD-R:
You got 3 options to run your CD-R on Saturn:
1.	modchip in Saturn
	-> no problems
2.	extended swap trick: swap CD twice
	-> no problems
3.	simple swap trick: swap only once
	CAUTION:
		only works correct, if the data track of your boot game cd
		is NOT SMALLER than the data track you have just burned
		example:	a 50 MB emu CD-R will not be able to load roms correct,
				if booted with SEGA Rally (24 MByte),
				but it will when booted with Bug (110 MByte).
Notes on region protection:
	this game is multiregion, if it nevertheless doesn't work on your Saturn,
	use a region converter cart or a region modded Saturn



CD browser usage:
---------------------------------------------------------------------
    - uses controller at port 1
    - exiting without loading a file:
        B-button
    - changing highlighted file or directory:
        slow:   D-pad LEFT/RIGHT
        fast:   L/R trigger
    - loading highlighted file /
      moving to highlighted directory:
        A/C/START button
    - rescan CD
        X-button
    - stop CD(for easy swap)
        Y-button

        

Sound Player menu usage:
---------------------------------------------------------------------
NOTE:   when playing raw .PCM or .SND files from game CD's
        you'll have to find out the attributes by yourself
        (bits, samplerate, mono/stereo)
NOTE:   incorrect sound attributes may make the 
        sound very distorted and LOUD
NOTE:   playback of multiple sound slots may fail, 
        if tey sum up too much channels
    - uses controller at port 1
    - change menu item
        D-PAD: up, down
    - change value(decrease/increase)
        D-PAD: left, right
        L / R trigger   (fast)
    - exit to next player(PCM player -> CDDA player -> (3D debug menu -> )PCM player)
    - hit a button
        START, A, C
    ? there is a hidden menu, ?
    ? you may find it out by holding a certain button at startup ?



Developer notes:
---------------------------------------------------------------------
This app uses some of my Saturn libs(RB_minCD, RB_TXT_MENU, RB_PCM).
For .PCM file conversion, my tool RB_SaturnPCM.EXE was used, which
uses RB_PCM, too. The 3D model was created with blender, exported as DXF,
made compatible with RB_SaturnDXF.EXE, converted to SG3 with DXF2SG3.EXE,
further material editing and SGL .C export with 3DEditor of SS-SDK Win95.

So it's very likely that this app will be released as sourcecode tutorial
in the future, showing the use of the libs and stuff mentioned above.
