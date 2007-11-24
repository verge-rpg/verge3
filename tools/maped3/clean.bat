@echo off
echo Cleaning...
regexcleaner.exe -d "*.idb$|*.obj$|buildlog.htm|*.res$|*.pch$|*.exe$|*.dll$|*.pdb$|*.resources$|*.ncb$|*.ilk$" "^MRU.dll$|^zlib.net.dll$|^RegisterFileType.dll$"
rd /q /s app\obj
rd /q /s app\bin
rd /q /s debug
cmd