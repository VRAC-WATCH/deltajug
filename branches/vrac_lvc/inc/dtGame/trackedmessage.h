/*-------------------------NOTES ON CREATING A MESSAGE IN D3D-------------------------
Written by Brice Pollock (7/19/2010)

You should use the trackedmessage class, as well as classes in dtGame/basemessages.h 
as a reference when creating your message class.

First of all you must create your message type as a message object in 
dtGame/messagetype.h and define it in dtGame/messagetype.cpp.  Use the other message
types as a guide and make sure you add the number of custom messages you have created
to the USER_DEFINED_MESSAGE_TYPE variable for the id.  For example, trackedmessage
was the second custom message so it's id was USER_DEFINED_MESSAGE_TYPE + 2.
 
Messages are created from the Message Factory found in dtGame/messagefactory.cpp
In the constructor MessageFactory::MessageFactory you will need to register this
message type using the registration for other message types as an example.  Make 
sure to include your message class.

Lastly, in order for an actor do anything with these messages they must be 
received using an invokable.  Therefore, you should go to the base actor
class, dtGame/gameactor.h and dtGame/gameactor.cpp.  First of all you will need to 
create the invokable for your messagetype at the top of the GameActorProxy class are 
the other message invokable declarations.  Next you should define the invoakble at the 
top of .cpp file using the other definitions as an example.  Then you need to locate 
the BuildInvokables function to add your invokable to the GameActorProxy class.  If 
you would like to call a specific method for reading your messages for example 
OnTickLocal(), you will want to create a virtual function in the GameActorProxy class.  
Dont forget to tell the header to use your class in the header file (class TrackedMessage.h).

Note:
--Be sure to register your message in the OnEnteredWorld function in the actor class you
wish to recieve those messages types.
--Your message type will be recieved in the OnProcessMessage function by default.
--See JugglerAppViewer.cpp in djGameStart of VeldtIG for examples on how to create, 
fill, and send a message.
*/





#ifndef TRACKED_MESSAGE
#define TRACKED_MESSAGE

#include <dtGame/messagetype.h>
#include <dtGame/message.h>
#include <dtGame/gamemanager.h>

namespace dtGame
{


class DT_GAME_EXPORT TrackedMessage : public dtGame::Message
{
public:
        TrackedMessage(void);
        
        //Set int representing the object's ID
        void SetID(const int& Id);
        
        //Set Boolean indicating whether the actor's gun's trigger was pulled
        void SetFire(const bool& fire);
        
        //Sets translation vector for actor
        void SetActorTrans(const osg::Vec3f& trans);
        
        //Sets rotation vector for actor
        void SetActorRot(const osg::Vec3f& rot);
        
        //Sets rotation angle belonging to rotation vector for actor
        void SetActorAng(const double& ang);
        
        //Sets translation vector for gun
        void SetGunTrans(const osg::Vec3f& trans);
        
        //Sets rotation vector for gun
        void SetGunRot(const osg::Vec3f& rot);
        
        //Sets rotation angle belonging to rotation vector for gun
        void SetGunAng(const double& ang);
        
        //Return int representing the object's ID
        const int GetID() const;
        
        //Returns Boolean which indicates whether actor's gun's trigger was pulled
        const bool GetFire() const;
        
        //Returns translation vector for actor
        const osg::Vec3f GetActorTrans() const;
        
        //Returns rotation vector for actor
        const osg::Vec3f GetActorRot() const;
        
        //Returns rotation angle belonging to rotation vector for actor
        const double GetActorAng() const;
        
        //Returns translation vector for gun
        const osg::Vec3f GetGunTrans() const;
        
        //Returns rotation vector for gun
        const osg::Vec3f GetGunRot() const;
        
        //Returns roation angle belonging to rotation vector for gun
        const double GetGunAng() const;

protected:
        virtual ~TrackedMessage(void);

private:
        //Paramter identifiers for all data contained in message
        static const std::string ID_PARAMETER;
        static const std::string FIRE_PARAMETER;
        
        static const std::string ACT_TRANS_PARAMETER;
        static const std::string ACT_ROT_PARAMETER;
        static const std::string ACT_ANG_PARAMETER;
        
        static const std::string GUN_TRANS_PARAMETER;
        static const std::string GUN_ROT_PARAMETER;
        static const std::string GUN_ANG_PARAMETER;
        
};

}

#endif


