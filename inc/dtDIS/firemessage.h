#ifndef __DELTA_DTDIS_FIRE_MESSAGE_H__
#define __DELTA_DTDIS_FIRE_MESSAGE_H__

#include "dtdisexport.h"

#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/messagefactory.h>

namespace dtDIS
{
    class DT_DIS_EXPORT FireMessageType : public dtGame::MessageType
    {
        DECLARE_ENUM(FireMessageType);
        
        public:
            static const FireMessageType FIRE;
            static void RegisterMessageTypes(dtGame::MessageFactory& factory);
            
        protected:
            FireMessageType(const std::string& name, const std::string& category, const std::string& description, const unsigned short messageId);
            virtual ~FireMessageType();
    };
    
    class DT_DIS_EXPORT FireMessage : public dtGame::Message
    {
        public:
            FireMessage();
            
            void SetLocation(const osg::Vec3& location);
            const osg::Vec3& GetLocation() const;
            
        protected:
            virtual ~FireMessage();
            
        private:
            static const std::string LOC_PARAMETER;
    };
}

#endif
