
#import <UiKit/UiKit.h>

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

//#include "mac_cocoa_util.h"
#import "iphone_sys.h"

extern "C++" void runloop() {
	int result;
	do {
	result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
	} while(result == kCFRunLoopRunHandledSource);
}


// include C parts we can use directly
extern "C++" {
#include "SDL.h"
#include "a_string.h"
#include "g_editcode.h"
void HandleMessages();
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

/*void m_main({
	//(try to) hide the status bar
	[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	
	//capture the bundle path and chdir to it
	NSString *bPath = [[NSBundle mainBundle] bundlePath];
	const char* iphone_appBundlePath = [bPath cString];
	chdir(iphone_appBundlePath);
	[bPath release];
}*/


// Other parts of verge should call
// showMessageBox instead, which handles
// details like what to do for full-screen.
void doMessageBox(string message)
{
	//NSRunAlertPanel(@"Verge Message:",[NSString stringWithCString: message.c_str()],@"OK",NULL,NULL);
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Verge Message:" message:[NSString stringWithCString: message.c_str()] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	
	for(;;) {
		HandleMessages();
	}
}

// call to retrieve a url's text
// Returns the empty string on error
string getUrlText(CStringRef inUrl) 
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
int getUrlImage(CStringRef inUrl)
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

extern "C++" void iphone_c_main();


UIWindow*				window;
GLView *glView ;

void init() {
	//(try to) hide the status bar
	//[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
	
	//capture the bundle path and chdir to it
	NSString *bPath = [[NSBundle mainBundle] bundlePath];
	const char* iphone_appBundlePath = [bPath cString];
	chdir(iphone_appBundlePath);
	[bPath release];
	
	
	CGRect	rect = [[UIScreen mainScreen] bounds];
	
	window = [[UIWindow alloc] initWithFrame:rect];
	
	glView = [[GLView alloc] initWithFrame:rect]; 
	glView.opaque = YES;
	[window addSubview:glView];

	//glView.delegate = self;
	//glView.animationInterval = 1.0 / kRenderingFrequency;
	//[glView startAnimation];
	//[glView release];
	
	[window makeKeyAndVisible];
	
}



extern "C++" void iphone_m_main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	UIApplicationMain(argc, argv,@"UIApplication",@"AppDelegate"); 
	
	//init();
	
	[pool release];
}

StringRef GetSystemSaveDirectory(CStringRef name)
{
  /*NSArray *arr = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, true);
  if([arr count] == 0 || !MAC_USE_VERGE_RES_DIR)
  {
    return "./";
  }
  else
  {
	  StringRef temp = (std::string)"com.verge-rpg." + name.c_str();
    NSString *prefsDirName = [NSString stringWithUTF8String:temp.c_str()];
    
    NSString *libDir = [arr objectAtIndex:0];
    NSString *vergeDir = [[libDir stringByAppendingPathComponent:@"Preferences"] stringByAppendingPathComponent:prefsDirName];
    
    NSFileManager *manager = [NSFileManager defaultManager];
    if(![manager fileExistsAtPath:vergeDir]) 
    {
      [manager createDirectoryAtPath:vergeDir attributes:nil];
    }
    
    return std::string([vergeDir UTF8String]) + "/";
  }*/
  static const StringRef dotSlash = "./";
  return dotSlash;
}

extern "C++" void iphone_m_flip(unsigned int* data) {
	glView->data = data;
	[glView->layer setNeedsDisplay];
	runloop();
}



@implementation AppDelegate
@synthesize window;

- (void)applicationDidFinishLaunching:(UIApplication*)application
{
	init();
	iphone_c_main();
	//for(;;) {
		//[glView drawView];
		//[glView->layer setNeedsDisplay];
		//runloop();
	//}
	
}


@end