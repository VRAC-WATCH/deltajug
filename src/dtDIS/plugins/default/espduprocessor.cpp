#include <dtDIS/plugins/default/espduprocessor.h>
#include <dtDIS/plugins/default/espduapplicator.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>

#include <dtGame/actorupdatemessage.h>
#include <DIS/EntityStatePdu.h>


using namespace dtDIS;

ESPduProcessor::ESPduProcessor(dtGame::GameManager* gm, SharedState* config)
   : mGM( gm )
   , mConfig(config)
   , mMachineInfo( new dtGame::MachineInfo() )
{
}

ESPduProcessor::~ESPduProcessor()
{
}

void ESPduProcessor::Process(const DIS::Pdu& packet)
{
   if (mConfig == NULL) return;

   const DIS::EntityStatePdu& pdu = static_cast<const DIS::EntityStatePdu&>(packet);

   // find out if there is an actor for this ID
   const dtCore::UniqueId* actorID = mConfig->GetActiveEntityControl().GetActor(pdu.getEntityID());

   // check to see if this class knows about it
   if (actorID != NULL)
   {
      // the entity is known, so update the previously created actor.
      dtDAL::ActorProxy* proxy = mGM->FindActorById(*actorID);
      if (proxy)
      {
         SendPartialUpdate(pdu, *proxy);
      }

	  this->UpdateActorUpdateTime(*actorID);
   }
   else
   {
      //looks like we received a packet that we sent.  Just ignore it and move on
      if ((mConfig->GetApplicationID() == pdu.getEntityID().getApplication()) &&
          (mConfig->GetSiteID() == pdu.getEntityID().getSite()))
      {
         return;
      }

      CreateRemoteActor(pdu);
   }
}

void ESPduProcessor::SendPartialUpdate(const DIS::EntityStatePdu& pdu, const dtDAL::ActorProxy& actor)
{
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED,msg);

   // customize it for the actor
   msg->SetSendingActorId( actor.GetId() );
   msg->SetAboutActorId( actor.GetId() );
   msg->SetName( actor.GetName() );
   msg->SetActorTypeName( actor.GetActorType().GetName() );
   msg->SetActorTypeCategory( actor.GetActorType().GetCategory() );

   details::PartialApplicator apply;
   apply(pdu, *msg, mConfig);

   // send it
   mGM->SendMessage( *msg );
}

void ESPduProcessor::ApplyFullUpdateToProxy(const DIS::EntityStatePdu& pdu, dtGame::GameActorProxy& proxy)
{
   // make a message
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED,msg);

   // customize it for the actor
   msg->SetSendingActorId( proxy.GetId() );
   msg->SetAboutActorId( proxy.GetId() );
   msg->SetName( proxy.GetName() );
   msg->SetActorTypeName( proxy.GetActorType().GetName() );
   msg->SetActorTypeCategory( proxy.GetActorType().GetCategory() );

   details::FullApplicator apply;
   apply( pdu , *msg, mConfig );

   proxy.ApplyActorUpdate( *msg );
}

//void ESPduProcessor::AddActor(const DIS::EntityStatePdu& pdu, dtDAL::ActorProxy* proxy)
//{
//   dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>( proxy );
//   if( gap )
//   {
//      ApplyFullUpdateToProxy( pdu, *gap );
//      mGM->AddActor( *gap, true, false );
//   }
//   else
//   {
//      mGM->AddActor( *proxy );
//   }
//
//   // keep this actor in local memory for easy access
//   mConfig->GetActiveEntityControl().AddEntity( pdu.getEntityID(), proxy );
//}

void dtDIS::ESPduProcessor::CreateRemoteActor(const DIS::EntityStatePdu& pdu)
{
   const dtDAL::ActorType* actorType(NULL);
   dtDIS::ActorMapConfig& emapper = mConfig->GetActorMap();

   const DIS::EntityType& entityType = pdu.getEntityType();
   
   //LCR: Setup this boolean so if actor mapping doesn't exist try to use the default actor mapping
   bool actorMappingExists = emapper.GetMappedActor(entityType, actorType);

   if( !actorMappingExists ) {

        //LCR:  Use DIS Enum (0,0,0,0,0,0,0) for a default ActorMapping
        DIS::EntityType defaultType;
        defaultType.setCategory(0);
        defaultType.setCountry(0);
        defaultType.setDomain(0);
        defaultType.setEntityKind(0);
        defaultType.setExtra(0);
        defaultType.setSpecific(0);
        defaultType.setSubcategory(0);

        actorMappingExists = emapper.GetMappedActor(defaultType, actorType);

        if( actorMappingExists ) {

          //LCR:  Print here so the user knows which entity types still need mapping
          std::string entTypeStr;
          entTypeStr += dtUtil::ToString<unsigned short>(entityType.getEntityKind()) + 
                  "." + dtUtil::ToString<unsigned short>(entityType.getDomain()) +
                  "." + dtUtil::ToString<unsigned short>(entityType.getCountry()) + 
                  "." + dtUtil::ToString<unsigned short>(entityType.getCategory()) + 
                  "." + dtUtil::ToString<unsigned short>(entityType.getSubcategory()) +
                  "." + dtUtil::ToString<unsigned short>(entityType.getSpecific()) + 
                  "." + dtUtil::ToString<unsigned short>(entityType.getExtra());
          LOG_INFO("Using the default entity type -> actor mapping for entity type:  " +  entTypeStr);
        }
   }

   //if(emapper.GetMappedActor(entityType, actorType))
   if (actorMappingExists)
   //LCR
   {
      dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
      mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED, msg);

      msg->SetSource(*mMachineInfo);

      msg->SetActorTypeCategory(actorType->GetCategory());
      msg->SetActorTypeName(actorType->GetName());
    
      dtCore::UniqueId newActorID;
      msg->SetAboutActorId(newActorID);
      msg->SetName(newActorID.ToString());

        //LCR: This is how to get the name of the Mesh from the mapped resources
        //const dtDAL::ResourceDescriptor* rdPtr = NULL;   
        //mConfig->GetResourceMap().GetMappedResource(entityType, rdPtr);

        //LCR: Showing various features of the mapped resource
        //LOG_INFO("charlie say resource ext is:           " + rdPtr->GetExtension());
        //LOG_INFO("charlie say resource id is:            " + rdPtr->GetResourceIdentifier());
        //LOG_INFO("charlie say resource name is:          " + rdPtr->GetResourceName());
        //LOG_INFO("charlie says resource display name is: " + rdPtr->GetDisplayName());

        //the call to AddUpdateParameter requires the NamedStringParameter to be created on the heap
        //possible memory leak? TODO: look into this further        
        //in any case, adding this "Mesh" parameter appears to be redundant with "Non-damaged actor"
        //TODO: look into how/where "Non-damaged actor" property gets set
        //dtDAL::NamedStringParameter* myNamedStringParam = new dtDAL::NamedStringParameter("Mesh", rdPtr->GetResourceName());
        //msg->AddUpdateParameter( *myNamedStringParam );
        //LCR
      
      dtDIS::details::FullApplicator copyToMsg;
      copyToMsg(pdu, *msg, mConfig);

      mGM->SendMessage(*msg);

      //store the ID for later retrieval
      mConfig->GetActiveEntityControl().AddEntity(pdu.getEntityID(), newActorID);


	  this->UpdateActorUpdateTime(newActorID);
      //TODO SendPartialUpdate()?
   }
   else
   {
      std::string entTypeStr;
      entTypeStr += dtUtil::ToString<unsigned short>(entityType.getEntityKind()) + 
              "." + dtUtil::ToString<unsigned short>(entityType.getDomain()) +
              "." + dtUtil::ToString<unsigned short>(entityType.getCountry()) + 
              "." + dtUtil::ToString<unsigned short>(entityType.getCategory()) + 
              "." + dtUtil::ToString<unsigned short>(entityType.getSubcategory()) +
              "." + dtUtil::ToString<unsigned short>(entityType.getSpecific()) + 
              "." + dtUtil::ToString<unsigned short>(entityType.getExtra());
      LOG_WARNING("Don't know the ActorType to create for:" +  entTypeStr);
   }
}
 
void dtDIS::ESPduProcessor::UpdateActorUpdateTime(const dtCore::UniqueId& entityID) {
   double time = mGM->GetSimulationTime();
	mConfig->GetActorUpdateMap().Update(entityID, time);
}
