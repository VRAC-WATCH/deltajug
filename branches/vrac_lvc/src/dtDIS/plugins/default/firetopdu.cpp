#include <dtDIS/plugins/default/firetopdu.h>

#include <DIS/FirePdu.h>
#include <DIS/EntityID.h>

#include <dtDIS/firemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/stringutils.h>

namespace dtDIS
{
    const unsigned char FireToPdu::MUNITION_KIND = 2;
    const unsigned char FireToPdu::ANTIPERSONNEL_DOMAIN = 8;
    const unsigned char FireToPdu::BALLISTIC_CATEGORY = 2;
    const unsigned char FireToPdu::M4_SUB_CAT = 1;
    const unsigned char FireToPdu::AK_SUB_CAT = 2;
    const unsigned char FireToPdu::ZERO_SPECIFIC = 0;
    const unsigned char FireToPdu::ZERO_EXTRA = 0;
//    const unsigned char FireToPdu::ENTITY_IMPACT = 1;
//    const unsigned char FireToPdu::GROUND_IMPACT = 3;
    const unsigned short FireToPdu::KINETIC_WARHEAD = 5000;
    const unsigned char FireToPdu::MULTI_FUNCTION_FUSE = 100;
    const unsigned short FireToPdu::US_COUNTRY_ID = 225;

    FireToPdu::FireToPdu(SharedState* config, dtGame::GameManager* gm)
        : mConfig(config), mGM(gm)
    {
    }
    
    FireToPdu::~FireToPdu()
    {
    }
        
    DIS::EntityID FireToPdu::GetEntityId(const dtCore::UniqueId& id)
    {
        DIS::EntityID eid;
        
        // Check to see if we already have an entity ID for this actor
        ActiveEntityControl& control = mConfig->GetActiveEntityControl();
        const DIS::EntityID* query = control.GetEntity(id);
        
        if (query)
        {
            eid = *query;
        }
        else
        {
            eid.setApplication(mConfig->GetApplicationID());
            eid.setSite(mConfig->GetSiteID());
        
            const std::string idString = id.ToString();
            char chars[4];
            for (int i = 0; i < 4; ++i)
            {
                chars[i] = idString[i];
            }
            
            eid.setEntity(strtol(&chars[0], NULL, 16));
            
            // Add to the mapping
            control.AddEntity(eid, id);
        }
        
        return eid;
    }    
        
    DIS::Pdu* FireToPdu::Convert(const dtGame::Message& source)
    {
        const FireMessage& fire = static_cast<const FireMessage&>(source);
        
        DIS::FirePdu* pdu = new DIS::FirePdu;
        
        pdu->setFiringEntityID(this->GetEntityId(fire.GetSendingActorId()));
        
        // Intentionally not setting target ID, how would we know this now?
        //pdu->setTargetEntityID();
        
        // Intentionally not setting right now. Good change this if we want to send out
        // entity states for the munition actor
        //pdu->SetMunitionID();
        
        // TODO - set the event ID
        //pdu->setEventID();
        
        // Not sure what this is for
        //pdu->setFireMissionIndex();
        
        const osg::Vec3& fireLoc = fire.GetLocation();
        osg::Vec3 disLoc = mConfig->GetCoordinateConverter().ConvertToRemoteTranslation(fireLoc);
        
        DIS::Vector3Double disPoint;
        disPoint.setX(disLoc[0]);
        disPoint.setY(disLoc[1]);
        disPoint.setZ(disLoc[2]);
        
        pdu->setLocationInWorldCoordinates(disPoint);

        DIS::BurstDescriptor burstDescriptor;
        
        DIS::EntityType entityType;
        entityType.setEntityKind(MUNITION_KIND);
        entityType.setDomain(ANTIPERSONNEL_DOMAIN);
        
        // TODO - Get country from the firer
        entityType.setCountry(US_COUNTRY_ID);
        
        
        entityType.setCategory(BALLISTIC_CATEGORY);
        
        // TODO - Based on country
        entityType.setSubcategory(M4_SUB_CAT);
        
        entityType.setSpecific(ZERO_SPECIFIC);
        entityType.setExtra(ZERO_EXTRA);
        
        burstDescriptor.setMunition(entityType);
        
        burstDescriptor.setWarhead(KINETIC_WARHEAD);
        burstDescriptor.setFuse(MULTI_FUNCTION_FUSE);
        burstDescriptor.setQuantity(1);
        burstDescriptor.setRate(1);
        
        pdu->setBurstDescriptor(burstDescriptor);
        
        // TODO
        //pdu->setVelocity();
        
        // TODO
        //pdu->setRange();

        return pdu;
    }
}
