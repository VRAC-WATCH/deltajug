/*
Dignitas Technologies, LLC

*/

#include <dtDIS/plugins/default/firepduprocessor.h>

#include <dtDIS/firemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>

#include <dtDAL/gameeventmanager.h>

using namespace dtDIS;

FirePduProcessor::FirePduProcessor(dtGame::GameManager* gm, SharedState* config)
   : mGM( gm )
   , mConfig(config)
   //, mMachineInfo( new dtGame::MachineInfo() )
{
}

FirePduProcessor::~FirePduProcessor()
{
}

void FirePduProcessor::Process(const DIS::Pdu& packet)
{
	LOG_INFO("**** FIRE PDU PROCESSOR *****");

   if (mConfig == NULL) return;
   
   const DIS::FirePdu& pdu = static_cast<const DIS::FirePdu&>(packet);
   
   DIS::EntityID firingEntityId = pdu.getFiringEntityID();
   
   // Check to make sure we are not resending an already received message
   if ((mConfig->GetApplicationID() == firingEntityId.getApplication()) &&
        (mConfig->GetSiteID() == firingEntityId.getSite()))
   {
       return;
   }
   
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
}

void FirePduProcessor::NotifyRemoteActor(const DIS::FirePdu& pdu, const dtDAL::ActorProxy& actor)
{
    dtCore::RefPtr<FireMessage> msg;
    
    mGM->GetMessageFactory().CreateMessage(FireMessageType::FIRE, msg);
    
    const DIS::Vector3Double& disLocation = pdu.getLocationInWorldCoordinates();
    osg::Vec3d location(disLocation.getX(), disLocation.getY(), disLocation.getZ());
    
    msg->SetLocation(mConfig->GetCoordinateConverter().ConvertToLocalTranslation(location));
    
    DIS::EntityID shooterEntityId = pdu.getFiringEntityID();
    const dtCore::UniqueId* shooterActorId = mConfig->GetActiveEntityControl().GetActor(shooterEntityId);
    
    if (shooterActorId)
    {
        msg->SetSendingActorId(*shooterActorId);
    }
    else
    {
        // TODO - exception
        return;
    }
    
    mGM->SendMessage(*msg);
}
