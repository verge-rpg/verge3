//
//  GLView.m
//  NeHe Lesson 02
//
//  Created by Jeff LaMarche on 12/11/08.
//  Copyright Jeff LaMarche Consulting 2008. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#include <stdlib.h>

#import "GLView.h"


static GLView* view;

@implementation GLView

const void * _CGDataProviderGetBytePointerCallback(void *info)
{
	return (void*)view->data;
}

void _CGDataProviderReleaseBytePointerCallback(void *info,const void *pointer)
{
}
   
- (void)displayLayer:(CALayer *)theLayer
{
	        
    CGDataProviderDirectCallbacks callbacks;
    callbacks.version = 0;
    callbacks.getBytePointer = _CGDataProviderGetBytePointerCallback;
    callbacks.releaseBytePointer = _CGDataProviderReleaseBytePointerCallback;
    callbacks.getBytesAtPosition = NULL;
    callbacks.releaseInfo = NULL;
    
   	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();


    
    //CGDataProviderRef bitmapProvider = CGDataProviderCreateDirect(NULL,320*480*2,&callbacks);
    //CGImageRef bitmap = CGImageCreate(320,480,5,16,320*2,colorSpace, 0, bitmapProvider, NULL, FALSE, kCGRenderingIntentDefault); 
	CGDataProviderRef bitmapProvider = CGDataProviderCreateDirect(NULL,320*480*4,&callbacks);
	CGImageRef bitmap = CGImageCreate(320,480,8,32,320*4,colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big, bitmapProvider, NULL, FALSE, kCGRenderingIntentDefault); 
    
    
    theLayer.contents = bitmap;
    
    
    //cleanup
    CGImageRelease(bitmap);
    CGDataProviderRelease(bitmapProvider);
    CGColorSpaceRelease(colorSpace);
}



-(id)initWithFrame:(CGRect)frame
{
	int i=0;
	self = [super initWithFrame:frame];
	view = self;
	layer = [self layer];
	layer.opaque = YES;
	//layer.delegate = drawDelegate;
	for(i=0;i<MAX_SIMULTANEOUS_TOUCHES;i++)
		iphone_mouses[i].data = NULL;
	return self;
}

// Stop animating and release resources when they are no longer needed.
- (void)dealloc
{
	[super dealloc];
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent*)event {
	
	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch = (UITouch*)[enumerator nextObject];
	
	int i;
	int found = 0;
	for(i=0;touch&&i<MAX_SIMULTANEOUS_TOUCHES;i++) {
		//check if this mouse is already tracking a touch
		if(iphone_mouses[i].data != NULL) continue;
		
		//mouse not associated with anything right now.
		//associate the mosue with this touch
		found = 1;
		
		CGPoint locationInView = [touch locationInView: self];
		iphone_mouses[i].data = [touch retain];
		
		//send press event
		iphone_mouses[i].l = 1;
		iphone_mouses[i].x = locationInView.x;
		iphone_mouses[i].y = locationInView.y;
		
		touch = (UITouch*)[enumerator nextObject];
		
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent*)event {
	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch = nil;
	
	while(touch = (UITouch*)[enumerator nextObject]) {
		//search for the mouse slot associated with this touch
		int i, found = NO;
		for(i=0;i<MAX_SIMULTANEOUS_TOUCHES && !found; i++) {
			if(iphone_mouses[i].data == touch) {
				//found the mouse associated with the touch
				[(UITouch*)(iphone_mouses[i].data) release];
				iphone_mouses[i].data = NULL;
				iphone_mouses[i].l = 0;
				found = YES;
			}
		}
	}
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event {
	[self touchesEnded: touches withEvent: event];
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	NSEnumerator *enumerator = [touches objectEnumerator];
	UITouch *touch = nil;
	
	while(touch = (UITouch*)[enumerator nextObject]) {
		//try to find the mouse associated with this touch
		int i, found = NO;
		for(i=0;i<MAX_SIMULTANEOUS_TOUCHES && !found; i++) {
			if(iphone_mouses[i].data == touch) {
				//found proper mouse
				CGPoint locationInView = [touch locationInView: self];
				iphone_mouses[i].x = locationInView.x;
				iphone_mouses[i].y = locationInView.y;
				found = YES;
			}
		}
	}
}

@end
