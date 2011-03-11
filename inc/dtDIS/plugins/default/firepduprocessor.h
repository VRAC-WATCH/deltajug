/*
Dignitas Technologies, LLC

*/

#ifndef __DELTA_PLUGIN_FIREPDU_PROCESSOR_H__
#define __DELTA_PLUGIN_FIREPDU_PROCESSOR_H__

#include <DIS/IPacketProcessor.h>   // for base class
#include <DIS/FirePdu.h>
#include <DIS/EntityID.h>
#include <map>
#include <dtCore/observerptr.h>          // for member

#include <dtDIS/entityidcompare.h>
#include <dtGame/messagefactory.h>        // for usage

#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>      // for export symbols


///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class EntityStatePdu;
}
///@endcond

namespace dtDAL
{
   class ActorProxy; 
   class ActorType;
}

namespace dtGame
{
   class GameManager;
   class GameActorProxy;
}

namespace dtDIS
{
   class SharedState;

   /// the interface class responsible for applying data from a DIS::FirePduProcessor packet.
   class DT_DIS_DEFAULT_EXPORT FirePduProcessor : public DIS::IPacketProcessor
   {
   public:
       FirePduProcessor(dtGame::GameManager* gm, SharedState* config);
      ~FirePduProcessor();

      void Process(const DIS::Pdu& p);

   protected:
      void NotifyRemoteActor(const DIS::FirePdu& pdu, const dtDAL::ActorProxy& actor);
      
   private:
      FirePduProcessor();  ///< not implemented by design

      //using a refptr results in a circular reference.
      dtCore::ObserverPtr<dtGame::GameManager> mGM;
      SharedState* mConfig;
   };
}

#endif  // __DELTA_PLUGIN_FIREPDU_PROCESSOR_H__
