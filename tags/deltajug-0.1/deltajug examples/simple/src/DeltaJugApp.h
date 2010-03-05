/*
 *  DeltaJugApp.h
 *  deltaJugApp
 *
 *  Created by Christian Noon on 2/15/10.
 *  Copyright 2010 Iowa State University. All rights reserved.
 *
 */

#ifndef _DELTAJUGAPP_H_
#define _DELTAJUGAPP_H_

#include <dtABC/application.h>

#include <dtCore/refptr.h>
#include <dtCore/object.h>
#include <dtCore/transformable.h>

class DeltaJugApp : public dtABC::Application
{
public:
	DeltaJugApp(const std::string& configFilename = "", dtCore::DeltaWin* win = NULL);

	// Override this function to setup your scene
	void InitScene();

   // Updates the navigation transformable
	void UpdateNavTrans(osg::Matrix matrix);

protected:
   // Destructors for subclasses of dtCore::Base must have a protected
   // destructor. Otherwise use of RefPtrs will cause some serious
   // problems if the objects are allocated on the stack.
   virtual ~DeltaJugApp() {}

private:
   // dtCore::RefPtr is a template for a smart pointer that takes
   // care of reference counting for objects allocated on the heap.
   // It is good practice to store all objects that derive from
   // dtCore::Base inside a RefPtr.
   dtCore::RefPtr<dtCore::Object> _terrainObject;
   dtCore::RefPtr<dtCore::Transformable> _navTrans;
   dtCore::RefPtr<dtCore::Transformable> _orientationTrans;
};

#endif
