@echo off
echo Cleaning...
regexcleaner.exe -d "*\.idb$|*\.obj$|buildlog\.htm|*\.res$|*\.pch$|*\.exe$|*\.dll$|*\.pdb$|*\.resources$|*\.ncb$|*\.ilk$|*\.sbr$|*\.lib$|*\.o$" "xd^redist$|^fmod$|^mac$" "xf^fmod.dll$"
cmd