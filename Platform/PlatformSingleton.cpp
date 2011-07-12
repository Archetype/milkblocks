/*
 *  PlatformSingleton.cpp
 *  MilkBlocks
 *
 *  Created by Robert Manson on 10/28/10.
 *  Copyright 2010 Archetype Inc. All rights reserved.
 *
 */

#include "PlatformSingleton.h"

// Objective c++ will not instantiate class members
static PlatformIPhone *_instance = NULL;


PlatformIPhone * PlatformSingleton::getInstance()
{
	if (_instance == NULL)
	{
		_instance = new PlatformIPhone();
	}
	
	return _instance;
}
