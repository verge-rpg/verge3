
#import <Cocoa/Cocoa.h>
#import "SDLMain.h"

#include "mac_cocoa_util.h"

// include C parts we can use directly
extern "C++" {
#include "SDL.h"
#include "a_string.h"
#include "g_editcode.h"
}

// Need the following declarations so that we don't have to
// include xerxes.h, which conflics with Cocoa
class image;
int HandleForImage(image *img);
image *xLoadImage(const char *fname);

// this value is set when we want to ignore
// all incoming events to verge (if we're focused
// in a window other than the SDL window)
extern bool IgnoreEvents;

// called to initialize editing code
// Shows the edit-code window.
// Doesn't check if code editing is possible
// accoring to current config.
void InitEditCode() {
	// retrieve SDLMain object from the NSApp
	// then get the reloadController outlet
	// (These are both hooked up in the nib file)
	SDLMain *sdlmain = [NSApp delegate];
	[sdlmain->macCocoaUtil showWindow];
}

// called to add a file to the pop-up
void AddSourceFile(string s) {
	// retrieve SDLMain object from the NSApp
	// then get the reloadController outlet
	// (These are both hooked up in the nib file)
    SDLMain *sdlmain = [NSApp delegate];
    [sdlmain->macCocoaUtil addFile:[NSString stringWithCString:s.c_str()]];
}

// call to show an alert panel. 
// Other parts of verge should call
// showMessageBox instead, which handles
// details like what to do for full-screen.
void doMessageBox(string message)
{
	NSRunAlertPanel(@"Verge Message:",[NSString stringWithCString: message.c_str()],@"OK",NULL,NULL);
}

// call to retrieve a url's text
// Returns the empty string on error
string getUrlText(string inUrl) 
{
	NSURL * url = [NSURL URLWithString: [NSString stringWithCString: inUrl.c_str()]];
	NSString * contents = [NSString stringWithContentsOfURL: url];
	if(contents == nil)
		return string();
	return string([contents cString]);
}

// returns an image from a given url
// returns 0 if the URL is not reachable,
// and will signal an err() if the image
// is not loadable.
int getUrlImage(string inUrl)
{
	NSURL * url = [NSURL URLWithString: [NSString stringWithCString: inUrl.c_str()]];
	NSString * contents = [NSString stringWithContentsOfURL: url];

	if(contents == nil || [contents length] == 0)
		return 0;
		
	[contents writeToFile: @"$$urlimagetemp.$$$" atomically: NO];
	
	int toReturn = HandleForImage(xLoadImage("$$urlimagetemp.$$$"));
	
	remove("$$urlimagetemp.$$$");
	
	return toReturn;
}

// The MacCocoaUtil handles user input
// from the code-editing window, other Cocoa stuff
@implementation MacCocoaUtil

// called to reload the code in the file selector
-(IBAction)reload:(id)sender {
	runReload((char *)[[fileSelector titleOfSelectedItem] cString]);
}

// called to reload the current map
-(IBAction)reloadMap:(id)sender {
	reloadMap();
}

// called to reload a given file
// Adds it to the fileSelector if it's not
// in there yet
-(void)reloadFile:(NSString *)fname {
    [self addFile:fname];
	runReload((char *)[fname cString]);
}

// adds the given file to the pop-up list if
// it's not there yet (uses string comparison)
-(void)addFile:(NSString *)fname {
	if([[fileSelector itemTitles] indexOfObject: fname] == NSNotFound) {
		[fileSelector addItemWithTitle: fname];
	}
}

// called to eval the code in the evalField
-(IBAction)eval:(id)sender {
	runEval((char *)[[evalField stringValue] cString]);
}

// call to show the code editing window
-(void)showWindow {
	[window makeKeyAndOrderFront:self];
}

// these are called when the code editing window
// becomes/stops being the front window. They
// ensure verge ignores (and the window recieves)
// events when it is in front, and vice-versa
// Also handle the cursor showing/hiding.
// We use *Main instead of *Key so dialogs don't 
// trigger these and make the mouse go away.
- (void)windowDidBecomeMain:(NSNotification *)aNotification {
	setenv("SDL_ENABLEAPPEVENTS", "1", 1);
	IgnoreEvents = true;
	SDL_ShowCursor(SDL_ENABLE);
}

- (void)windowDidResignMain:(NSNotification *)aNotification {
	unsetenv("SDL_ENABLEAPPEVENTS");
	IgnoreEvents = false;
	SDL_ShowCursor(SDL_DISABLE);
}

- (void)toggleFullscreen {
	sdl_toggleFullscreen();
}

@end