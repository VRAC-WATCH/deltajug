#include <dtdis/DetonationMessage.h>
#include <dtgame/messagefactory.h>

namespace dtDIS
{

IMPLEMENT_ENUM(DetonationMessageType);
const DetonationMessageType DetonationMessageType::DETONATION("Detonation", "Info", "Detonation Occurred", USER_DEFINED_MESSAGE_TYPE + 1);

DetonationMessageType::DetonationMessageType(const std::string& name, const std::string& category, const std::string& description, const unsigned short messageId)
	: dtGame::MessageType(name, category, description, messageId)
{
	AddInstance(this);
}

void DetonationMessageType::RegisterMessageTypes(dtGame::MessageFactory& factory)
{
	factory.RegisterMessageType<DetonationMessage>(DETONATION);
}

DetonationMessageType::~DetonationMessageType()
{
}

const std::string DetonationMessage::LOC_PARAMETER = "DetonationLocation";

DetonationMessage::DetonationMessage()
{
	AddParameter(new dtGame::Vec3MessageParameter(LOC_PARAMETER));
}

DetonationMessage::~DetonationMessage(void)
{
}

void DetonationMessage::SetLocation(const osg::Vec3& location)
{
	dtGame::Vec3MessageParameter* param = static_cast<dtGame::Vec3MessageParameter*>(GetParameter(LOC_PARAMETER));
	param->SetValue(location);
}

const osg::Vec3& DetonationMessage::GetLocation() const
{
	return static_cast<const dtGame::Vec3MessageParameter*>(GetParameter(LOC_PARAMETER))->GetValue();
}

}