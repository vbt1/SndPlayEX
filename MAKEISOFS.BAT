copy ABS.TXT CD
copy BIB.TXT CD
copy CPY.TXT CD
copy README.TXT CD
copy SoundPlayerH.bin CD
cd CD
move SoundPlayerH.bin 0.BIN
cd ..
del SoundPlayer.iso
mkisofs -sysid "SEGA SATURN" -volid "SoundPlayer" -volset "SoundPlayer" -publisher "SEGA ENTERPRISES, LTD." -preparer "SEGA ENTERPRISES, LTD." -appid "SoundPlayer" -abstract "ABS.TXT" -copyright "CPY.TXT" -biblio "BIB.TXT" -generic-boot IP.BIN -full-iso9660-filenames -o J:\MULTIB\SoundPlayer.ISO cd
