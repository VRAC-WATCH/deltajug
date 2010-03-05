/*
 *  DeltaJugApp.cpp
 *  deltaJugApp
 *
 *  Created by Christian Noon on 2/15/10.
 *  Copyright 2010 Iowa State University. All rights reserved.
 *
 */

#include "DeltaJugApp.h"

DeltaJugApp::DeltaJugApp(const std::string& configFilename, dtCore::DeltaWin* win)
   : dtABC::Application(configFilename, win)
{
	;
}

void DeltaJugApp::InitScene()
{
   // Create the base objects
   _navTrans = new dtCore::Transformable;
   _orientationTrans = new dtCore::Transformable;
	_terrainObject = new dtCore::Object("Terrain");

   // Add the children appropriately
   _navTrans->AddChild(_orientationTrans.get());
   _orientationTrans->AddChild(_terrainObject.get());

	// Load the terrain model file
	_terrainObject->LoadFile("models/terrain_simple.ive");

	// Reposition the terrain object using the orientation transformable
	float scaleFactor = 50.0;
	osg::Matrix scaleMat = osg::Matrix::scale(osg::Vec3(scaleFactor,
																		 scaleFactor,
																		 scaleFactor));
	osg::Matrix rotMat = osg::Matrix::rotate(osg::Quat(osg::DegreesToRadians(-90.0),
                                            				 osg::Vec3(1,0,0)));
	osg::Matrix transMat = osg::Matrix::translate(osg::Vec3(0,-300, 0));
	_orientationTrans->SetMatrix(scaleMat * rotMat * transMat);

	// Add the nav transformable to the scenegraph
	AddDrawable(_navTrans.get());
}

void DeltaJugApp::UpdateNavTrans(osg::Matrix matrix)
{
	_navTrans->SetMatrix(matrix);
}
