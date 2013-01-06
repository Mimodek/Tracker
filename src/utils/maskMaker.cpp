/*
 *  maskMaker.cpp
 *  fachada_tracking
 *
 *  Created by Chris on 2/12/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "maskMaker.h"

MaskMaker::MaskMaker()
{
	PolyEditable::PolyEditable();
	drawOffset.set(0,0);
	bSetMask = false;
}

void MaskMaker::setup(int w, int h, int xOff, int yOff, float scl)
{
	mask.allocate(w, h);
	mask.set(0);

	PolyEditable::setup();
	enable();
	setMode(POLY_EDIT_MODE_ADD_PTS);
	drawOffset.set(xOff,yOff);

	scale = scl;
}

void MaskMaker::clear()
{
	PolyEditable::clear();
	mask.set(0);
	bSetMask = false;

}

void MaskMaker::draw()
{

	glPushMatrix();
		//glScalef(1.f/scale, 1.f/scale, 1);
		PolyEditable::draw();
	glPopMatrix();

}

void MaskMaker::makeMask(bool bInverted)
{
	if(pts.size() < 3 ) return;


	if( bInverted) mask.set(255);
	else mask.set(0);

	vector <ofPoint> scaledpts;
	for( int i = 0; i < pts.size(); i++)
	{
		scaledpts.push_back( ofPoint( scale*(pts[i].x-drawOffset.x), scale*(pts[i].y-drawOffset.y) ) );
	}

	//moveAllPointsBy( ofPoint(-drawOffset.x,-drawOffset.y) );

	if( bInverted) mask.drawPolyIntoMe(scaledpts,0);
	else mask.drawPolyIntoMe(scaledpts,255);

	bSetMask = true;
	//cout << pts.size() << endl;

	//moveAllPointsBy( drawOffset);
}

void  MaskMaker::loadMask(string filename)
{
	clear();

	ofxXmlSettings xml;
	xml.loadFile(filename);
	xml.pushTag("mask");

	int totalpts = xml.getNumTags("pt");

	for( int i = 0; i < totalpts; i++)
	{
		xml.pushTag("pt",i);
			float x = xml.getValue("x",0.f);
			float y = xml.getValue("y",0.f);
			addPoint(ofPoint(x,y));
		xml.popTag();
	}

	xml.popTag();

}

void  MaskMaker::saveMask(string filename)
{
	ofxXmlSettings xml;

	xml.addTag("mask");
	xml.pushTag("mask");
	for( int i = 0; i < pts.size(); i++)
	{
		xml.addTag("pt");
		xml.pushTag("pt",i);
			xml.setValue("x",		pts[i].x);
			xml.setValue("y",		pts[i].y);
		xml.popTag();
	}

	xml.popTag();

	xml.saveFile(filename);

}
