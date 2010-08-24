#include <dtDIS/plugins/default/detonationtopdu.h>

#include <DIS/DetonationPdu.h>
#include <DIS/EntityID.h>

#include <dtDIS/detonationmessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/stringutils.h>

namespace dtDIS
{
    const unsigned char DetonationToPdu::MUNITION_KIND = 2;
    const unsigned char DetonationToPdu::ANTIPERSONNEL_DOMAIN = 8;
    const unsigned char DetonationToPdu::BALLISTIC_CATEGORY = 2;
    const unsigned char DetonationToPdu::M4_SUB_CAT = 1;
    const unsigned char DetonationToPdu::AK_SUB_CAT = 2;
    const unsigned char DetonationToPdu::ZERO_SPECIFIC = 0;
    const unsigned char DetonationToPdu::ZERO_EXTRA = 0;
    const unsigned char DetonationToPdu::ENTITY_IMPACT = 1;
    const unsigned char DetonationToPdu::GROUND_IMPACT = 3;
    const unsigned short DetonationToPdu::KINETIC_WARHEAD = 5000;
    const unsigned char DetonationToPdu::MULTI_FUNCTION_FUSE = 100;
    const unsigned short DetonationToPdu::US_COUNTRY_ID = 225;
    
    DetonationToPdu::DetonationToPdu(SharedState* config, dtGame::GameManager* gm)
        : mConfig(config), mGM(gm)
    {
    }
    
    DetonationToPdu::~DetonationToPdu()
    {
    }
        
    DIS::EntityID DetonationToPdu::GetEntityId(const dtCore::UniqueId& id)
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
        
    DIS::Pdu* DetonationToPdu::Convert(const dtGame::Message& source)
    {
        const DetonationMessage& detonation = static_cast<const DetonationMessage&>(source);
        
        DIS::DetonationPdu* pdu = new DIS::DetonationPdu;
        
        pdu->setFiringEntityID(this->GetEntityId(detonation.GetSendingActorId()));
        
        // Check to see if we hit something
        if (detonation.GetTargetHit())
        {
            // Entity Impact == 1
            pdu->setDetonationResult(ENTITY_IMPACT);
            pdu->setTargetEntityID(this->GetEntityId(detonation.GetAboutActorId()));
        }
        else
        {
            // Ground Impact == 3
            pdu->setDetonationResult(GROUND_IMPACT);
        }
        
        const osg::Vec3& detLoc = detonation.GetLocation();
        osg::Vec3 disLoc = mConfig->GetCoordinateConverter().ConvertToRemoteTranslation(detLoc);
        
        DIS::Vector3Double disPoint;
        disPoint.setX(disLoc[0]);
        disPoint.setY(disLoc[1]);
        disPoint.setZ(disLoc[2]);
        
        pdu->setLocationInWorldCoordinates(disPoint);
        
        // TODO - Rest of this stuff should be part of the message
        
        // Leaving this blank, I don't think we need to update our munition
        // actor
        //pdu->setMunitionID();
        
        // TODO - Events need a relationship between fire and detonations
        // Skipping this
        //pdu->setEventID();
        
        // Skipping velocity
        //pdu->setVelocity();
        
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
        
        return pdu;
    }
}
