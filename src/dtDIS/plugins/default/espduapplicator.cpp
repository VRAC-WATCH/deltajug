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
   //LCR: "Non-damaged actor" (aka RESOURCE_DAMAGE_OFF) property is set to the mesh specified in disActorTypeMapping.xml file
   mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::RESOURCE_DAMAGE_OFF , dtDAL::DataType::STATIC_MESH );
   if( mp != NULL )
   {
      if (config != NULL)
      {
         const ResourceMapConfig& resources = config->GetResourceMap();
         const dtDAL::ResourceDescriptor* rdPtr = NULL;
         bool found = resources.GetMappedResource( source.getEntityType(), rdPtr ) ;

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

           found = resources.GetMappedResource( defaultType, rdPtr ) ;
         }
         //LCR

         if( found )
         {
            dtDAL::NamedResourceParameter* nrp = static_cast<dtDAL::NamedResourceParameter*>( mp );
            nrp->SetValue( rdPtr );
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


   // --- support the engine-core properties. --- //
   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(EnginePropertyName::ENTITY_LOCATION))
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      osg::Vec3 val = v3mp->GetValue();
      if (config != NULL)
      {
         val = config->GetCoordinateConverter().ConvertToRemoteTranslation(val);        
      }

      DIS::Vector3Double loc;
      loc.setX(val[0]);
      loc.setY(val[1]);
      loc.setZ(val[2]);
      dest.setEntityLocation(loc);
   }

   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_ORIENTATION)) 
   {
      // DIS EntityState actor property
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      const osg::Vec3& val = v3mp->GetValue();
      DIS::Orientation orie;
      orie.setPhi(osg::DegreesToRadians(val[0])); //pitch
      orie.setTheta(osg::DegreesToRadians(val[1])); //roll
      orie.setPsi(osg::DegreesToRadians(val[2])); //heading
      dest.setEntityOrientation(orie);
   }

   if (const dtGame::MessageParameter* mp = source.GetUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY))
   {
      // DIS EntityState actor property
      //TODO convert to DIS coordinate system?
      const dtGame::Vec3MessageParameter* v3mp = static_cast<const dtGame::Vec3MessageParameter*>(mp);
      const osg::Vec3& val = v3mp->GetValue();
      DIS::Vector3Float vel;
      vel.setX(val[0]);
      vel.setY(val[1]);
      vel.setZ(val[2]);
      dest.setEntityLinearVelocity(vel);
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
      dest.setEntityAppearance( appearance );
   }
}


///\todo use dtUtil::Coordinates::ConvertToLocalRotation for ENTITY_ORIENTATION.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromVelocityVectorToMessageParam for ENTITY_LINEARY_VELOCITY.
///\todo implement dtHLAGM::RPRParameterTranslator::MapFromAngularVelocityVectorToMessageParam for ANGULAR_VELOCITY.
void PartialApplicator::operator ()(const DIS::EntityStatePdu& source,
                                    dtGame::ActorUpdateMessage& dest,
                                    dtDIS::SharedState* config) 
{
   dtDAL::NamedParameter* mp ;
   
   // position //
   const DIS::Vector3Double& pos = source.getEntityLocation() ;
   osg::Vec3d v3(pos.getX(), pos.getY(), pos.getZ());

   //LCR: DIS debugging
   //char buffer[128];
   //sprintf(buffer, "DIS xyz: %f, %f, %f", pos.getX(), pos.getY(), pos.getZ());
   //LOG_INFO(buffer);
   //LCR

   if (config != NULL)
   {
      v3 = config->GetCoordinateConverter().ConvertToLocalTranslation(v3);
   }

   // dtDIS Actor Property Name 
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_LOCATION, dtDAL::DataType::VEC3)))
   {
      dtGame::Vec3MessageParameter* v3mp = static_cast<dtGame::Vec3MessageParameter*>(mp);
      v3mp->SetValue(v3);
   }

   // euler angles //
   osg::Vec3 xyzRot;

   if (config != NULL)
   {
      const DIS::Orientation& orie = source.getEntityOrientation();
      const osg::Vec3 hpr = config->GetCoordinateConverter().ConvertToLocalRotation(orie.getPsi(), 
                                                                                    orie.getTheta(), 
                                                                                    orie.getPhi());
      xyzRot[0] = hpr[1]; //swap from HPR to "rotations about the axis"
      xyzRot[1] = hpr[2];
      xyzRot[2] = hpr[0];
      //xyzRot[0] = osg::RadiansToDegrees(orie.getPhi()); //pitch
      //xyzRot[1] = osg::RadiansToDegrees(orie.getTheta()); //roll
      //xyzRot[2] = osg::RadiansToDegrees(orie.getPsi()); //heading

   }

   // dtDIS Actor Property Name
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::LAST_KNOWN_ORIENTATION, dtDAL::DataType::VEC3)))
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter*>(mp);
      v3mp->SetValue(xyzRot);
   }

   // velocity //

   // dtDIS Actor Property Name
   if ((mp = dest.AddUpdateParameter(dtDIS::EnginePropertyName::ENTITY_LINEARY_VELOCITY, dtDAL::DataType::VEC3 )))
   {

       //LCR: convert velocity from GCC to local coordinates
       // There is probably a better way to do this! 
       //(probably involving a simple rotation of the vector)
       const DIS::Vector3Float&  velGcc = source.getEntityLinearVelocity();
       const DIS::Vector3Double& posGcc = source.getEntityLocation();

       osg::Vec3d u3(posGcc.getX() + velGcc.getX(), 
                     posGcc.getY() + velGcc.getY(),
                     posGcc.getZ() + velGcc.getZ());

       if (config != NULL)
       {
           u3 = config->GetCoordinateConverter().ConvertToLocalTranslation(u3);
           dtDAL::NamedVec3Parameter* v3mp = static_cast<dtDAL::NamedVec3Parameter*>(mp);
           v3mp->SetValue(u3 - v3);
       }
       //LCR
   }


#if 0
   UpdateAcceleration( vel ) ;

   if( mp = dest.AddUpdateParameter( dtDIS::HLABaseEntityPropertyName::PROPERTY_ACCELERATION_VECTOR , dtDAL::DataType::VEC3 ) )
   {
      dtDAL::NamedVec3Parameter* v3mp = static_cast< dtDAL::NamedVec3Parameter* > ( mp ) ;
      v3mp->SetValue( mAcceleration ) ;
   }
#endif

   //TODO: add angular velocity vector

   // articulation support
   unsigned char art_param_count=source.getArticulationParameters().size();
   if( art_param_count > 0 )
   {
      mp = dest.AddUpdateParameter( dtDIS::EnginePropertyName::ARTICULATION, dtDAL::DataType::GROUP );
      if( mp != NULL )
      {
         dtDAL::NamedGroupParameter* articulation_group = static_cast<dtDAL::NamedGroupParameter*>( mp );

         // respond to each articulation parameter
         const std::vector<DIS::ArticulationParameter>& params = source.getArticulationParameters();
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
   }  // end if( art_param_count
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
