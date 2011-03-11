#include <dtDIS/plugins/default/defaultplugin.h>
#include <DIS/EntityStatePdu.h>
#include <DIS/IncomingMessage.h>
#include <dtDIS/outgoingmessage.h>
#include <dtDIS/detonationmessage.h>
#include <dtDIS/firemessage.h>

#include <dtGame/gamemanager.h>

#include <vector>
#include <dtCore/refptr.h>

#include <dtDAL/actortype.h>
#include <DIS/PDUType.h>

#include <cstddef>
#include <dtDIS/plugins/default/detonationpduprocessor.h>
#include <dtDIS/plugins/default/firepduprocessor.h>
#include <dtDIS/plugins/default/espduprocessor.h>  
#include <dtDIS/plugins/default/createentityprocessor.h>   
#include <dtDIS/plugins/default/removeentityprocessor.h>   
#include <dtDIS/plugins/default/actorupdatetoentitystate.h>
#include <dtDIS/plugins/default/detonationtopdu.h>
#include <dtDIS/plugins/default/firetopdu.h>

using namespace dtDIS;

DefaultPlugin::DefaultPlugin()
   : mDetonationProcessor( NULL)
   , mFireProcessor( NULL )
   , mESProcessor( NULL )
   , mCreateProcessor( NULL )
   , mRemoveProcessor( NULL )
   , mSendingAdapter( NULL )
   , mDetonationSendingAdapter(NULL)
   , mFireSendingAdapter(NULL)
   , mConfig(NULL)
{
}

DefaultPlugin::~DefaultPlugin()
{
}

void DefaultPlugin::Start(DIS::IncomingMessage& imsg,
                                 dtDIS::OutgoingMessage& omsg,
                                 dtGame::GameManager* gm,
                                 dtDIS::SharedState* config)
{
   mDetonationProcessor = new DetonationPduProcessor(gm, config);
   mFireProcessor = new FirePduProcessor(gm, config);
   mESProcessor = new ESPduProcessor(gm, config);
   mCreateProcessor = new CreateEntityProcessor( &omsg, config);
   mRemoveProcessor = new RemoveEntityProcessor( &omsg, config);
   mSendingAdapter = new ActorUpdateToEntityState( config, gm );
   mDetonationSendingAdapter = new DetonationToPdu(config, gm);
   mFireSendingAdapter = new FireToPdu(config, gm);

   imsg.AddProcessor( DIS::PDU_DETONATION,   mDetonationProcessor);
   imsg.AddProcessor( DIS::PDU_FIRE,         mFireProcessor);
   imsg.AddProcessor( DIS::PDU_ENTITY_STATE, mESProcessor );
   imsg.AddProcessor( DIS::PDU_CREATE_ENTITY, mCreateProcessor );
   imsg.AddProcessor( DIS::PDU_REMOVE_ENTITY, mRemoveProcessor );
   omsg.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_UPDATED , mSendingAdapter );
   omsg.AddAdaptor(&dtDIS::DetonationMessageType::DETONATION, mDetonationSendingAdapter);
   omsg.AddAdaptor(&dtDIS::FireMessageType::FIRE, mFireSendingAdapter);

   // use the current state of the game.
   typedef std::vector<dtGame::GameActorProxy*> ProxyVector;
   ProxyVector actors;
   gm->GetAllGameActors( actors );

   ProxyVector::iterator iter = actors.begin();
   ProxyVector::iterator enditer = actors.end();
   while( iter != enditer )
   {
      this->OnActorAdded((*iter));
      ++iter;
   }
}

void DefaultPlugin::Finish(DIS::IncomingMessage& imsg, dtDIS::OutgoingMessage& omsg)
{
   imsg.RemoveProcessor( DIS::PDU_DETONATION,    mDetonationProcessor);
   imsg.RemoveProcessor( DIS::PDU_FIRE,          mFireProcessor );
   imsg.RemoveProcessor( DIS::PDU_ENTITY_STATE,  mESProcessor );
   imsg.RemoveProcessor( DIS::PDU_CREATE_ENTITY, mCreateProcessor );
   imsg.RemoveProcessor( DIS::PDU_REMOVE_ENTITY, mRemoveProcessor );
   omsg.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_UPDATED , mSendingAdapter );
   omsg.RemoveAdaptor(&dtDIS::DetonationMessageType::DETONATION, mDetonationSendingAdapter);
   omsg.RemoveAdaptor(&dtDIS::FireMessageType::FIRE, mFireSendingAdapter);
   
   delete mDetonationProcessor;
   delete mFireProcessor;
   delete mESProcessor;
   delete mCreateProcessor;
   delete mRemoveProcessor;
   delete mSendingAdapter;
   delete mDetonationSendingAdapter;
   delete mFireSendingAdapter;
}

void DefaultPlugin::OnActorAdded(dtGame::GameActorProxy* actor)
{
}

