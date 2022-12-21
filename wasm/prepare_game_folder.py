# TODO:
#
# generate manifest tool:
# 
# - make python tool that does the equivalent of:
# 
# ```
# find * -type f > manifest.txt
# ```
# 
# - if it finds any uppercase path, lower() the paths, rename, then rescans the list
# 
# - remove the following:
# 
# ```
# system.xvc
# *.exe
# *.dll
# v3.log
# *.pdb
# *.lib
# MapedPreferences.xml (or could keep, but then do not tolower)
# ```
# 
# - may need to manually remove save files
# - may need to add lines to verge.cfg to toggle VC compatibility features.

