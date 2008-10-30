/*
 *  mac_cocoa_util.h
 *  verge3
 * Header for mac_cocoa_util.mm. Not used by other files.
 *
 */

#include <Cocoa/Cocoa.h>

/* Use this flag to determine if we use Resources/verge as the CWD 
 or if we use the app's instead instead */
#define     MAC_USE_VERGE_RES_DIR   0


void sdl_toggleFullscreen();

@interface MacCocoaUtil : NSObject {
IBOutlet NSPopUpButton *fileSelector;
IBOutlet NSWindow *window;
IBOutlet NSTextField *evalField;
}
-(void)addFile:(NSString *)name;
-(void)reloadFile:(NSString *)path;
-(IBAction)reload:(id)sender;
-(IBAction)reloadMap:(id)sender;
-(IBAction)eval:(id)sender;
-(void)showWindow;
- (void)windowDidBecomeMain:(NSNotification *)aNotification;
- (void)windowDidResignMain:(NSNotification *)aNotification;
- (void)toggleFullscreen;
@end
