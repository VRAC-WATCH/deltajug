#include <dtDIS/plugins/default/espduapplicator.h>

#include <DIS/EntityStatePdu.h>
#include <DIS/Conversion.h>
#include <DIS/StreamUtils.h>
#include <DIS/Orientation.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtDIS/valuemaps.h>
#include <dtDIS/articulationconstants.h>

#include <dtDAL/datatype.h>
#include <dtUtil/stringutils.h>
#include <dtDAL/resourcedescriptor.h>

#include <dtDIS/propertyname.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/namedparameter.h>
#include <dtGame/deadreckoningcomponent.h>
#include <dtUtil/coordinates.h>

#include <sstream>

using namespace dtDIS::details;

void FullApplicator::operator ()(const DIS::EntityStatePdu& source,
                                 dtGame::ActorUpdateMessage& dest,
                                 dtDIS::SharedState* config) const
{
   PartialApplicator partial;
   partial(source, dest, config);

   dtDAL::NamedParameter* mp;

   mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_MARKING, dtDAL::DataType::STRING);
   if (mp != NULL)
   {
      dtDAL::NamedStringParameter* strAP = static_cast<dtDAL::NamedStringParameter*>(mp);
      
      strAP->SetValue(source.getMarking().getCharacters());
   }


   

   source.getEntityType();
   
   // Keep the undamaged model around...
   const dtDAL::ResourceDescriptor* undamagedResource = NULL;
   
   //LCR: "Non-damaged actor" (aka RESOURCE_DAMAGE_OFF) property is set to the mesh specified in disActorTypeMapping.xml file
   mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::RESOURCE_DAMAGE_OFF , dtDAL::DataType::STATIC_MESH );
   if( mp != NULL )
   {
//		LOG_INFO("In Here");

      if (config != NULL)
      {
		  const ResourceMapConfig& resources = config->GetHealthyResourceMap();
		bool found = resources.GetMappedResource( source.getEntityType(), undamagedResource ) ;

         //LCR: Use DIS Enum (0,0,0,0,0,0,0) for a default mapping to an random object of some sort
         //Obviously for this to work the mapping must be in the mapping file and the mapped Mesh must available
         if(!found ) 
         {

           DIS::EntityType defaultType;
           defaultType.setCategory(0);
           defaultType.setCountry(0);
           defaultType.setDomain(0);
           defaultType.setEntityKind(0);
           defaultType.setExtra(0);
           defaultType.setSpecific(0);
           defaultType.setSubcategory(0);

           found = resources.GetMappedResource( defaultType, undamagedResource) ;
         }
         //LCR

         if( found )
         {
            dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>( mp );
            nrp->SetValue(undamagedResource);
         }
      }
   }

   // Add damaged model
    mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::RESOURCE_DAMAGE_ON , dtDAL::DataType::STATIC_MESH );
	
	if (mp != NULL)
    {
       if (config != NULL)
       {
		  const ResourceMapConfig& resources = config->GetDamagedResourceMap();
		  const dtDAL::ResourceDescriptor* rdPtr = NULL;
          bool found = resources.GetMappedResource(source.getEntityType(), rdPtr);

	      if(found)
          {
			dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>(mp);
            nrp->SetValue(rdPtr);
          }
		  // This entity type does not have a damaged model, so just use the undamaged model
		  else
		  {
			  if (undamagedResource)
			  {
				  dtDAL::NamedResourceParameter* parameter = static_cast<dtDAL::NamedResourceParameter*>(mp);
				  parameter->SetValue(undamagedResource);
			  }
		  }
      }
   }

   // Add destroyed model
	mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::RESOURCE_DAMAGE_DESTROYED , dtDAL::DataType::STATIC_MESH );
	
	if (mp != NULL)
    {
       if (config != NULL)
       {
		  const ResourceMapConfig& resources = config->GetDestroyedResourceMap();
		  const dtDAL::ResourceDescriptor* rdPtr = NULL;
          bool found = resources.GetMappedResource(source.getEntityType(), rdPtr);

	      if(found)
          {
			dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>(mp);
            nrp->SetValue(rdPtr);
          }
		  // This entity type does not have a damaged model, so just use the undamaged model
		  else
		  {
			  if (undamagedResource)
			  {
				  dtDAL::NamedResourceParameter* parameter = static_cast<dtDAL::NamedResourceParameter*>(mp);
				  parameter->SetValue(undamagedResource);
			  }
		  }
      }
   }

   std::string drm;
   if( ValueMap::GetDeadReckoningModelPropertyValue( source.getDeadReckoningParameters().getDeadReckoningAlgorithm(), drm ) )
   {
      mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::DEAD_RECKONING_ALGORITHM, dtDAL::DataType::ENUMERATION );
      if( mp != NULL )
      {
         dtDAL::NamedEnumParameter* ep = static_cast<dtDAL::NamedEnumParameter*>( mp );
         ep->SetValue( drm );
      }
   }

   // ground clamping property
   {
      bool doclamp(false);
      bool issupported = ValueMap::GetRequiresGroundClamping( source.getEntityType(),doclamp );
      if( issupported )
      {
         dtDAL::NamedBooleanParameter* nbpptr = new dtDAL::NamedBooleanParameter(dtDIS::EnginePropertyName::GROUND_CLAMP);
         ///\todo BMH actor likes "do flying",
         /// so use the opposite until delta3d offers an actor that uses this property.
         nbpptr->SetValue( !doclamp );
         dest.AddUpdateParameter( *nbpptr );
      }
      else
      {
         std::ostringstream strm;
         strm << "Can not determine if ground clamping is needed for Entity:" << std::endl
              << source.getEntityID() << std::endl
              << "of type:" << std::endl
              << source.getEntityType();
         LOG_ERROR( strm.str() )
         ///\todo should not have added the parameter, so remove it here, or change the code above.
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void FullApplicator::operator ()(const dtGame::ActorUpdateMessage& source,
                                 const DIS::EntityID& eid,
                                 DIS::EntityStatePdu& dest,
                                 dtDIS::SharedState* config) const
{
   // --- support the engine-core properties. --- //
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(EnginePropertyName::ENTITY_MARKING))
   {
      const dtGame::StringMessageParameter* v3mp = static_cast<const dtGame::StringMessageParameter*>(mp);
      const std::string value = v3mp->GetValue();
      DIS::Marking marking;
      marking.setCharacters(value.c_str());
      dest.setMarking(marking);
   }

   // These will be used later, right now assuming that they work
   osg::Vec3 gameTranslation;
   osg::Vec3 disTranslation;
   
   // --- support the engine-core properties. --- //
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(EnginePropertyName::ENTITY_LOCATION))
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      gameTranslation = v3mp->GetValue();
      
	  if (config != NULL)
      {
         disTranslation = config->GetCoordinateConverter().ConvertToRemoteTranslation(gameTranslation);        
      }

		DIS::Vector3Double disPoint;
      disPoint.setX(disTranslation[0]);
      disPoint.setY(disTranslation[1]);
      disPoint.setZ(disTranslation[2]);
      dest.setEntityLocation(disPoint);
   }

   // Convert orientation from game matrix orientation to DIS euler angles...
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_ORIENTATION)) 
   {
	   const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
	   const osg::Vec3& gameOrientation = v3mp->GetValue();

       //hack 
       osg::Vec3 gameOri(gameOrientation.z(), gameOrientation.y(), gameOrientation.x());
	   
	   // If we don't have config we should probably throw an exception, cause none of this is going to work
	   if (config)
	   {
			// Convert to DIS rotation
		   osg::Vec3d remoteRotation = config->GetCoordinateConverter().ConvertToRemoteRotation(gameOri);

		   // These may have to be jimmied around, to receive we mapped 1-2-0...
			float psi = remoteRotation[0]; 
			float theta = remoteRotation[1]; 
			float phi = remoteRotation[2];

			DIS::Orientation disOrientation;
			disOrientation.setPsi(psi);
			disOrientation.setTheta(theta);
			disOrientation.setPhi(phi);
			dest.setEntityOrientation(disOrientation);
	   }
   }

   // Convert velocity vector from game coordinates to DIS coordinates
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY))
   {
		// To calculate this, we will need to add the game velocity vector to the game coordinate.
	    // Then convert both of those to DIS coordinate space.  Then subtract...
	   const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
	   const osg::Vec3& gameVelocity = v3mp->GetValue();

	   osg::Vec3 gameEndPoint = gameTranslation + gameVelocity;

		if (config)
		{
			osg::Vec3 end = config->GetCoordinateConverter().ConvertToRemoteTranslation(gameEndPoint);
			end -= disTranslation;

			DIS::Vector3Float disVelocity;
			disVelocity.setX(end[0]);
			disVelocity.setY(end[1]);
			disVelocity.setZ(end[2]);
      
			dest.setEntityLinearVelocity(disVelocity);
		}
   }

   /// support the dead reckoning data
   {
      ///\todo adjust more settings
      ///\todo verify the algorithm assignments
      DIS::DeadReckoningParameter drparams;
      if(const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::DEAD_RECKONING_ALGORITHM) )
      {
         const dtDAL::NamedEnumParameter* nep = static_cast< const dtDAL::NamedEnumParameter* >( mp ) ;
         const std::string& val = nep->GetValue();

         unsigned char algo( 0 );
         if( val == dtGame::DeadReckoningAlgorithm::STATIC.GetName() )
         {
            algo = 1;
         }
         else if( val == dtGame::DeadReckoningAlgorithm::VELOCITY_ONLY.GetName() )
         {
            algo = 2;
         }
         else if( val == dtGame::DeadReckoningAlgorithm::VELOCITY_AND_ACCELERATION.GetName() )
         {
            algo = 3;
         }
         drparams.setDeadReckoningAlgorithm( algo );
      }

      dest.setDeadReckoningParameters( drparams );
   }

   // --- attempt to support the non-engine-core properties. --- //

   // this was already stored locally by the calling code,
   // but could have come from a group message parameter
   // if such a property existed, which it probably does not.
   dest.setEntityID( eid );

   if(const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EntityPropertyName::APPEARANCE) )
   {
      const dtGame::IntMessageParameter* imp = static_cast<const dtGame::IntMessageParameter*>( mp );
      int appearance = imp->GetValue();
	  LOG_INFO("***** ENTITY APPEARANCE: " + dtUtil::ToString(appearance));
      dest.setEntityAppearance( appearance );
   }
}


///\todo use dtUtil::Coordinates::ConvertToLocalRotation for ENTITY_ORIENTATION.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromVelocityVectorToMessageParam for ENTITY_LINEARY_VELOCITY.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromAngularVelocityVectorToMessageParam for ANGULAR_VELOCITY.
void PartialApplicator::operator ()(const DIS::EntityStatePdu& sourcePdu,
                                    dtGame::ActorUpdateMessage& gameMessage,
                                    dtDIS::SharedState* sharedState) 
{
	// Going to exit if we can't get shared state info
	if (!sharedState) {
		return;
	}

   dtDAL::NamedParameter* parameter;
   
   // Get the position of the entity in world coordinates...
   const DIS::Vector3Double& disPosition = sourcePdu.getEntityLocation() ;
   osg::Vec3d position(disPosition.getX(), disPosition.getY(), disPosition.getZ());

   position = sharedState->GetCoordinateConverter().ConvertToLocalTranslation(position);
      
   parameter = gameMessage.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_LOCATION, dtDAL::DataType::VEC3);
   if (parameter)
   {
      static_cast<dtGame::Vec3MessageParameter*>(parameter)->SetValue(position);
   }

   // compute euler angles...
   osg::Vec3 xyzRotation;

   const DIS::Orientation& orientation = sourcePdu.getEntityOrientation();
   const osg::Vec3 headingPitchRoll = 
	   sharedState->GetCoordinateConverter().ConvertToLocalRotation(
	      orientation.getPsi(), orientation.getTheta(), orientation.getPhi());
      
   xyzRotation[0] = headingPitchRoll[0];
   xyzRotation[1] = headingPitchRoll[1];
   xyzRotation[2] = headingPitchRoll[2];
   
   parameter = gameMessage.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_ORIENTATION, dtDAL::DataType::VEC3);
   if (parameter)
   {
      static_cast<dtDAL::NamedVec3Parameter*>(parameter)->SetValue(xyzRotation);
   }

   // Get the velocity
   parameter = gameMessage.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY, dtDAL::DataType::VEC3 );
   if (parameter)
   {
       const DIS::Vector3Float&  velocityGcc = sourcePdu.getEntityLinearVelocity();
       const DIS::Vector3Double& positionGcc = sourcePdu.getEntityLocation();

       osg::Vec3d addedPositionGcc(
	      positionGcc.getX() + velocityGcc.getX(), 
          positionGcc.getY() + velocityGcc.getY(),
          positionGcc.getZ() + velocityGcc.getZ());

       addedPositionGcc = sharedState->GetCoordinateConverter().ConvertToLocalTranslation(addedPositionGcc);
       static_cast<dtDAL::NamedVec3Parameter*>(parameter)->SetValue(addedPositionGcc - position);
   }

   // Appearence
   parameter = gameMessage.AddUpdateParameter(dtDIS::EntityPropertyName::APPEARANCE, dtDAL::DataType::INT);
   if (parameter)
   {
	  static_cast<dtDAL::NamedIntParameter*>(parameter)->SetValue(sourcePdu.getEntityAppearance());
   }

#if 1
   //TODO: add angular velocity vector

   // TODO - special case for different entity articulation support
   unsigned char art_param_count = sourcePdu.getArticulationParameters().size();
   if( art_param_count > 0 )
   {
      parameter = gameMessage.AddUpdateParameter( dtDIS::EnginePropertyName::ARTICULATION, dtDAL::DataType::GROUP );
      if( parameter != NULL )
      {
         dtDAL::NamedGroupParameter* articulation_group = static_cast<dtDAL::NamedGroupParameter*>( parameter );

         // respond to each articulation parameter
         const std::vector<DIS::ArticulationParameter>& params = sourcePdu.getArticulationParameters();
         if( params.size() != art_param_count )
         {  // some debug help, error checking
            LOG_DEBUG("Parameter count does not equal the number of parameters received.")
         }
         for(char art_param_index=0; art_param_index<art_param_count; ++art_param_index)
         {
            char designator = params[art_param_index].getParameterTypeDesignator();
            switch( designator )
            {
            case dtDIS::Articulation::ARTICULATED_PART:
               {
                  AddArticulationMessageParameters( params[art_param_index], articulation_group, art_param_index );
               } break;

            case dtDIS::Articulation::ATTACHED_PART:
               {
                  LOG_INFO("No articulation support for attached parts.")
               } break;

            default:
               {
                  LOG_ERROR("Unknown Articulation Parameter Type Designator with value: " + dtUtil::ToString(designator) )
               } break;
            }  // end switch
         } // end for
      } // end if( mp
   } // end if( art_param_count
#endif
}

#if 0
void PartialApplicator::UpdateAcceleration( osg::Vec3& currentVelocity )
{
   // TODO: determine mUpdateTimeFrame between each EntityState PDU received

 //  mTimeRec = ??

   mAcceleration = ( currentVelocity - mLastVelocity ) / /* mTimeRec */ 0.01f ;
   mLastVelocity = currentVelocity ;
}
#endif

void PartialApplicator::AddArticulationMessageParameters(const DIS::ArticulationParameter& source,
                                                         dtDAL::NamedGroupParameter* topgroup,
                                                         unsigned int param_index) const
{
   int parametertype = source.getParameterType();
   int typemetric = DIS::Convert::GetArticulationTypeMetric( parametertype );
   int typeclass = DIS::Convert::GetArticulationTypeClass( parametertype );

   // in short, the name is arbitrary, but the 'r' is important.
   // this really incredible naming convention was not invented by the original author.
   // however, the original author supported it in order to gain the support of the actor, dvte::IG::Entity.
   const char second_letter_in_name = 'r';
   char name_array[3];
   name_array[0] = param_index + 1;  // never let the first letter value be '0'
   name_array[1] = second_letter_in_name;
   name_array[2] = '\0';
   dtDAL::NamedGroupParameter* datagroup = new dtDAL::NamedGroupParameter( name_array );
   topgroup->AddParameter( *datagroup );

   AddPartParameter(typeclass, datagroup);
   AddMotionParameter(typemetric, source.getParameterValue(), datagroup);
}

void PartialApplicator::AddPartParameter(unsigned int partclass, dtDAL::NamedGroupParameter* parent) const
{
   // find the node to modify
   std::string nodename;
   if( ValueMap::GetArticulationNodeName(partclass, nodename) )
   {
      AddStringParam( dtDIS::EnginePropertyName::DOF_NODE_NAME, nodename, parent);
   }
   else
   {
      LOG_DEBUG("Unsupported Articulation type class of value: " + dtUtil::ToString(partclass))
   }
}

void PartialApplicator::AddMotionParameter(unsigned int motionclass, double motionvalue, dtDAL::NamedGroupParameter* parent) const
{
    //LCR: Crazy stuff going on with the articulated parameters!
    //There appears to be a bug (or perhaps a version discrepancy)
    //in the underlying DIS library.  Reference: ext/inc/DIS/ArticulationParameter.h
    //basically their structure for receiving the ArticulatedParameters doesn't appear correct
    //         Spec                      Spec                DIS Library
    //---------------------------------------------------------------------------
    //Parameter Type Designator Field     8                 char (correct)     
    //Parameter Change Indicator Field    8                 char (correct)
    //Articulation Attachment ID Field    16                unsignedshort (correct)
    //Parameter Type Variant              64                int (sort of correct) -- missing a second int to make total of 64 bits
    //Articulation Parameter Value Field  64                double (correct)

    // 
    //the "correct" value is in the second 32 bits of the passed in double motionvalue
    float* pFloat                = (float*)&motionvalue;
    float hackedFloatMotionValue = *(pFloat + 1);
    //char buf[128];
    //sprintf(buf, "PartialApplicator::AddMotionParameter: %f", motionvalue);
    //LOG_INFO(buf);
    //sprintf(buf, "hackedFloatMotionValue: %f", hackedFloatMotionValue);
    //LOG_INFO(buf);    
    //LCR

   // find the property to modify
   std::string propertyname;
   if( ValueMap::GetArticulationMotionPropertyName(motionclass, propertyname) )
   {
      //LCR: 
      //AddFloatParam( propertyname, motionvalue, parent);
      AddFloatParam( propertyname, hackedFloatMotionValue, parent);
      //LCR
   }
   else
   {
      LOG_DEBUG("Unsupported Articulation type metric of value: " + dtUtil::ToString(motionclass))
   }
}

void PartialApplicator::AddFloatParam(const std::string& name, float value, dtDAL::NamedGroupParameter* parent) const
{
   dtDAL::NamedFloatParameter* param = new dtDAL::NamedFloatParameter( name );
   param->SetValue( value );
   parent->AddParameter( *param );
}

void PartialApplicator::AddStringParam(const std::string& name, const std::string& value, dtDAL::NamedGroupParameter* parent) const
{
   dtDAL::NamedStringParameter* param = new dtDAL::NamedStringParameter( name );
   param->SetValue( value );
   parent->AddParameter( *param );
}
