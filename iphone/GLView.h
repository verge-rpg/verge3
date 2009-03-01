//
//  GLView.h
//  NeHe Lesson 02
//
//  Created by Jeff LaMarche on 12/11/08.
//  Copyright Jeff LaMarche Consulting 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#include "iphone_interface.h"

@interface GLView : UIView
{
@public
	CALayer* layer;
	unsigned int* data;
}

- (void)displayLayer:(CALayer *)theLayer;
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent*)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent*)event;
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;


@end

