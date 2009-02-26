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
	CGImageRef bitmap = CGImageCreate(320,480,8,32,320*4,colorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little, bitmapProvider, NULL, FALSE, kCGRenderingIntentDefault); 
    
    
    theLayer.contents = bitmap;
    
    
    //cleanup
    CGImageRelease(bitmap);
    CGDataProviderRelease(bitmapProvider);
    CGColorSpaceRelease(colorSpace);
}



-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	view = self;
	layer = [self layer];
	layer.opaque = YES;
	//layer.delegate = drawDelegate;
	return self;
}

// Stop animating and release resources when they are no longer needed.
- (void)dealloc
{
	[super dealloc];
}

@end
