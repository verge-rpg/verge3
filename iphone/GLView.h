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

#define MAX_SIMULTANEOUS_TOUCHES 5

@interface GLView : UIView
{
@public
	CALayer* layer;
	unsigned int* data;
}

- (void)displayLayer:(CALayer *)theLayer;


@end

