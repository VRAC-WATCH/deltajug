/*
 *  JugglerAppViewer.h
 *  deltaJugApp
 *
 *  Created by Christian Noon on 2/15/10.
 *  Copyright 2010 Iowa State University. All rights reserved.
 *
 */

#ifndef _JUGGLERAPPVIEWER_H_
#define _JUGGLERAPPVIEWER_H_

#include <vrj/vrjConfig.h>
#include <vrj/Draw/OSG/AppViewer.h>
#include <vrj/Draw/OpenGL/App.h>
#include <vrj/Draw/OpenGL/ContextData.h>
#include <plugins/ApplicationDataManager/UserData.h>

#include <gmtl/Vec.h>
#include <gmtl/Coord.h>
#include <gmtl/Xforms.h>
#include <gmtl/Math.h>
#include <gmtl/Matrix.h>
#include <gmtl/Point.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/VecOps.h>
#include <gmtl/Xforms.h>
#include <gmtl/Output.h>
#include <gmtl/Generate.h>

#include <gadget/Type/AnalogInterface.h>
#include <gadget/Type/DigitalInterface.h>

#include <dtCore/globals.h>
#include <dtCore/system.h>

#include "nav.h"
#include "DeltaJugApp.h"

class JugglerAppViewer : public vrj::osg::AppViewer
{
public:
	// Constructor/Destructor
    JugglerAppViewer(vrj::Kernel* kern, int & argc, char** argv);
    ~JugglerAppViewer() {}

	// Juggler draw loop override methods
    void initScene();
    void preFrame();
    void latePreFrame();
	 void bufferPreDraw();

	// Accessors
	osg::Group* getScene();
   void setIsMaster(bool master) { _isMaster = master; }
   void setIsSlave(bool slave) { _isSlave = slave; }

private:
   // Positional interface for the HEAD position
	gadget::PositionInterface _head;

   // Gamepad digital interface
	gadget::DigitalInterface _button[10];

   // Gamepad analog interface (Logitech Rumblepad 2 gamepad)
	gadget::AnalogInterface _axis0;
	gadget::AnalogInterface _axis1;
	gadget::AnalogInterface _axis2;
	gadget::AnalogInterface _axis3;
	gadget::AnalogInterface _axis4;
	gadget::AnalogInterface _axis5;

   // Gamepad settings
   float _navSpeed;
   float _rotationSpeed;

	// Controls the gamepad navigation calculations
	OsgNavigator _osgNavigator;

   // Flag for whether this app is the master
   bool _isMaster;
   bool _isSlave;

	// Delta3D application instance
	dtCore::RefPtr<DeltaJugApp> _djApp;
};

#endif
