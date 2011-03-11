#include <dtDIS/firemessage.h>

namespace dtDIS
{
    IMPLEMENT_ENUM(FireMessageType);
    const FireMessageType FireMessageType::FIRE("Fire", "Info", "Fire Occurred", USER_DEFINED_MESSAGE_TYPE + 3);
    
    FireMessageType::FireMessageType(const std::string& name, const std::string& category, const std::string& description, const unsigned short messageId)
        : dtGame::MessageType(name, category, description, messageId)
    {
        AddInstance(this);
    }
    
    void FireMessageType::RegisterMessageTypes(dtGame::MessageFactory& factory)
    {
        LOG_ERROR("* Trying to register fire message *");
        factory.RegisterMessageType<FireMessage>(FIRE);
        LOG_ERROR("* After fire message register *");
    }
    
    FireMessageType::~FireMessageType()
    {
    }
    
    const std::string FireMessage::LOC_PARAMETER = "FireLocation";
    
    FireMessage::FireMessage()
    {
        AddParameter(new dtGame::Vec3MessageParameter(LOC_PARAMETER));
    }
    
    FireMessage::~FireMessage()
    {
    }
    
    void FireMessage::SetLocation(const osg::Vec3& location)
    {
        dtGame::Vec3MessageParameter* param = static_cast<dtGame::Vec3MessageParameter*>(GetParameter(LOC_PARAMETER));
        param->SetValue(location);
    }

    const osg::Vec3& FireMessage::GetLocation() const
    {
        return static_cast<const dtGame::Vec3MessageParameter*>(GetParameter(LOC_PARAMETER))->GetValue();
    }
}
