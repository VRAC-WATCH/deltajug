#ifndef __DELTA_PLUGIN_FIRE_TO_PDU_H__
#define __DELTA_PLUGIN_FIRE_TO_PDU_H__

#include <dtDIS/imessagetopacketadapter.h>
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>

namespace dtGame
{
    class GameManager;
}

namespace dtCore
{
    class UniqueId;
}

namespace DIS
{
    class FirePdu;
    class EntityID;
}

namespace dtDIS
{
    class SharedState;
    
    class DT_DIS_DEFAULT_EXPORT FireToPdu : public dtDIS::IMessageToPacketAdapter
    {
        public:
            FireToPdu(dtDIS::SharedState* config, dtGame::GameManager* gm);
            ~FireToPdu();
            
            DIS::Pdu* Convert(const dtGame::Message& source);
            
        private:
            static const unsigned char MUNITION_KIND;

            static const unsigned char ANTIPERSONNEL_DOMAIN;
            
            static const unsigned char BALLISTIC_CATEGORY;
            
            static const unsigned char M4_SUB_CAT;
            static const unsigned char AK_SUB_CAT;
        
            static const unsigned char ZERO_SPECIFIC;
            static const unsigned char ZERO_EXTRA;
        
//            static const unsigned char ENTITY_IMPACT;
//            static const unsigned char GROUND_IMPACT;
            static const unsigned short KINETIC_WARHEAD;
            static const unsigned char MULTI_FUNCTION_FUSE;
//            static const unsigned char ENTITY_TYPE;
            
            static const unsigned short US_COUNTRY_ID;
      
            FireToPdu(); //< not implemented by design
            
            DIS::EntityID GetEntityId(const dtCore::UniqueId& id);
                        
            dtDIS::SharedState* mConfig;
            dtGame::GameManager* mGM;
            DIS::Pdu* mPdu;
    };
}

#endif
