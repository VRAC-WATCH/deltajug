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
	
	//Set double representing the transformation matrix at position row:0 col:0
	void SetMatrix_00(const double& matrix_00);
	
	//Set double representing the transformation matrix at position row:0 col:1
	void SetMatrix_01(const double& matrix_01);
	
	//Set double representing the transformation matrix at position row:0 col:2
	void SetMatrix_02(const double& matrix_02);
	
	//Set double representing the transformation matrix at position row:0 col:3
	void SetMatrix_03(const double& matrix_03);
	
	//Set double representing the transformation matrix at position row:1 col:0
	void SetMatrix_10(const double& matrix_10);
	
	//Set double representing the transformation matrix at position row:1 col:1
	void SetMatrix_11(const double& matrix_11);
	
	//Set double representing the transformation matrix at position row:1 col:2
	void SetMatrix_12(const double& matrix_12);
	
	//Set double representing the transformation matrix at position row:1 col:3
	void SetMatrix_13(const double& matrix_13);
	
	//Set double representing the transformation matrix at position row:2 col:0
	void SetMatrix_20(const double& matrix_20);
	
	//Set double representing the transformation matrix at position row:2 col:1
	void SetMatrix_21(const double& matrix_21);
	
	//Set double representing the transformation matrix at position row:2 col:2
	void SetMatrix_22(const double& matrix_22);
	
	//Set double representing the transformation matrix at position row:2 col:3
	void SetMatrix_23(const double& matrix_23);
	
	//Set double representing the transformation matrix at position row:3 col:0
	void SetMatrix_30(const double& matrix_30);
	
	//Set double representing the transformation matrix at position row:3 col:1
	void SetMatrix_31(const double& matrix_31);
	
	//Set double representing the transformation matrix at position row:3 col:2
	void SetMatrix_32(const double& matrix_32);
	
	//Set double representing the transformation matrix at position row:3 col:3
	void SetMatrix_33(const double& matrix_33);
	
	//Return int representing the object's ID
	const int GetID() const;
	
	//Return double representing transformation matrix at position row:0 col:0
	const double GetMatrix_00() const;

	//Return double representing transformation matrix at position row:0 col:1
	const double GetMatrix_01() const;

	//Return double representing transformation matrix at position row:0 col:2
	const double GetMatrix_02() const;

	//Return double representing transformation matrix at position row:0 col:3
	const double GetMatrix_03() const;

	//Return double representing transformation matrix at position row:1 col:0
	const double GetMatrix_10() const;

	//Return double representing transformation matrix at position row:1 col:1
	const double GetMatrix_11() const;

	//Return double representing transformation matrix at position row:1 col:2
	const double GetMatrix_12() const;

	//Return double representing transformation matrix at position row:1 col:3
	const double GetMatrix_13() const;

	//Return double representing transformation matrix at position row:2 col:0
	const double GetMatrix_20() const;

	//Return double representing transformation matrix at position row:2 col:1
	const double GetMatrix_21() const;

	//Return double representing transformation matrix at position row:2 col:2
	const double GetMatrix_22() const;

	//Return double representing transformation matrix at position row:2 col:3
	const double GetMatrix_23() const;

	//Return double representing transformation matrix at position row:3 col:0
	const double GetMatrix_30() const;

	//Return double representing transformation matrix at position row:3 col:1
	const double GetMatrix_31() const;

	//Return double representing transformation matrix at position row:3 col:2
	const double GetMatrix_32() const;

	//Return double representing transformation matrix at position row:3 col:3
	const double GetMatrix_33() const;

protected:
	virtual ~TrackedMessage(void);

private:
	//Paramter identifiers for all data contained in message
	static const std::string ID_PARAMETER;
	static const std::string MATRIX00_PARAMETER;
	static const std::string MATRIX01_PARAMETER;
	static const std::string MATRIX02_PARAMETER;
	static const std::string MATRIX03_PARAMETER;
	static const std::string MATRIX10_PARAMETER;
	static const std::string MATRIX11_PARAMETER;
	static const std::string MATRIX12_PARAMETER;
	static const std::string MATRIX13_PARAMETER;
	static const std::string MATRIX20_PARAMETER;
	static const std::string MATRIX21_PARAMETER;
	static const std::string MATRIX22_PARAMETER;
	static const std::string MATRIX23_PARAMETER;
	static const std::string MATRIX30_PARAMETER;
	static const std::string MATRIX31_PARAMETER;
	static const std::string MATRIX32_PARAMETER;
	static const std::string MATRIX33_PARAMETER;

};

}

#endif


