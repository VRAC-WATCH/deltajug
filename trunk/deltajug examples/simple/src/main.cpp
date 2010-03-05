/*
 *  main.cpp
 *  deltaJugApp
 *
 *  Created by Christian Noon on 2/15/10.
 *  Copyright 2010 Iowa State University. All rights reserved.
 *
 */

#include <cstdlib>
#include "JugglerAppViewer.h"
#include <vrj/Kernel/Kernel.h>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	// Get the kernel
	vrj::Kernel* kernel = vrj::Kernel::instance();
	
	// Instantiate an instance of the app
	JugglerAppViewer* application = new JugglerAppViewer(kernel, argc, argv);
	
	// Find the master and slave nodes
	// Create options for the application
	po::options_description vrj_test_desc("vrjTest Options");
	po::options_description& general_desc = kernel->getGeneralOptions();
	po::options_description& cluster_desc = kernel->getClusterOptions();
	vrj_test_desc.add(general_desc).add(cluster_desc);

	vrj_test_desc.add_options()
	("jconf", po::value< std::vector<std::string> >()->composing(), "VR Juggler Configuration File");

	// jconf files can be given as position arguments.
	po::positional_options_description pos_desc;
	pos_desc.add("jconf", -1);

	// Construct a parser and do the actual parsing.
	po::command_line_parser parser(argc, argv);
	po::parsed_options parsed = 
					parser.options(vrj_test_desc).positional(pos_desc).allow_unregistered().run();

	// Finally store our options and use them.
	po::variables_map vm;
	po::store(parsed, vm);
	po::notify(vm);

	if(vm.count("help"))
	{
		std::cout << vrj_test_desc << std::endl;
		return 0;
	}

	// Tell our application if we're a cluster slave and/or master
   application->setIsMaster(vm["vrjmaster"].as<bool>());
   application->setIsSlave(vm["vrjslave"].as<bool>());

	if ( argc <= 1 )
	{
		// display some usage info (holding the user by the hand stuff)
		//  this will probably go away once the kernel becomes separate
		//  and can load application plugins.
		std::cout<<"\n"<<std::flush;
		std::cout<<"\n"<<std::flush;

		std::cout << "Usage: " << argv[0]
		<< " vjconfigfile[0] vjconfigfile[1] ... vjconfigfile[n]\n"
		<< std::endl << std::endl;

		std::exit(EXIT_FAILURE);
	}

	kernel->init(argc, argv);

	// Load any config files specified on the command line
	for ( int i = 1; i < argc; ++i )
	{
   		std::cout << "Loading config file named " << argv[i];
		kernel->loadConfigFile(argv[i]);
	}

	kernel->start();

	kernel->setApplication(application);
	kernel->waitForKernelStop();

	delete application;

	return EXIT_SUCCESS;
}
