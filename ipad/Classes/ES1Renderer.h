//
//  ES1Renderer.h
//  Milkblocks
//
//  Created by Robert Manson on 10/28/10.
//  Copyright 2010 Archetype Inc. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer;
}

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@end
