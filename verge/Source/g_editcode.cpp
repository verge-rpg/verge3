/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#include "xerxes.h"
extern VCCore *vc;

#ifdef ALLOW_SCRIPT_COMPILATION

// Runtime code compilation and loading functions

// Call to run code directly. Wraps it into a function,
// spits it to a file, compiles it, loads it, runs
// the function and then unloads it. Uses the TEMP core.
// On error during compilation, shows a message box to the user.
// errors during code evalution are handled as usual. (err)
// Code is considered map-local for scope purposes.
void runEval(char *code)
{
	char *fname = "__temp_eval.vc";
	char *fname_compiled = "__temp_eval.vc.out";

	if(releasemode) {
		err("Can't eval code in release mode!");
	}

	// write code to file, wrapped in a function
	// added \n after code to avoid an ending // causing problems
	FILE * f = fopen(fname, "w");
	fprintf(f, "void __temp_eval(){%s\n}", code);
	fclose(f);

	// compile against map and system, replacing temp image
	std::vector<int> images;
	images.push_back(CIMAGE_MAP);
	images.push_back(CIMAGE_SYSTEM);

	// try to compile.
	// Allow dup function names because we're going to patch
	if(!vcc->CompileOther(fname, CIMAGE_TEMP, false, true, images)) {
		showMessageBox(vcc->errmsg);
		remove(fname);
		return;
	}

	// load the compiled version
	VFILE *fin = vopen(fname_compiled);
	if(!fin) err("RunString: Couldn't open temp compiled code file.");

	// note we patch any existing __temp_eval function to this one;
	// once we unload the core below, it will revert to any
	// previous definition.
	vc->LoadCore(fin, CIMAGE_TEMP, false, true);

	vclose(fin);
	remove(fname);
	remove(fname_compiled);

	// run it!
	vc->ExecuteFunctionString("__temp_eval");

	// finished, unload it
	vc->UnloadCore(CIMAGE_TEMP);
}

// Recompiles and reloads a given source file.
// In fact, the given file neededn't have already
// been loaded. Compiles into the SYSTEM, appending
// and using its scope (not map local).
// Errors in compilation go to message box, errors
// in running are handled as usual (err).
void runReload(char *fname)
{
    int image = CIMAGE_SYSTEM;
    char namebuf[BUFSIZ];

	if(releasemode) {
		err("Can't reload code in release mode!");
	}

    if(!strcmp(fname,"(Current Map VC)")) {
        char *s = stripext(mapname);
        sprintf(namebuf, "%s.vc", s);
        if (Exist(namebuf)) {
            fname = namebuf;
            image = CIMAGE_MAP;
        } else {
            return; // no map vc
        }
    }

	// compile it into the appropriate image, appending
	// we permit duplicate functions between system image
	// and the compiled code for non-map code, since we're
	// about to reload and patch that. For map code, we don't
	// want to permit dups betwen system and map.
	if(!vcc->CompileOther(fname, image, true, (image == CIMAGE_SYSTEM), std::vector<int>())) {
		showMessageBox(vcc->errmsg);
		return;
	}

	char *fname_compiled = va("%s.out", fname);
	VFILE *fin = vopen(fname_compiled);
	if(!fin) err("RunString: Couldn't open temp compiled code file.");

	// load it, replacing old versions of the same functions
	vc->LoadCore(fin, image, true, true);

	vclose(fin);
	remove(fname_compiled);
}


// reloads the current map by recompiling the
// map (if there's a vc file) and then getting
// out of the current loop (like a call to
// vc_Map() would do, but without switching maps
// first). This triggers a map reload.
void reloadMap()
{
	if(releasemode) {
		err("Can't reload map in release mode!");
	}

	// check to make sure we've got a map to reload!
	if(!current_map)
		return;

	char *s = stripext(mapname);
	if (Exist(va("%s.vc", s)))
		if(!vcc->CompileMap(s)) {
			showMessageBox(vcc->errmsg);
			return;
		}

	// get out of the current loop to force
	// a map reloading
	die = 1;
	done = 1;
}

#endif
