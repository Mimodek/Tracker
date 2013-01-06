/*
 *  polygonEditable.h
 *  fachada_tracking
 *
 *  Created by Chris on 2/12/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "ofEvents.h"

#define POLY_EDIT_MODE_ADD_PTS	0
#define POLY_EDIT_MODE_MOVE_PTS 1
#define POLY_EDIT_MODE_MOVE_ALL 2
#define POLY_EDIT_MODE_ROTATE	3
#define POLY_EDIT_MODE_NONE		4

#define POLY_TOTAL_MODES		3

/*
 TO DO: 
 - add rotation option
 - copy constructor
 */

#pragma once
#include "ofMain.h"
#include "ofxVectorMath.h"

class PolyEditable{
	
public:
	
	PolyEditable();
	~PolyEditable();
	
	void setup();
	
	void pushVertex( ofPoint pt );
	void popVertex( ofPoint pt );
	void insertVertex( ofPoint pt, int ptBefore );
	void clear();
	
	void addPoint( ofPoint pt );
	void nextMode();
	void setMode( int md ){ mode = md; }
	void moveAllPointsBy( ofPoint pt );
	void movePointBy( int index, ofPoint pt);
	
	void setConstraints( ofRectangle rect );
	
	ofPoint			getCentroid();
	ofRectangle		getBoundingBox();
	float			getArea();
	float			getSignedArea();
	
	void draw();
	
	void keyPressed(ofKeyEventArgs& event);
	void keyReleased(ofKeyEventArgs& event);
	void mouseMoved(ofMouseEventArgs& event);
	void mouseDragged(ofMouseEventArgs& event);
	void mousePressed(ofMouseEventArgs& event);
	void mouseReleased(ofMouseEventArgs& event);
	
	void enable(){ bEnabled = true; }
	void disable(){ bEnabled = false; }
	
	int uId;
	
	int mode;		// current editing mode
	bool bUseContraints;
	bool bEnabled;	// enable or disable events
	bool bUseClosePoly;	// if true this will automatically close polygon if click close to start point
	bool bUseKeyPress;
	bool bUseRotation;
	
	// for editing
	int selectedPoint;
	ofRectangle constraintRect;
	// params
	float selectDist;	// maximum distance from a point/node for it to be selected
	ofPoint lastMouse, lastCenter, moveOffSet;
	float rotation; // this is used to perform rendring rotations. should make func to apply actual transformation from this
	
	vector<ofPoint>		pts;				
	int                 nPts;
	bool				bDrawnWithPoints, bClosed;
	
};


