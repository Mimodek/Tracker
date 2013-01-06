/*
 *  colorPicker.cpp
 *  fachada_tracking
 *
 *  Created by Chris on 2/19/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "colorPicker.h"

void ColorPicker::pickColorSimple( int hue )
{
	//cout <<  hue << endl;

	if( hue > R_FAMILY_L && hue < R_FAMILY_H )
	{
		//cout << "red family " << hue << endl;
	}
	else if( hue > G_FAMILY_L && hue < G_FAMILY_H )
	{
		//cout << "green family " << hue << endl;
	}else if( hue > B_FAMILY_L && hue < B_FAMILY_H )
	{
		//cout << "blue family " << hue << endl;
	}
}

