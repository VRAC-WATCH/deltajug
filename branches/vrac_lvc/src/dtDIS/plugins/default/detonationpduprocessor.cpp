/*
Dignitas Technologies, LLC
*/

#include <dtDIS/plugins/default/detonationpduprocessor.h>

#include <dtDIS/DetonationMessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>

#include <dtDAL/gameeventmanager.h>

using namespace dtDIS;

DetonationPduProcessor::DetonationPduProcessor(dtGame::GameManager* gm, SharedState* config)
   : mGM( gm )
   , mConfig(config)
   //, mMachineInfo( new dtGame::MachineInfo() )
{
}

DetonationPduProcessor::~DetonationPduProcessor()
{
}

void DetonationPduProcessor::Process(const DIS::Pdu& packet)
{
   if (mConfig == NULL) return;
   
   const DIS::DetonationPdu& pdu = static_cast<const DIS::DetonationPdu&>(packet);
   
   DIS::EntityID firingEntityId = pdu.getFiringEntityID();

   // find out if there is an actor for this ID
   const dtCore::UniqueId* firingActorId = mConfig->GetActiveEntityControl().GetActor(firingEntityId);

   // check to see if this class knows about it
   if (firingActorId != NULL)
   {
      // the entity is known, so update the previously created actor.
      dtDAL::ActorProxy* firingActorProxy = mGM->FindActorById(*firingActorId);
      if (firingActorProxy)
      {
         NotifyRemoteActor(pdu, *firingActorProxy);
      }     
   }
   else
   {      
      if ((mConfig->GetApplicationID() == firingEntityId.getApplication()) &&
          (mConfig->GetSiteID()        == firingEntityId.getSite()))
      {
         //looks like we received a packet that we sent.  Ignore it and move on
      }
      else {

         //A detonation occurred but we have no knowledge of the firing entity - ignore it
      }
   }
}


void DetonationPduProcessor::NotifyRemoteActor(const DIS::DetonationPdu& pdu, const dtDAL::ActorProxy& actor)
{   
#if 0
    //Technically
    dtCore::RefPtr<dtGame::GameEventMessage> msg;
    mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT,msg);

    //strictly help with visual studio intellisense
    dtGame::GameEventMessage* gameEventMessage = (dtGame::GameEventMessage*) msg.get();

    //LCR: For now I'm assuming I can reuse the one and only "Munition Detonation Event"
    //     But I need to make sure that everyone knows about the detonation event before I start to reuse it for 
    //     another incoming Detonation PDU.
    //     If I am not supposed to reuse it, then who is responsible for deleting old events?
    //     LCR: TODO: Put the "Munition Detonation Event" string in propertyname.h/.cpp
    dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent("Munition Detonation Event");

    if(NULL == event) {
        event = new dtDAL::GameEvent("Munition Detonation Event", "Detonation originating from DIS Network");
        dtDAL::GameEventManager::GetInstance().AddEvent( *event );       
    }

    //use the sending actor id to hold the actor that fired the munition
    gameEventMessage->SetSendingActorId(actor.GetId());

    //use the about actor id to hold the target actor
    DIS::EntityID targetEntityId = pdu.getTargetEntityID();
    const dtCore::UniqueId* targetActorId = mConfig->GetActiveEntityControl().GetActor(targetEntityId);
    if( targetActorId != NULL ) {        
         dtDAL::ActorProxy* targetActorProxy = mGM->FindActorById(*targetActorId);
         if (targetActorProxy) {
                gameEventMessage->SetAboutActorId(targetActorProxy->GetId());
         }
    }
    else {
        //maybe I don't know about the target?
        //maybe it didn't even hit a target
        //maybe it was indirect fire
    }

    //LCR: TODO: Pack more information in the Munition Detonation Events, such as...
    //     the firing entity/actor, the munition type, etc. - probably other stuff too
    //
    gameEventMessage->SetGameEvent(*event);
#endif
	dtCore::RefPtr<DetonationMessage> msg;
	mGM->GetMessageFactory().CreateMessage(DetonationMessageType::DETONATION, msg);
	
	const DIS::Vector3Double& disLocation = pdu.getLocationInWorldCoordinates();
	osg::Vec3d location(disLocation.getX(), disLocation.getY(), disLocation.getZ());	

	//msg->SetLocation(osg::Vec3(1010,1310,635));
	msg->SetLocation(mConfig->GetCoordinateConverter().ConvertToLocalTranslation(location));

	LOG_INFO("***** SENDING DETONATION MESSAGE ************");

   // send it
   mGM->SendMessage(*msg);
}
