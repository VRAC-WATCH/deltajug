#include <dtDIS/sharedstate.h>
#include <dtDAL/actorproxy.h>
#include <dtUtil/xercesparser.h>
#include <dtDIS/disxml.h>

#include <cstddef>                   // for NULL
#include <dtUtil/stringutils.h>
using namespace dtDIS;

bool ActorUpdateConfig::Update(const dtCore::UniqueId& entityID, double updateTime) {
	mMap[entityID] = updateTime;
	return true;
}

bool ActorUpdateConfig::Remove(const dtCore::UniqueId& entityId) {
	ActorUpdateMap::iterator itr = mMap.find(entityId);

	if (itr != mMap.end()) {
		mMap.erase(itr);
		return true;
	}

	return false;
}

std::vector<dtCore::UniqueId> ActorUpdateConfig::GetTimedOutActors(double currentTime, double timeOut) {
	std::vector<dtCore::UniqueId> timedOutEntities;
	ActorUpdateMap::iterator itr = mMap.begin();

	while (itr != mMap.end())
	{
		if (currentTime > (itr->second + timeOut))
		{
			timedOutEntities.push_back(itr->first);
		}
		
		++itr;
	}

	return timedOutEntities;
}

bool ActorMapConfig::AddActorMapping(const DIS::EntityType& eid, const dtDAL::ActorType* at)
{
	LOG_INFO("Adding actor map: " 
			+ dtUtil::ToString(static_cast<int>(eid.getEntityKind()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getDomain()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getCountry()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getCategory()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getSubcategory()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getSpecific()))
			+ " " + dtUtil::ToString(static_cast<int>(eid.getExtra()))
			+ " - For Actor Type: " + at->GetName());
   return( mMap.insert( ActorMap::value_type(eid,at) ).second );
}

bool ActorMapConfig::RemoveActorMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ActorMapConfig::GetMappedActor(const DIS::EntityType& eid, const dtDAL::ActorType*& toWrite)
{
   ActorMap::iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = iter->second.get();
      return true;
   }

   //toWrite = NULL;
   return false;
}


bool ResourceMapConfig::AddResourceMapping(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor& resource)
{
   return( mMap.insert( ResourceMap::value_type(eid,resource) ).second );
}

bool ResourceMapConfig::RemoveResourceMapping(const DIS::EntityType& eid)
{
   return( mMap.erase( eid )>0 );
}

bool ResourceMapConfig::GetMappedResource(const DIS::EntityType& eid, const dtDAL::ResourceDescriptor*& toWrite) const
{
   ResourceMap::const_iterator iter = mMap.find( eid );
   if( iter != mMap.end() )
   {
      toWrite = &(iter->second);
      return true;
   }

   //toWrite = NULL;
   return false;
}



SharedState::SharedState(const std::string& connectionXMLFile,
                         const std::string& entityMappingXMLFile)
   : mActorMapConfig()
   , mHealthyResourceMap()
   , mDamagedResourceMap()
   , mDestroyedResourceMap()
   , mActiveEntityControl()
   , mConnectionData()
   , mSiteID(1)
   , mApplicationID(1)
{
   //TODO Should read and process a DIS xml configuration file

   if (!connectionXMLFile.empty())
   {
      //parse connection file
      ParseConnectionData(connectionXMLFile);
   }

   if (!entityMappingXMLFile.empty())
   {
      ParseEntityMappingData(entityMappingXMLFile);
   }

   //initialize to something valid
   mCoordConverter.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   mCoordConverter.SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);
}

SharedState::~SharedState()
{
}

ActorUpdateConfig& SharedState::GetActorUpdateMap() {
	return mActorUpdateConfig;
}

const ActorUpdateConfig& SharedState::GetActorUpdateMap() const {
	return mActorUpdateConfig;
}

ActorMapConfig& SharedState::GetActorMap()
{
   return mActorMapConfig;
}

const ActorMapConfig& SharedState::GetActorMap() const
{
   return mActorMapConfig;
}

ResourceMapConfig& SharedState::GetHealthyResourceMap()
{
   return mHealthyResourceMap;
}

const ResourceMapConfig& SharedState::GetHealthyResourceMap() const
{
   return mHealthyResourceMap;
}

ResourceMapConfig& SharedState::GetDamagedResourceMap()
{
   return mDamagedResourceMap;
}

const ResourceMapConfig& SharedState::GetDamagedResourceMap() const
{
   return mDamagedResourceMap;
}

ResourceMapConfig& SharedState::GetDestroyedResourceMap()
{
   return mDestroyedResourceMap;
}

const ResourceMapConfig& SharedState::GetDestroyedResourceMap() const
{
   return mDestroyedResourceMap;
}

ResourceMapConfig& SharedState::GetAnimationResourceMap()
{
	return mAnimationResourceMap;
}

const ResourceMapConfig& SharedState::GetAnimationResourceMap() const
{
	return mAnimationResourceMap;
}

ActiveEntityControl& SharedState::GetActiveEntityControl()
{
   return mActiveEntityControl;
}

const ActiveEntityControl& SharedState::GetActiveEntityControl() const
{
   return mActiveEntityControl;
}

void SharedState::SetConnectionData(const ConnectionData& data)
{
   mConnectionData = data;
   SetApplicationID(mConnectionData.application_id);
   SetSiteID(mConnectionData.site_id);
}

const ConnectionData& SharedState::GetConnectionData() const
{
   return mConnectionData;
}


////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::ParseConnectionData(const std::string& file)
{
   dtUtil::XercesParser parser;
   dtDIS::ConnectionXMLHandler handler;
   bool parsed = parser.Parse(file, handler, "dis_connection.xsd");

   if (parsed)
   {
      this->SetConnectionData(handler.GetConnectionData());
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::ParseEntityMappingData(const std::string& file)
{
   dtUtil::XercesParser parser;
   dtDIS::EntityMapXMLHandler handler(this);
   bool parsed = parser.Parse(file, handler, "dis_mapping.xsd");

   if (parsed)
   {
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetSiteID(unsigned short ID)
{
   mSiteID = ID;
}

////////////////////////////////////////////////////////////////////////////////
unsigned short dtDIS::SharedState::GetSiteID() const
{
   return mSiteID;
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetApplicationID(unsigned short ID)
{
   mApplicationID = ID;
}

////////////////////////////////////////////////////////////////////////////////
unsigned short dtDIS::SharedState::GetApplicationID() const
{
   return mApplicationID;
}

////////////////////////////////////////////////////////////////////////////////
void dtDIS::SharedState::SetCoordinateConverter(const dtUtil::Coordinates& coordConverter)
{
   mCoordConverter = coordConverter; //copy

   //Ensure that the converter is setup to what dtDIS needs.  Incoming DIS is geocentric and
   //the local is Flat earth?
   mCoordConverter.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
   mCoordConverter.SetLocalCoordinateType(dtUtil::LocalCoordinateType::CARTESIAN_FLAT_EARTH);
}

////////////////////////////////////////////////////////////////////////////////
const dtUtil::Coordinates& dtDIS::SharedState::GetCoordinateConverter() const
{
   return mCoordConverter;
}

////////////////////////////////////////////////////////////////////////////////
dtUtil::Coordinates& dtDIS::SharedState::GetCoordinateConverter()
{
   return mCoordConverter;
}
