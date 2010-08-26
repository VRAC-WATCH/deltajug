/*
Dignitas Technologies, LLC
*/

#include <dtDIS/plugins/default/detonationpduprocessor.h>

#include <dtDIS/detonationmessage.h>
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
   
   // Check to make sure we are not just resending an already received message
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

void DetonationPduProcessor::NotifyRemoteActor(const DIS::DetonationPdu& pdu, const dtDAL::ActorProxy& actor)
{   
	dtCore::RefPtr<DetonationMessage> msg;
	mGM->GetMessageFactory().CreateMessage(DetonationMessageType::DETONATION, msg);
	
	const DIS::Vector3Double& disLocation = pdu.getLocationInWorldCoordinates();
	osg::Vec3d location(disLocation.getX(), disLocation.getY(), disLocation.getZ());	

	//msg->SetLocation(osg::Vec3(1010,1310,635));
	msg->SetLocation(mConfig->GetCoordinateConverter().ConvertToLocalTranslation(location));

    DIS::EntityID shooterEntityId = pdu.getFiringEntityID();
    const dtCore::UniqueId* shooterActorId = mConfig->GetActiveEntityControl().GetActor(shooterEntityId);

    if (shooterActorId)
    {
        msg->SetSendingActorId(*shooterActorId);
    }
    
    DIS::EntityID targetEntityId = pdu.getTargetEntityID();
    
    const dtCore::UniqueId* targetActorId = mConfig->GetActiveEntityControl().GetActor(targetEntityId);
    
    // Check if the detonation hit somebody
    if (targetActorId)
    {
        msg->SetTargetHit(true);
        msg->SetAboutActorId(*targetActorId);
    }


   // send it
   mGM->SendMessage(*msg);
}
