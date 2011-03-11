/*
 *  JugglerAppViewer.cpp
 *  deltaJugApp
 *
 *  Created by Christian Noon on 2/15/10.
 *  Copyright 2010 Iowa State University. All rights reserved.
 *
 */

#include "JugglerAppViewer.h"

JugglerAppViewer::JugglerAppViewer(vrj::Kernel* kern, int& argc, char** argv)
   : vrj::osg::AppViewer(kern),
   _navSpeed(0.2),
   _rotationSpeed(0.0025),
   _isMaster(false)
{
   ;
}

void JugglerAppViewer::initScene()
{
	// Setup head proxy
	_head.init("VJHead");
	
	// Setup gamepad proxies
	_button[0].init("Button 1");
	_button[1].init("Button 2");
	_button[2].init("Button 3");
	_button[3].init("Button 4");
	_button[4].init("Button 5");
	_button[5].init("Button 6");
	_button[6].init("Button 7");
	_button[7].init("Button 8");
	_button[8].init("Button 9");
	_button[9].init("Button 10");
	_axis0.init("Left Joystick Horizontal Axis");
	_axis1.init("Left Joystick Vertical Axis");
	_axis2.init("Right Joystick Vertical Axis");
	_axis3.init("Right Joystick Horizontal Axis");
	_axis4.init("DirectionPad Horizontal Axis");
	_axis5.init("DirectionPad Vertical Axis");

	//===================== SETUP THE DELTA3D APPLICATION ======================
	// Setup the data file search paths for the config file and the models files.
	// This is best done in main prior to configuring app. That way the paths
	// are ensured to be correct when loading data.
	dtCore::SetDataFilePathList( "../models");

   // Create and configure the DeltaJug application
	_djApp = new DeltaJugApp();
	_djApp->Config();
	_djApp->InitScene();

   // Initialize the DeltaJug system
	dtCore::System::GetInstance().InitVars();

   // Initialize the navigation class
   _osgNavigator.init();
   _osgNavigator.setWalkMode(false);
}

void JugglerAppViewer::preFrame()
{
   if (_isMaster || !_isMaster && !_isSlave)
   {
      // User navigation with the joysticks
      float axis0 = _axis0->getData();
      float axis1 = _axis1->getData();
      float axis2 = _axis2->getData();
      float axis3 = _axis3->getData();
      float axis4 = _axis4->getData();
      float axis5 = _axis5->getData();

      // The gamepad provides 0.0 - 1.0, this makes the origin 0.0 instead of 0.5
      axis0 -= 0.5; axis1 -= 0.5; axis2 -= 0.5; axis3 -= 0.5; axis4 -= 0.5; axis5 -= 0.5;

      if (axis0 != 0.0 || axis1 != 0.0 || axis2 != 0.0 || axis3 != 0.0 || axis4 != 0.0 || axis5 != 0.0)
      {
         // Scale the values nicely
         axis0 *= _navSpeed; axis1 *= _navSpeed; axis2 *= _navSpeed/2; axis4 /= 20; axis5 /= 20;
	
         // This ties rotation speed to navSpeed
			printf("axis3: %f\n", axis3);
         axis3 *= _rotationSpeed;

         // Only allow Yaw (rot y) for navigation
         gmtl::EulerAngleXYZf euler(0.0f, -axis3, 0.0f);
         _osgNavigator.setRotationalVelocity(gmtl::makeRot<gmtl::Matrix44f>(euler));
         _osgNavigator.setVelocity(gmtl::Vec3f(axis0, -axis2, axis1));

         // Only allow Pitch (rot x) and Roll (rot z) for tilt
         gmtl::EulerAngleXYZf eulerTilt(-axis5, 0.0f, axis4);
         _osgNavigator.setTiltVelocity(gmtl::makeRot<gmtl::Matrix44f>(eulerTilt));
      }

      // Update the navigator for both the wand and gamepad
      _osgNavigator.update();
   }
}

void JugglerAppViewer::latePreFrame()
{
	// Handle all button events
	for (int i = 0; i < 10; i++)
	{
		if (_button[i]->getData() == gadget::Digital::TOGGLE_ON)
		{
			if (i == 4 && _navSpeed > 0.0)
				_navSpeed -= 0.1;
			if (i == 5)
				_navSpeed += 0.1;
		}
	}

   // Update the navigation matrix using the osgNavigator
   // Invert the osg navigator matrix
   gmtl::Matrix44f world_transform, currentPosition;
   currentPosition = _osgNavigator.getCurPos();
   gmtl::invertFull(world_transform, currentPosition);

   // Convert the matrix into an osg matrix and apply
   // to the deltajug scenegraph
   osg::Matrixf osg_current_matrix;
   osg_current_matrix.set(world_transform.getData());
	_djApp->UpdateNavTrans(osg_current_matrix);

   // Run all the necessary DeltaJug updates
   dtCore::System::GetInstance().LatePreFrame();

	// Run the parent AppViewer latePreFrame
	vrj::osg::AppViewer::latePreFrame();
}

void JugglerAppViewer::bufferPreDraw()
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Set the clear color
	(*mContextViewer)->getCamera()->setClearColor(osg::Vec4(0.5294, 0.7059, 0.9216, 1.0));
}

osg::Group* JugglerAppViewer::getScene()
{
	return _djApp->GetRootNode();
}
