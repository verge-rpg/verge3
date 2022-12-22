#!/usr/bin/env python

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

import os
import sys
import glob
import fnmatch

SKIPPED_FILENAMES = [
    'MapedPreferences.xml',
    'manifest.txt',
]

DELETED_FILENAMES = [
    '*.exe',
    '*.dll',
    '*.bat',
    '*.sh',
    'v3.log',
]

def run(game_folder_path):
    oldcwd = os.getcwd()

    try:
        os.chdir(game_folder_path)
        paths = set(glob.glob('**/*', recursive=True))
        paths_to_fix_case = set()
        paths_to_skip = set()
        paths_to_delete = set()

        for path in paths:
            basename = os.path.basename(path)
            basename_lower = basename.lower()
            if any(fnmatch.fnmatch(basename_lower, fn) for fn in SKIPPED_FILENAMES):
                paths_to_skip.add(path)
            elif any(fnmatch.fnmatch(basename_lower, fn) for fn in DELETED_FILENAMES):
                paths_to_delete.add(path)            
            elif basename != basename_lower:
                paths_to_fix_case.add(path)

        for path in paths_to_skip:
            print('omitting ' + path + ' from manifest')
            paths.remove(path)
        for path in paths_to_delete:
            print('delete ' + path)
            os.remove(path)
            paths.remove(path)
        for path in sorted(paths_to_fix_case, key=lambda x: x.count(os.sep) + (x.count(os.altsep) if os.altsep else 0)):
            path_lower = path.lower()

            print('rename ' + path + ' to ' + path_lower)

            os.rename(path, path_lower)
            paths.remove(path)
            paths.add(path_lower)

        file_paths = list(filter(os.path.isfile, paths))
        dir_paths = [path + '/' for path in filter(os.path.isdir, paths)]
        manifest_paths = file_paths + dir_paths

        print('writing manifest.txt...')
        with open('manifest.txt', 'w') as f:
            f.writelines(list(sorted(path.replace('\\', '/') + '\n' for path in manifest_paths)))

        print('')
        print('DONE!')
        print('')
        print('After this, you should tweak verge.cfg to adjust compatibility options and test things out.')
        print('')
        print('You may need to do more manual cleanup / fixes, but ideally anything that is a compatibility issue should be reported, so it can be fixed/implemented in Verge.'
            + ' This is preferable to one-off modifications, because then old games can be run via config modifications only, without any source modification required.')
    finally:
        os.chdir(oldcwd)

if __name__ == '__main__':
    def usage():
        sys.exit(
            'Usage: ' + sys.argv[0] + ' game_folder_path\n'
            + 'NOTE: back up any originals before running this!')

    if len(sys.argv) != 2:
        usage()

    run(sys.argv[1])

