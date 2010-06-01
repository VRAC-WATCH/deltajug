#ifndef __DELTA_DTDIS_DETONATION_MESSAGE_H__
#define __DELTA_DTDIS_DETONATION_MESSAGE_H__

#include "dtdisexport.h"
#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/messagefactory.h>

namespace dtDIS
{

class DT_DIS_EXPORT DetonationMessageType : public dtGame::MessageType
{
	DECLARE_ENUM(DetonationMessageType);

public:
	static const DetonationMessageType DETONATION;
	static void RegisterMessageTypes(dtGame::MessageFactory& factory);

protected:
	DetonationMessageType(const std::string& name, const std::string& category, const std::string& description, const unsigned short messageId);
	virtual ~DetonationMessageType();
};

class DT_DIS_EXPORT DetonationMessage : public dtGame::Message
{
public:
	DetonationMessage(void);

	void SetLocation(const osg::Vec3& location);
	const osg::Vec3& GetLocation() const;

protected:
	virtual ~DetonationMessage(void);

private:
	static const std::string LOC_PARAMETER;
};

}

#endif	  // __DELTA_DTDIS_DETONATION_MESSAGE_H__
