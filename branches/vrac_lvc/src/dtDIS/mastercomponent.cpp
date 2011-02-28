#include <dtDIS/mastercomponent.h>
#include <dtDIS/sharedstate.h>
#include <dtDIS/detonationmessage.h>
#include <dtDIS/firemessage.h>

#include <DIS/PDUType.h>

#include <dtUtil/functor.h>
#include <list>
#include <dtDIS/dllfinder.h>

#include <dtDIS/plugins/default/defaultplugin.h>
#include <dtActors/engineactorregistry.h>
#include <dtActors/coordinateconfigactor.h>


using namespace dtDIS;

const double MasterComponent::TIME_OUT_CHECK_INTERVAL = 3.0;
const double MasterComponent::TIME_OUT_LIMIT = 15.0;

///\todo what should set the network stream's endian type?  the SharedState's connection data?
MasterComponent::MasterComponent(SharedState* config)
   : dtGame::GMComponent("dtDIS_MasterComponent")
   , mPluginManager()
   , mConnection()
   , mIncomingMessage()
   , mOutgoingMessage(DIS::BIG, config->GetConnectionData().exercise_id )
   , mConfig( config )
   , mDefaultPlugin(new dtDIS::DefaultPlugin())
   , mTimeOutDelta(0.0)
{
   // add support for the network packets
   LoadPlugins( mConfig->GetConnectionData().plug_dir );
}

MasterComponent::~MasterComponent()
{
   delete mDefaultPlugin;

   // release the memory for the packet support plugins
   mPluginManager.UnloadAllPlugins();

   //mPluginManager.GetLoadedSignal().disconnect( this );
   //mPluginManager.GetUnloadedSignal().disconnect( this );
}

//const PluginManager& MasterComponent::GetPluginManager() const
//{
//   return mPluginManager;
//}

void MasterComponent::OnAddedToGM()
{
   const ConnectionData& connect_data = mConfig->GetConnectionData();

    // Register Detonation Message
   DetonationMessageType::RegisterMessageTypes(GetGameManager()->GetMessageFactory());
   FireMessageType::RegisterMessageTypes(GetGameManager()->GetMessageFactory());

   // add the default "plugin"
   mDefaultPlugin->Start( mIncomingMessage, mOutgoingMessage, GetGameManager(), mConfig );

   // initialize all the plugins
   typedef dtUtil::Functor<void,TYPELIST_1(PluginManager::LibraryRegistry::value_type&)> ForEachPluginFunctor;
   ForEachPluginFunctor func(this,&MasterComponent::OnPluginLoaded);
   PluginManager::LibraryRegistry& plugins = mPluginManager.GetRegistry();
   PluginManager::LibraryRegistry::iterator enditer = plugins.end();
   std::for_each( plugins.begin(), plugins.end(), func );

   // make a connection to the DIS multicast network
   mConnection.Connect( connect_data.port , connect_data.ip.c_str() );
}

void MasterComponent::OnRemovedFromGM()
{
   // add the default "plugin"
   mDefaultPlugin->Finish( mIncomingMessage, mOutgoingMessage );

   // shutdown all the plugins
   typedef dtUtil::Functor<void,TYPELIST_1(PluginManager::LibraryRegistry::value_type&)> ForEachPluginFunctor;
   ForEachPluginFunctor func(this,&MasterComponent::OnPluginUnloaded);
   PluginManager::LibraryRegistry& plugins = mPluginManager.GetRegistry();
   PluginManager::LibraryRegistry::iterator enditer = plugins.end();
   std::for_each( plugins.begin(), plugins.end(), func );

   ///\todo remove the controlled entities.
   // clear the state data
   mConfig->GetActiveEntityControl().ClearAll();

   // stop reading the port
   mConnection.Disconnect();
}

void MasterComponent::LoadPlugins(const std::string& dir)
{
   typedef std::list<std::string> StringList;
   StringList dyn_libs;

   dtDIS::details::DLLFinder filefinder;
   filefinder( dir , dyn_libs );

#ifdef WIN32
      char separator('\\');
#else
      char separator('/');
#endif

   // for each file, load the plugins
   StringList::iterator iter = dyn_libs.begin();
   StringList::iterator enditer = dyn_libs.end();
   for(; iter!=enditer; ++iter)
   {
      mPluginManager.LoadPlugin( dir + separator + *iter );
   }
}

void MasterComponent::OnPluginLoaded(PluginManager::LibraryRegistry::value_type& entry)
{
   dtGame::GameManager* gm = GetGameManager();

   // intialize the plugin
   entry.second.mCreated->Start( mIncomingMessage, mOutgoingMessage, gm, mConfig );
}

void MasterComponent::OnPluginUnloaded(PluginManager::LibraryRegistry::value_type& entry)
{
   entry.second.mCreated->Finish( mIncomingMessage, mOutgoingMessage );
}

void MasterComponent::CheckForDefunctEntities() {

	double time = this->GetGameManager()->GetSimulationTime();
	mTimeOutDelta += time;

	// Don't check every tick...
	if (mTimeOutDelta > TIME_OUT_CHECK_INTERVAL) {
		mTimeOutDelta = 0.0;

		// Check to see if there are any timeout connections
		double time = this->GetGameManager()->GetSimulationTime();
		std::vector<dtCore::UniqueId> timedOutList = mConfig->GetActorUpdateMap().GetTimedOutActors(time, TIME_OUT_LIMIT);

		if (!timedOutList.empty()) {

		   for (size_t i = 0; i < timedOutList.size(); ++i) {

			   dtDAL::ActorProxy* proxy = this->GetGameManager()->FindActorById( timedOutList[i] );
			   
			   if ( proxy ) {

 					// SB TODO - maybe see if we can tie this in with the delete call
				   mConfig->GetActorUpdateMap().Remove(timedOutList[i]);
				   
				   //Remove from DIS Entity List based on Timeout
				   const DIS::EntityID* id = mConfig->GetActiveEntityControl().GetEntity(timedOutList[i]);
				   //must copy the id before passing it in to the RemoveEntity call
				   DIS::EntityID idCopy = *id;
				   mConfig->GetActiveEntityControl().RemoveEntity(idCopy, timedOutList[i]);

				   this->GetGameManager()->DeleteActor(*proxy);
			   }
		   }
	   }
	}
}


///\todo should it handle a pause message, by not updating the incoming or outgoing network classes?
void MasterComponent::ProcessMessage(const dtGame::Message& msg)
{
   const dtGame::MessageType& mt = msg.GetMessageType();

   // Put detonations onto the wire
   if (mt == dtDIS::DetonationMessageType::DETONATION ||
       mt == dtDIS::FireMessageType::FIRE)
   {
        // The sender of the message is the actor who shot the munition that triggered the detonation
        dtDAL::ActorProxy *proxy = GetGameManager()->FindActorById(msg.GetSendingActorId());
    
        // Avoid re-issuing events that arrived from remote actors
        if (proxy->IsGameActorProxy() && !static_cast<dtGame::GameActorProxy*>(proxy)->IsRemote())
        {
            this->mOutgoingMessage.Handle(msg);
            
            //write the outgoing packets
             const DIS::DataStream& ds = mOutgoingMessage.GetData();
             const unsigned int MTU = 1500;
             if( ds.size() > MTU )
             {
                LOG_WARNING("Network buffer is bigger than LAN supports.")
             }
    
             if ( ds.size() > 0 )
             {          
                //LCR: Here is where PDU get sent            
                mConnection.Send( &(ds[0]), ds.size() );        
                //LOG_INFO("Sent PDU");
                //LCR
                mOutgoingMessage.ClearData();
             }
        }
   }

   if(mt == dtGame::MessageType::INFO_ACTOR_UPDATED)
   {
      dtDAL::ActorProxy *proxy = GetGameManager()->FindActorById( msg.GetAboutActorId() );
      
      //LCR: avoid issuing PDU's for remote actors
      if( proxy->IsGameActorProxy() && !static_cast<dtGame::GameActorProxy*>(proxy)->IsRemote() )
      {
         this->mOutgoingMessage.Handle(msg);

         //write the outgoing packets
         const DIS::DataStream& ds = mOutgoingMessage.GetData();
         const unsigned int MTU = 1500;
         if( ds.size() > MTU )
         {
            LOG_WARNING("Network buffer is bigger than LAN supports.")
         }
		 	 
         if ( ds.size() > 0 )
         {			
            //LCR: Here is where PDU get sent            
			mConnection.Send( &(ds[0]), ds.size() );        
            mOutgoingMessage.ClearData();                                                  
     	} 	        
      }
   }
   //LCR


   if( mt == dtGame::MessageType::TICK_LOCAL )
   {
	    // Remove entities that are no longer active
		this->CheckForDefunctEntities();

	  	// Process all incoming buffers
	  	std::vector<std::pair<const char*, int> >::iterator iter;
	  	for (iter = mIncomingBuffers.begin(); iter != mIncomingBuffers.end(); ++iter)
			mIncomingMessage.Process(iter->first, iter->second, DIS::BIG);
		
		// Clear out all the buffers
		mIncomingBuffers.clear();

/*
	  DELETE ME

      // read the incoming packets
      const unsigned int MTU = 1500;
      char buffer[MTU];

      bool done = false;

	  // Need to swap this to use our mIncomingBuffers vector
      while(!done)
      {
         size_t recvd(0);

          recvd = mConnection.Receive(buffer , MTU);
          
	      if (recvd != 0)
          {
             mIncomingMessage.Process( buffer , recvd , DIS::BIG );
          }
          else
          {
              done = true;
          }
      }
*/
   }
   else if (mt == dtGame::MessageType::INFO_MAP_LOADED)
   {
      //find any coordinate config actor and pass it to the SharedState
      dtActors::CoordinateConfigActorProxy* proxy(NULL);
      GetGameManager()->FindActorByType(*dtActors::EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE, proxy);
      if (proxy != NULL)
      {
         dtActors::CoordinateConfigActor* actor(NULL);
         proxy->GetActor(actor);
         if (actor != NULL)
         {
            mConfig->SetCoordinateConverter(actor->GetCoordinates());
         }
      }
   }

}

DIS::IncomingMessage& MasterComponent::GetIncomingMessage()
{
   return mIncomingMessage;
}

const DIS::IncomingMessage& MasterComponent::GetIncomingMessage() const
{
   return mIncomingMessage;
}

OutgoingMessage& MasterComponent::GetOutgoingMessage()
{
   return mOutgoingMessage;
}

const OutgoingMessage& MasterComponent::GetOutgoingMessage() const
{
   return mOutgoingMessage;
}

SharedState* MasterComponent::GetSharedState()
{
   return mConfig;
}

const SharedState* MasterComponent::GetSharedState() const
{
   return mConfig;
}

void MasterComponent::AddIncomingBuffer(const char* buffer, int size)
{
	std::pair<const char*, int> incBuffer(buffer, size);
	mIncomingBuffers.push_back(incBuffer);
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::MasterComponent::DispatchNetworkMessage(const dtGame::Message& message)
{
   mOutgoingMessage.Handle(message);
}
