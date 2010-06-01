#include <dtDIS/connection.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <iostream>
#include <sstream>

using namespace dtDIS;

void Connection::Connect(unsigned int port, const char* host)
{
   NLboolean success = nlInit();

   if (!success)
   {
      HandleError();
      return;
   }

   if (nlSelectNetwork(NL_IP) == NL_FALSE)
   {
      LOG_ERROR("Can't select network type");
   }

   NLaddress  maddr;
   if (nlStringToAddr(host, &maddr) == NL_FALSE )
   {
      std::ostringstream strm;
      strm << "Can't get address for : " + std::string(host) + ". "
           << "Error:" << nlGetErrorStr(nlGetError())
           << ". System: " << nlGetSystemErrorStr(nlGetSystemError());
      LOG_ERROR( strm.str() );
   }

   nlSetAddrPort(&maddr, port);

   //LCR: (mods to nlHint)
   //nlHint(NL_MULTICAST_TTL, NL_TTL_LOCAL);
   nlHint(NL_REUSE_ADDRESS, NL_TRUE);
   //LCR

   //LCR: (mods to nlOpen)
   //mSocket = nlOpen(port, NL_UDP_MULTICAST);
   mSocket = nlOpen(port, NL_UDP_BROADCAST);
   //LCR

//	LOG_INFO("!!!!!!!!!!!!! OPENING SOCKET !!!!!!!!!!!!!!!!!!!!!!!!!!!!");

   if(mSocket == NL_INVALID)
   {
      std::ostringstream strm;
      strm << "Can't open socket: " << nlGetErrorStr(nlGetError())
           << ". System: " << + nlGetSystemErrorStr(nlGetSystemError());
      LOG_ERROR( strm.str() )
   } 
   else
   {
	   NLaddress addr;
	   nlGetLocalAddr(mSocket, &addr);

	   NLbyte byteString[256];
//	   LOG_INFO("Socket address: " + dtUtil::ToString(nlAddrToString(&addr, byteString)));
   }

   /*
   //LCR: (no need to call nlConnect for broadcast!)
   if(nlConnect(mSocket, &maddr) == NL_FALSE)
   {
	   switch(nlGetError()) {

	   case NL_INVALID_SOCKET:
			LOG_ERROR("NL_INVALID_SOCKET");
			break;
	   case NL_NULL_POINTER:
		   LOG_ERROR("NL_NULL_POINTER");
			break;
	   case NL_SYSTEM_ERROR:
		   LOG_ERROR("NL_SYSTEM_ERROR");
			break;
	   case NL_CON_REFUSED:
		   LOG_ERROR("NL_CON_REFUSED");
			break;
	   case NL_WRONG_TYPE:
		   LOG_ERROR("NL_WRONG_TYPE");            //broadcast
			break;
	   default:
		   LOG_ERROR("nlGetError was no help!");  //multicast 
		   break;
	   }

      nlClose(mSocket);

      std::ostringstream strm;
      strm << "Can't connect to socket: " << nlGetErrorStr(nlGetError())
           << ". System: " << nlGetSystemErrorStr(nlGetSystemError());
      LOG_ERROR( strm.str() );
   }
   */
}



///\todo is this the ideal NL call?
void Connection::Disconnect()
{
   nlShutdown();
}

void Connection::Send(const char* buf, size_t numbytes)
{
   if( numbytes < 1 )
   {
      return;
   }
#if 0
   LOG_INFO("***** Connection::Send, buf: " + dtUtil::ToString((void*)buf) + ", numbytes: " + dtUtil::ToString(numbytes)); 
   
   LOG_INFO("***** numbytes: " + dtUtil::ToString(numbytes));
   LOG_INFO("***** SOCKET: " + dtUtil::ToString(mSocket));		
   LOG_INFO("**** SENDING: " + dtUtil::ToString(buf));
#endif
#if 1

//   LOG_INFO("** Sending PDU Message ***");

   if (int ret = nlWrite(mSocket, (NLvoid *)buf, numbytes ))
   {
      if (ret == 0)
      {
         LOG_WARNING("Network buffers are full");
      }
      else if (ret == NL_INVALID)
      {
         std::ostringstream strm;
         strm << "Problem sending: ";
         LOG_ERROR(strm.str() + nlGetErrorStr(nlGetError()) + ". System: " + nlGetSystemErrorStr(nlGetSystemError()) );
      }
   }
#endif
}

size_t Connection::Receive(char* buf, size_t numbytes)
{
#if 0
	LOG_INFO("TRYING TO RECEIVE A MESSAGE");

	LOG_INFO("NUM BYTES: " + dtUtil::ToString(numbytes));
	
	LOG_INFO("size of buf: " + dtUtil::ToString(sizeof buf));

	std::cerr.flush();
	std::cout.flush();
#endif
#if 0
	char* localBuffer = new char[1500];

   //NLint result = nlRead(mSocket, (NLvoid *)buf, (NLint)numbytes);
	NLint result = nlRead(mSocket, (NLvoid *)localBuffer, (NLint)numbytes);
	strcpy(localBuffer, buf);
	delete[] localBuffer;
#endif
#if 0
	const size_t MTU = 1500;
	char buffer[MTU];
	NLint result = nlRead(mSocket, (NLvoid*) buffer, (NLint)MTU);
#endif
	NLint result = nlRead(mSocket, (NLvoid *)buf, (NLint)numbytes);
	

   if ( result == NL_INVALID )
   {
      HandleError();
      return 0;
   }
	
   return result;
}

void Connection::HandleError()
{
   NLenum error = nlGetError();
   const NLchar* errorString = nlGetErrorStr( error );

   LOG_ERROR("A network error occurred: " + std::string(errorString));

   NLint errorInt = nlGetSystemError();
   const NLchar* systemErrorString = nlGetSystemErrorStr(errorInt);

   LOG_ERROR("A system error occurred: " + std::string(systemErrorString));
}

