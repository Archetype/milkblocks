//
//  ESRenderer.h
//  Milkblocks
//
//  Created by Robert Manson on 10/28/10.
//  Copyright 2010 Archetype Inc. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@end
