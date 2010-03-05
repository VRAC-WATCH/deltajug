#
#	Makefile
#  deltaJugApp
#
#	Created by Christian Noon on 2/15/10.
#	Copyright 2010 Iowa State University. All rights reserved.
#

# Set the shell to sh
SHELL = /bin/sh

# Specify the objects to be generated from source files
objects = 	main.o \
				DeltaJugApp.o \
				JugglerAppViewer.o

# Specify the application binary
application = deltaJugApp

# Set the source directory
src_dir = ../src

# Setup the targets
all: $(objects) $(application)

# Setup the VR Juggler flags
DZR_BASE_DIR = $(shell flagpoll doozer --get-prefix)
include $(DZR_BASE_DIR)/ext/vrjuggler/dzr.vrjuggler.mk
include $(DZR_BASE_DIR)/ext/vrjuggler/dzr.vrjuggler.glapp.mk

# Setup the include and lib flags
EXTRA_INCLUDES += -I$(DELTAJUG_HOME)/include -I$(OSG_HOME)/include -I$(ODE_HOME)/include $(VRJ_CXX_FLAGS) -I$(src_dir)
EXTRA_LIBS += -L$(DELTAJUG_HOME)/lib64 -ldtABC -ldtCore -ldtUtil -L$(OSG_HOME)/lib64 -losg -losgDB -losgUtil -lOpenThreads -losgViewer -L$(ODE_HOME)/lib -lode

# Generate all the object files for all the source files
$(objects): %.o: $(src_dir)/%.cpp
	g++ $(EXTRA_INCLUDES) -c $< -o $@

# Link the application together
$(application): $(objects)
	g++ $(objects) $(EXTRA_LIBS) $(LIBS) -o $@

# Cleanup
clean:
	rm -f *.o *core core.*
	rm -rf ./ii_files
	rm -f $(application)
