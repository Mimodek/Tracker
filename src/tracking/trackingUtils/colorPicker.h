/*
 *  colorPicker.h
 *  fachada_tracking
 *
 *  Created by Chris on 2/19/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#define R_FAMILY_L	0
#define R_FAMILY_H	60
#define G_FAMILY_L	61
#define G_FAMILY_H	100
#define B_FAMILY_L	101
#define B_FAMILY_H	180

class ColorPicker{
	
public:
	ColorPicker(){};
	~ColorPicker(){};
	
	void pickColorSimple( int hue );
};
