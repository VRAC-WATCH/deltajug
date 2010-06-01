/*
Dignitas Technologies, LLC

*/

#include <dtDIS/plugins/default/firepduprocessor.h>

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
   if (mConfig == NULL) return;
   
   const DIS::FirePdu& pdu = static_cast<const DIS::FirePdu&>(packet);
   
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
      //looks like we received a packet that we sent.  Ignore it and move on
      if ((mConfig->GetApplicationID() == firingEntityId.getApplication()) &&
          (mConfig->GetSiteID()        == firingEntityId.getSite()))
      {
         return;
      }

      //A fire occurred but we have no knowledge of the firing entity - ignore it
   }
}


void FirePduProcessor::NotifyRemoteActor(const DIS::FirePdu& pdu, const dtDAL::ActorProxy& actor)
{   
    //Technically
    dtCore::RefPtr<dtGame::GameEventMessage> msg;
    mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT,msg);

    //strictly help with visual studio intellisense
    dtGame::GameEventMessage* gameEventMessage = (dtGame::GameEventMessage*) msg.get();

    //LCR: For now I'm assuming I can reuse the one and only "Weapon Fire Event"
    //     But I need to make sure that everyone knows about the fire event before I start to reuse it for 
    //     another incoming Fire PDU.
    //     If I am not supposed to reuse it, then who is responsible for deleting old events?
    //     LCR: TODO: Put the "Weapon Fire Event" string in a constant somewhere
    dtDAL::GameEvent *event = dtDAL::GameEventManager::GetInstance().FindEvent("Weapon Fire Event");

    if(NULL == event) {
        event = new dtDAL::GameEvent("Weapon Fire Event", "Fire event originating from DIS Network");
        dtDAL::GameEventManager::GetInstance().AddEvent( *event );       
    }

    //LCR: TODO: Find out if ALL actors get this message or if only the about actor gets it
    gameEventMessage->SetAboutActorId(actor.GetId());

    //LCR: TODO: Pack more information in the Weapon Fire Events, such as...
    //     the firing entity/actor, the munition type, etc. - probably other stuff too
    //
    gameEventMessage->SetGameEvent(*event);

   // send it
   mGM->SendMessage( *msg );
}
