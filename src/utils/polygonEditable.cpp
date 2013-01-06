/*
 *  polygonEditable.cpp
 *  fachada_tracking
 *
 *  Created by Chris on 2/12/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polygonEditable.h"


PolyEditable::PolyEditable()
{

	nPts		= 0;
	bUseContraints = false;
	bEnabled	= false;
	bUseKeyPress = true;
	bUseRotation = true;
	bDrawnWithPoints = true;
	mode		= POLY_EDIT_MODE_NONE;
	selectedPoint = -1;
	selectDist = 4;
	bUseClosePoly = true;
	lastMouse.set(0,0);
	moveOffSet.set(0,0);
	rotation = 0;
	uId = ofRandom( 200000,1000000);
}

PolyEditable::~PolyEditable()
{
}

void PolyEditable::setup()
{
	ofAddListener(ofEvents.mousePressed, this, &PolyEditable::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &PolyEditable::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &PolyEditable::mouseDragged);
	//ofAddListener(ofEvents.keyPressed, this, &PolyEditable::keyPressed);
	//ofAddListener(ofEvents.keyReleased, this, &PolyEditable::keyReleased);

}

void PolyEditable::setConstraints(ofRectangle rect)
{
	constraintRect = rect;
	bUseContraints = true;
}

//--------------------------------------------------------------
//
//
//
//--------------------------------------------------------------
void PolyEditable::pushVertex( ofPoint pt )
{
	pts.push_back(pt);
}

void PolyEditable::popVertex( ofPoint pt )
{
	if(pts.size() > 0)
		pts.pop_back();
}

void PolyEditable::insertVertex( ofPoint pt, int ptBefore )
{
	if( pts.size() > ptBefore)
		pts.insert(pts.begin()+ptBefore,pt);
}

void PolyEditable::clear()
{
	pts.clear();
	nPts = 0;
	bClosed = false;
	mode = POLY_EDIT_MODE_ADD_PTS;
}
void PolyEditable::addPoint( ofPoint pt )
{


	if( bUseContraints )
	{
		int maxX = constraintRect.x +  constraintRect.width;
		int maxY = constraintRect.y +  constraintRect.height;
		int minX = constraintRect.x;
		int minY = constraintRect.y;
		if( pt.x > maxX || pt.x < minX || pt.y > maxY || pt.y < minY)
			return;

	}

	if( bUseClosePoly && pts.size() > 2)
	{
		if( abs( (int)(pts[0].x-pt.x) ) <= selectDist && abs( (int)(pts[0].y-pt.y) ) <= selectDist )
		{
			bClosed = true;
		}

	}

	if(!bClosed) pushVertex( pt );
	if(!bClosed) cout << "add point " << endl;

}

void PolyEditable::nextMode()
{
	mode++;
	mode %= POLY_TOTAL_MODES;

	if( mode == POLY_EDIT_MODE_MOVE_PTS )
	{
		selectedPoint = pts.size()-1;
	}
}
//--------------------------------------------------------------
//
//	Events
//
//--------------------------------------------------------------

void PolyEditable::keyPressed(ofKeyEventArgs& event)
{
	if( !bEnabled || !bUseKeyPress ) return;

	if( event.key == ' ')	nextMode();

	// Note:; may not work cross-platform
	if(event.key == 9 )
	{
		if(  mode == POLY_EDIT_MODE_MOVE_PTS )
		{
			selectedPoint++;
			selectedPoint %= pts.size();
		}

	}

	if( event.key == OF_KEY_UP )
	{
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(0,-1) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(0,-1) );
	}
	else if( event.key == OF_KEY_DOWN)
	{
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(0,1) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(0,1) );

	}
	else if( event.key == OF_KEY_LEFT )
	{
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(-1,0) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(-1,0) );

	}
	else if( event.key == OF_KEY_RIGHT )
	{
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(1,0) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(1,0) );

	}
}

void PolyEditable::keyReleased(ofKeyEventArgs& event)
{
	if( !bEnabled ) return;

}

void PolyEditable::mouseMoved(ofMouseEventArgs& event)
{
	lastMouse.set( event.x, event.y );


	if( !bEnabled ) return;

}


void PolyEditable::mouseDragged(ofMouseEventArgs& event)
{
	if( !bEnabled ) return;

	if( mode == POLY_EDIT_MODE_MOVE_PTS )
	{
		if(selectedPoint >= 0)
			pts[ selectedPoint ].set( event.x, event.y);

	}else if( mode == POLY_EDIT_MODE_MOVE_ALL && event.button == 0)
	{

		ofPoint diff = ofPoint( (event.x-lastCenter.x)-moveOffSet.x,(event.y-lastCenter.y)-moveOffSet.y);

		moveAllPointsBy( diff );

		lastCenter = getCentroid();
	}else if( mode == POLY_EDIT_MODE_MOVE_ALL)
	{
		rotation += .5*(event.y-lastMouse.y);
	}

	lastMouse.set( event.x, event.y );

}


void PolyEditable::mousePressed(ofMouseEventArgs& event)
{

	cout << uId << " enabled " << bEnabled << " mode " << mode << endl;
	if( !bEnabled ) return;

	lastMouse.set( event.x, event.y );

	if( event.button != 0 ) mode = POLY_EDIT_MODE_MOVE_PTS;

	if( mode == POLY_EDIT_MODE_ADD_PTS )
	{

		//--- add point here
		addPoint( ofPoint(event.x, event.y) );

	}else if( mode == POLY_EDIT_MODE_MOVE_PTS)
	{
		selectedPoint = -1;

		//--- select points, chooses first closest (must use tab to go next point for close points)
		for( int i = 0; i < pts.size(); i++)
		{
			if( abs( (int)(event.x-pts[i].x) ) < selectDist && abs( (int)(event.y-pts[i].y) ) < selectDist )
			{
				selectedPoint = i;
				break;
			}
		}


	}else if( mode == POLY_EDIT_MODE_MOVE_ALL )
	{
		lastCenter = getCentroid();
		moveOffSet.set(event.x-lastCenter.x,event.y-lastCenter.y);
	}


}


void PolyEditable::mouseReleased(ofMouseEventArgs& event)
{

	if( !bEnabled ) return;
	lastMouse.set( event.x, event.y );
	moveOffSet.set(0,0);
}


void PolyEditable::draw()
{
	//cout << "draw poly editable " << endl;

	ofNoFill();

	ofBeginShape();
	for( int i = 0; i < pts.size(); i++)
	{
		ofVertex(pts[i].x,pts[i].y);
	}
	ofEndShape(bClosed);

	if( bDrawnWithPoints )
	{
		// NOTE: change to points not circles (this is slow)
		for( int i = 0; i < pts.size(); i++)
		{
			ofCircle(pts[i].x,pts[i].y,2);
		}
	}


	if( mode == POLY_EDIT_MODE_MOVE_PTS )
	{
		//-- draw selected point
		if( selectedPoint >= 0 )
		{
			ofNoFill();
			ofCircle(pts[selectedPoint].x, pts[selectedPoint].y, 4);
		}

	}else if( mode == POLY_EDIT_MODE_MOVE_ALL)
	{
		ofRectangle boundingBox = getBoundingBox();
		ofRect( boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
	}


}
//--------------------------------------------------------------
//
//	Editing points
//
//--------------------------------------------------------------

void PolyEditable::moveAllPointsBy( ofPoint pt )
{
	for( int i = 0; i < pts.size(); i++)
	{
		pts[i].x += pt.x;
		pts[i].y += pt.y;
	}
}

void PolyEditable::movePointBy( int index, ofPoint pt)
{
	if( index < pts.size() && index >= 0 )
	{
		pts[index].x += pt.x;
		pts[index].y += pt.y;
	}
}

ofPoint PolyEditable::getCentroid()
{
	float cx=0;
	float cy=0;
	float A =getSignedArea();

	ofPoint res;
	int j = 0;
    int N = pts.size();

	float factor=0;
	for (int i=0; i<N; i++)
	{
		j = (i + 1) % N;
		factor=(pts[i].x*pts[j].y-pts[j].x*pts[i].y);
		cx+=(pts[i].x+pts[j].x)*factor;
		cy+=(pts[i].y+pts[j].y)*factor;
	}

	A*=6.0f;
	factor=1.f/A;
	cx*=factor;
	cy*=factor;
	res.x=cx;
	res.y=cy;

	return res;
}

ofRectangle PolyEditable::getBoundingBox()
{
	ofPoint min = pts[0];
    ofPoint max = pts[0];

    for( int i = 1; i < pts.size(); i++)
    {
        if(pts[i].x<min.x) min.x = pts[i].x;
        if(pts[i].y<min.y) min.y = pts[i].y;
        if(pts[i].x>max.x) max.x = pts[i].x;
        if(pts[i].y>max.y) max.y = pts[i].y;
    }

    return ofRectangle( min.x, min.y, max.x-min.x, max.y-min.y);

}


float PolyEditable::getArea()
{
	double area = 0;

	int n = pts.size();

	for (int i=0; i < n; i++)
	{
		int j = (i + 1) % n;
		area += pts[i].x * pts[j].y;
		area -= pts[i].y * pts[j].x;
	}

	area /= 2.f;
	return (area < 0 ? -area : area);
}

float PolyEditable::getSignedArea()
{
    double area = 0;

	int n = pts.size();

	for (int i=0; i < n; i++)
	{
		int j = (i + 1) % n;
		area += pts[i].x * pts[j].y;
		area -= pts[i].y * pts[j].x;
	}

	area /= 2.f;
	return area;
}
