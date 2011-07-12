/*
 *  PlatformSingleton.h
 *  MilkBlocks
 *
 *  Created by Robert Manson on 10/28/10.
 *  Copyright 2010 Archetype Inc. All rights reserved.
 *
 */

#include "Platform/PlatformIphone.h"

class PlatformSingleton
{
private:
	
	PlatformSingleton() {}
	~PlatformSingleton() {}
	
	PlatformSingleton(const PlatformSingleton &);             // intentionally undefined
	PlatformSingleton & operator=(const PlatformSingleton &); // intentionally undefined
public:
	
	static PlatformIPhone * getInstance();
};


