//
//  MilkblocksAppDelegate.h
//  Milkblocks
//
//  Created by Robert Manson on 10/28/10.
//  Copyright 2010 Archetype Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface MilkblocksAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EAGLView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

