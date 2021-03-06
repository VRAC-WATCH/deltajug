/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author David Guthrie
 */

#include <prefix/dtdalprefix-src.h>
#include <string>
#include <dtDAL/namedparameter.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/containeractorproperty.h>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   const char NamedParameter::DEFAULT_DELIMETER = '|';

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter::NamedParameter(DataType& dataType, const dtUtil::RefString& name, bool isList):
      AbstractParameter(dataType, name),
      mIsList(isList)
   {
      SetParamDelimeter(DEFAULT_DELIMETER);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedParameter::IsList() const
   {
      return mIsList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   char NamedParameter::GetParamDelimeter() const
   {
      return mParamListDelimeter;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetParamDelimeter(char delim)
   {
      mParamListDelimeter = delim;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in SetFromProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in ApplyValueToProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ValidatePropertyType(const dtDAL::ActorProperty& property) const
   {
      if (property.GetDataType() != GetDataType())
      {
         throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
            "Actor Property [" + property.GetName() + "] with Data Type [" + property.GetDataType().GetName() +
            "] does not match the Message Parameter [" + GetName() +
            "] with Data Type [" + GetDataType().GetName() + "]", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedParameter::CreateFromType(
      dtDAL::DataType& type, const dtUtil::RefString& name, bool isList)
   {
      dtCore::RefPtr<NamedParameter> param;

      switch (type.GetTypeId())
      {
      case dtDAL::DataType::CHAR_ID:
      case dtDAL::DataType::UCHAR_ID:
         param = new NamedUnsignedCharParameter(name,0,isList);
         break;
      case dtDAL::DataType::FLOAT_ID:
         param = new NamedFloatParameter(name,0.0f,isList);
         break;
      case dtDAL::DataType::DOUBLE_ID:
         param = new NamedDoubleParameter(name,0.0,isList);
         break;
      case dtDAL::DataType::INT_ID:
         param = new NamedIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::UINT_ID:
         param = new NamedUnsignedIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::ULONGINT_ID:
         param = new NamedUnsignedLongIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::LONGINT_ID:
         param = new NamedLongIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::SHORTINT_ID:
         param = new NamedShortIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::USHORTINT_ID:
         param = new NamedUnsignedShortIntParameter(name,0,isList);
         break;
      case dtDAL::DataType::STRING_ID:
         param = new NamedStringParameter(name,"",isList);
         break;
      case dtDAL::DataType::BOOLEAN_ID:
         param = new NamedBooleanParameter(name,false,isList);
         break;
      case dtDAL::DataType::VEC2_ID:
         param = new NamedVec2Parameter(name,osg::Vec2(0,0),isList);
         break;
      case dtDAL::DataType::VEC3_ID:
         param = new NamedVec3Parameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4_ID:
         param = new NamedVec4Parameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2F_ID:
         param = new NamedVec2fParameter(name,osg::Vec2f(0,0),isList);
         break;
      case dtDAL::DataType::VEC3F_ID:
         param = new NamedVec3fParameter(name,osg::Vec3f(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4F_ID:
         param = new NamedVec4fParameter(name,osg::Vec4f(0,0,0,0),isList);
         break;
      case dtDAL::DataType::VEC2D_ID:
         param = new NamedVec2dParameter(name,osg::Vec2d(0,0),isList);
         break;
      case dtDAL::DataType::VEC3D_ID:
         param = new NamedVec3dParameter(name,osg::Vec3d(0,0,0),isList);
         break;
      case dtDAL::DataType::VEC4D_ID:
         param = new NamedVec4dParameter(name,osg::Vec4d(0,0,0,0),isList);
         break;
      case dtDAL::DataType::RGBCOLOR_ID:
         param = new NamedRGBColorParameter(name,osg::Vec3(0,0,0),isList);
         break;
      case dtDAL::DataType::RGBACOLOR_ID:
         param = new NamedRGBAColorParameter(name,osg::Vec4(0,0,0,0),isList);
         break;
      case dtDAL::DataType::ENUMERATION_ID:
         param = new NamedEnumParameter(name,"",isList);
         break;
      case dtDAL::DataType::GAMEEVENT_ID:
         param = new NamedGameEventParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::ACTOR_ID:
         param = new NamedActorParameter(name,dtCore::UniqueId(""),isList);
         break;
      case dtDAL::DataType::GROUP_ID:
         param = new NamedGroupParameter(name);
         break;
      case dtDAL::DataType::ARRAY_ID:
         param = new NamedArrayParameter(name);
         break;
      case dtDAL::DataType::CONTAINER_ID:
         param = new NamedContainerParameter(name);
         break;
      case dtDAL::DataType::STATICMESH_ID:
      case dtDAL::DataType::TEXTURE_ID:
      case dtDAL::DataType::TERRAIN_ID:
      case dtDAL::DataType::SOUND_ID:
      case dtDAL::DataType::PARTICLESYSTEM_ID:
      case dtDAL::DataType::SKELETAL_MESH_ID:
         param = new NamedResourceParameter(type,name,isList);
         break;
      default:
         throw dtUtil::Exception(ExceptionEnum::InvalidParameter, "Type " + type.GetName() + " is not supported by the MessageParameter class.", __FILE__, __LINE__);
         break;
      }

      return param;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::WriteToLog(dtUtil::Log &logger, dtUtil::Log::LogMessageType level) const
   {
      if (logger.IsLevelEnabled(level))
      {
         std::ostringstream oss("");
         oss << "Message Parameter is: \"" << GetName() << ".\" ";
         oss << "Its message type is: \""  << GetDataType() << ".\" ";
         oss << "Its value is: \"" << ToString() << ".\"";

         logger.LogMessage(level, __FUNCTION__, __LINE__, oss.str().c_str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedGroupParameter::NamedGroupParameter(const dtUtil::RefString& name) :
      NamedParameter(dtDAL::DataType::GROUP, name, false)
   {}

   void NamedGroupParameter::ToDataStream(dtUtil::DataStream& stream) const
   {
      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mParameterList.size();

      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i != end; ++i)
      {
         stream << i->second->GetDataType().GetTypeId();
         stream << i->second->GetName();
         stream << i->second->IsList();
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromDataStream(dtUtil::DataStream& stream)
   {
      bool okay = true;

      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      for (unsigned short int i = 0; i < size; i++)
      {
         unsigned char id;
         stream >> id;
         dtDAL::DataType* type = NULL;

         for (unsigned int j = 0; j < dtDAL::DataType::EnumerateType().size(); j++)
         {
            dtDAL::DataType* d = dtDAL::DataType::EnumerateType()[j];
            if (d->GetTypeId() == id)
            {
               type = d;
               break;
            }
         }
         if (type == NULL) //|| type == &dtDAL::DataType::UNKNOWN)
         {
            throw dtUtil::Exception(ExceptionEnum::BaseException, "The datatype was not found in the stream", __FILE__, __LINE__);
            okay = false;
         }

         std::string name;
         stream >> name;

         bool isList;
         stream >> isList;

         dtCore::RefPtr<NamedParameter> param = GetParameter(name);
         if (param == NULL)
         {
            if (isList)
            {
               param = CreateFromType(*type, name, true);
               AddParameter(*param);
            }
            else
            {
               param = AddParameter(name, *type);
            }
         }

         okay = okay && param->FromDataStream(stream);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::GroupActorProperty *gap = static_cast<const dtDAL::GroupActorProperty*>(&property);
      if (gap->GetValue().valid())
      {
         CopyFrom(*gap->GetValue());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::GroupActorProperty *gap = static_cast<dtDAL::GroupActorProperty*>(&property);
      gap->SetValue(*this);
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedGroupParameter::ToString() const
   {
      std::string toFill;
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (; i!= end; ++i)
      {
         NamedParameter& param = *i->second;
         toFill.append(param.GetName());
         toFill.append(" ");
         toFill.append(dtUtil::ToString(param.GetDataType().GetName()));
         toFill.append(" ");
         // output this boolean as "true" or "false" in the string
         bool isList = param.IsList();
         if (isList)
            toFill.append("true");
         else
            toFill.append("false");

         toFill.append(" ");
         toFill.append(param.ToString());
         toFill.append(1, '\n');
      }
      return toFill;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::FromString(const std::string& value)
   {
      std::istringstream iss(value);

      std::string paramName;
      std::string typeString;
      std::string isListString;
      std::string paramValue;

      // get values
      std::getline(iss, paramName, ' ');
      std::getline(iss, typeString, ' ');
      std::getline(iss, isListString, ' ');
      std::getline(iss, paramValue);

      dtDAL::DataType *type = dtDAL::DataType::GetValueForName(typeString);

      if (type == NULL)
         return false;

      // isList
      bool isList = isListString == "true";

      // try and retrieve the parameter
      dtCore::RefPtr<NamedParameter> param = GetParameter(paramName);

      if (param == NULL)
      { // add it if it does not exist
         if (isList)
         {
            param = CreateFromType(*type, paramName, true);
            AddParameter(*param);
         }
         else
         {
            param = AddParameter(paramName, *type);
         }
      }

      param->FromString(paramValue);
      return true;

   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::CopyFrom(const NamedParameter& otherParam)
   {
      if (otherParam.GetDataType() != GetDataType())
         throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
         "The msg parameter must be of type GROUP.", __FILE__, __LINE__);

      const NamedGroupParameter& gpm = static_cast<const NamedGroupParameter&>(otherParam);

      //wipe out any existing parameters.  It's easier to just recreate them.
      mParameterList.clear();

      //copy parameters
      NamedGroupParameter::ParameterList::const_iterator i = gpm.mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = gpm.mParameterList.end();
      for (; i != end; ++i)
      {
         dtCore::RefPtr<NamedParameter> newParameter = NULL;
         if (i->second->IsList())
         {
            newParameter = CreateFromType(i->second->GetDataType(), i->first, true);
            AddParameter(*newParameter);
         }
         else
         {
            newParameter = AddParameter(i->first, i->second->GetDataType());
         }
         if (newParameter == NULL)
            //This case should not happen, the method above should throw an exception if it doesn't work, but
            //this is a case of paranoid programming.
            throw dtUtil::Exception(ExceptionEnum::BaseException,
            "Unable to create parameter of type " + i->second->GetDataType().GetName(),
            __FILE__, __LINE__);

         newParameter->CopyFrom(*i->second);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::AddParameter(const dtUtil::RefString& name,
      dtDAL::DataType& type)
   {
      dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(type, name);
      if (param.valid())
      {
         AddParameter(*param);
         return param.get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<NamedParameter> NamedGroupParameter::RemoveParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      if (itor != mParameterList.end())
      {
         dtCore::RefPtr<NamedParameter> param = itor->second;
         mParameterList.erase(itor);
         return param;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::AddParameter(NamedParameter& newParam)
   {
      if (!mParameterList.insert(std::make_pair(newParam.GetName(), &newParam)).second)
         throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
         "Could not add new parameter: "+ newParam.GetName() +
         ". A parameter with that name already exists.", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name)
   {
      NamedGroupParameter::ParameterList::iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const NamedParameter* NamedGroupParameter::GetParameter(const dtUtil::RefString& name) const
   {
      NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.find(name);
      return itor == mParameterList.end() ? NULL : itor->second.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<NamedParameter*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::iterator end = mParameterList.end();
      for (;i != end; ++i)
         toFill.push_back(i->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGroupParameter::GetParameters(std::vector<const NamedParameter*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mParameterList.size());
      NamedGroupParameter::ParameterList::const_iterator i = mParameterList.begin();
      NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
      for (;i != end; ++i)
         toFill.push_back(i->second.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGroupParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedGroupParameter& groupToCompare = static_cast<const NamedGroupParameter&>(toCompare);
         //if the size doesn't match. it's not equal.
         if (mParameterList.size() != groupToCompare.mParameterList.size())
            return false;
         NamedGroupParameter::ParameterList::const_iterator itor = mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator itorComp = groupToCompare.mParameterList.begin();
         NamedGroupParameter::ParameterList::const_iterator end = mParameterList.end();
         for (; itor != end; ++itor)
         {
            //spin through the props and return false if one is not equal.
            if (*itor->second != *itorComp->second)
               return false;

            ++itorComp;
         }
         //all props are equal.
         return true;
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedBooleanParameter::NamedBooleanParameter(const dtUtil::RefString& name, bool defaultValue,
      bool isList) : NamedPODParameter<bool>(dtDAL::DataType::BOOLEAN, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedBooleanParameter::~NamedBooleanParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedBooleanParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<bool> &values = GetValueList();

         if (values[0])
            stream << "true";
         else
            stream << "false";
         for (unsigned int i=1; i<values.size(); i++)
         {
            stream << GetParamDelimeter();
            if (values[i])
               stream << "true";
            else
               stream << "false";
         }

         return stream.str();
      }
      else
      {
         const char* result = GetValue() ? "true" : "false";
         return result;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedBooleanParameter::FromString(const std::string &value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            const std::string &str = result[i];

            bool bResult = dtUtil::ToType<bool>(str);
            GetValueList().push_back(bResult);
         }
      }
      else
      {
         bool bResult = dtUtil::ToType<bool>(value);
         SetValue(bResult);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedBooleanParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::BooleanActorProperty *bap = static_cast<const dtDAL::BooleanActorProperty*> (&property);
      SetValue(bap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedBooleanParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::BooleanActorProperty *bap = static_cast<dtDAL::BooleanActorProperty*> (&property);
      bap->SetValue(GetValue());
   }


   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedCharParameter::NamedUnsignedCharParameter(const dtUtil::RefString& name,
      unsigned char defaultValue, bool isList) :
         NamedPODParameter<unsigned char>(dtDAL::DataType::UCHAR, name, defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedCharParameter::~NamedUnsignedCharParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedShortIntParameter::NamedShortIntParameter(const dtUtil::RefString& name,
      short defaultValue, bool isList) :
         NamedPODParameter<short>(dtDAL::DataType::SHORTINT, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedShortIntParameter::~NamedShortIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedShortIntParameter::NamedUnsignedShortIntParameter(const dtUtil::RefString& name,
      unsigned short defaultValue, bool isList) :
         NamedPODParameter<unsigned short>(dtDAL::DataType::USHORTINT, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedShortIntParameter::~NamedUnsignedShortIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedIntParameter::NamedUnsignedIntParameter(const dtUtil::RefString& name,
      unsigned int defaultValue, bool isList)  :
         NamedPODParameter<unsigned int>(dtDAL::DataType::UINT, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedIntParameter::~NamedUnsignedIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedIntParameter::NamedIntParameter(const dtUtil::RefString& name, int defaultValue, bool isList) :
   NamedPODParameter<int>(dtDAL::DataType::INT, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedIntParameter::~NamedIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedIntParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::IntActorProperty *ap = static_cast<const dtDAL::IntActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedIntParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::IntActorProperty *ap = static_cast<dtDAL::IntActorProperty*> (&property);
      ap->SetValue(GetValue());
   }


   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedLongIntParameter::NamedUnsignedLongIntParameter(const dtUtil::RefString& name,
      unsigned long defaultValue, bool isList) :
         NamedPODParameter<unsigned long>(dtDAL::DataType::ULONGINT, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedUnsignedLongIntParameter::~NamedUnsignedLongIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedLongIntParameter::NamedLongIntParameter(const dtUtil::RefString& name, long defaultValue,
      bool isList) : NamedPODParameter<long>(dtDAL::DataType::LONGINT, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedLongIntParameter::~NamedLongIntParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedLongIntParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::LongActorProperty *ap = static_cast<const dtDAL::LongActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedLongIntParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::LongActorProperty *ap = static_cast<dtDAL::LongActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedFloatParameter::NamedFloatParameter(const dtUtil::RefString& name, float defaultValue,
      bool isList) : NamedPODParameter<float>(dtDAL::DataType::FLOAT, name, defaultValue, isList)
   {
      SetNumberPrecision(9);
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedFloatParameter::~NamedFloatParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedFloatParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::FloatActorProperty *ap = static_cast<const dtDAL::FloatActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedFloatParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::FloatActorProperty *ap = static_cast<dtDAL::FloatActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedDoubleParameter::NamedDoubleParameter(const dtUtil::RefString& name, double defaultValue,
      bool isList) : NamedPODParameter<double>(dtDAL::DataType::DOUBLE, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedDoubleParameter::~NamedDoubleParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedDoubleParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::DoubleActorProperty *ap = static_cast<const dtDAL::DoubleActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedDoubleParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::DoubleActorProperty *ap = static_cast<dtDAL::DoubleActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::NamedStringParameter(const dtUtil::RefString& name, const std::string& defaultValue,
      bool isList) : NamedGenericParameter<std::string>(DataType::STRING, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::NamedStringParameter(DataType& dataType, const dtUtil::RefString& name,
      const std::string& defaultValue, bool isList) :
         NamedGenericParameter<std::string>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedStringParameter::~NamedStringParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedStringParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::StringActorProperty *ap = static_cast<const dtDAL::StringActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedStringParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::StringActorProperty *ap = static_cast<dtDAL::StringActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedStringParameter::ToString() const
   {
      if (IsList())
      {
         const std::vector<std::string> &values = GetValueList();
         std::ostringstream stream;

         if (values[0].empty())
            stream << "null";
         else
            stream << values[0];

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i];
         }

         return stream.str();
      }
      else
      {
         return GetValue();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedStringParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back("");
            else
               GetValueList().push_back(result[i]);
         }
      }
      else
      {
         SetValue(value);
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::NamedArrayParameter(const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(DataType::ARRAY, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::NamedArrayParameter(DataType& dataType, const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(dataType, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedArrayParameter::~NamedArrayParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::ArrayActorPropertyBase *ap = static_cast<const dtDAL::ArrayActorPropertyBase*> (&property);
      SetValue(ap->ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedArrayParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::ArrayActorPropertyBase *ap = static_cast<dtDAL::ArrayActorPropertyBase*> (&property);
      ap->FromString(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedArrayParameter::ToString() const
   {
      return GetValue();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedArrayParameter::FromString(const std::string& value)
   {
      SetValue(value);
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::NamedContainerParameter(const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(DataType::CONTAINER, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::NamedContainerParameter(DataType& dataType, const dtUtil::RefString& name)
      : NamedGenericParameter<std::string>(dataType, name, "", false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedContainerParameter::~NamedContainerParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::ContainerActorProperty* ap = static_cast<const dtDAL::ContainerActorProperty*> (&property);
      SetValue(ap->ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedContainerParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::ContainerActorProperty *ap = static_cast<dtDAL::ContainerActorProperty*> (&property);
      ap->FromString(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedContainerParameter::ToString() const
   {
      return GetValue();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedContainerParameter::FromString(const std::string& value)
   {
      SetValue(value);
      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedEnumParameter::NamedEnumParameter(const dtUtil::RefString& name, const std::string& defaultValue,
      bool isList) : NamedStringParameter(dtDAL::DataType::ENUMERATION, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedEnumParameter::~NamedEnumParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedEnumParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      SetValue(property.ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedEnumParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      if (!property.FromString(GetValue()))
         LOG_ERROR(("Failed to set the enum value \"" + GetValue() + "\" on property \"" + GetName() + "\".").c_str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedActorParameter::NamedActorParameter(const dtUtil::RefString& name,
      const dtCore::UniqueId& defaultValue, bool isList) :
   NamedGenericParameter<dtCore::UniqueId>(dtDAL::DataType::ACTOR, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedActorParameter::~NamedActorParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedActorParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      if (property.GetDataType() == DataType::ACTOR)
      {
         dtCore::UniqueId id(property.ToString());
         SetValue(id);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedActorParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ActorIDActorProperty* actorIDProp = dynamic_cast<ActorIDActorProperty*>(&property);
      if (actorIDProp != NULL)
      {
         actorIDProp->SetValue(GetValue());
      }
      else
      {

         throw dtUtil::Exception(ExceptionEnum::InvalidParameter,
            "Cannot call ApplyValueToProperty()on an ActorMessageParameter.  See the GameActor::ApplyActorUpdate() for an example of how to do this.",
            __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedActorParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<dtCore::UniqueId> &values = GetValueList();

         if (values[0].ToString().empty())
            stream << "null";
         else
            stream << values[0].ToString();

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].ToString().empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i].ToString();
         }

         return stream.str();
      }
      else
         return GetValue().ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedActorParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back(dtCore::UniqueId(""));
            else
               GetValueList().push_back(dtCore::UniqueId(result[i]));
         }
      }
      else
      {
         SetValue(dtCore::UniqueId(value));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedGameEventParameter::NamedGameEventParameter(const dtUtil::RefString& name,
      const dtCore::UniqueId& defaultValue, bool isList) :
   NamedGenericParameter<dtCore::UniqueId>(dtDAL::DataType::GAME_EVENT, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedGameEventParameter::~NamedGameEventParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGameEventParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      FromString(property.ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedGameEventParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      property.FromString(ToString());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedGameEventParameter::ToString() const
   {
      if (IsList())
      {
         std::ostringstream stream;
         const std::vector<dtCore::UniqueId> &values = GetValueList();

         if (values[0].ToString().empty())
            stream << "null";
         else
            stream << values[0].ToString();

         for (unsigned int i=1; i<values.size(); i++)
         {
            if (values[i].ToString().empty())
               stream << GetParamDelimeter() << "null";
            else
               stream << GetParamDelimeter() << values[i].ToString();
         }

         return stream.str();
      }
      else
         return GetValue().ToString();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedGameEventParameter::FromString(const std::string& value)
   {
      if (IsList())
      {
         std::vector<std::string> result;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         GetValueList().clear();
         for (unsigned int i=0; i<result.size(); i++)
         {
            if (result[i] == "null")
               GetValueList().push_back(dtCore::UniqueId(""));
            else
               GetValueList().push_back(dtCore::UniqueId(result[i]));
         }
      }
      else
      {
         SetValue(dtCore::UniqueId(value));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2Parameter::NamedVec2Parameter(const dtUtil::RefString& name,
      const osg::Vec2& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec2>(dtDAL::DataType::VEC2, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2Parameter::~NamedVec2Parameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2Parameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2ActorProperty *ap = static_cast<const dtDAL::Vec2ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2Parameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec2ActorProperty *ap = static_cast<dtDAL::Vec2ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2fParameter::NamedVec2fParameter(const dtUtil::RefString& name,
      const osg::Vec2f& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec2f>(dtDAL::DataType::VEC2F, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2fParameter::~NamedVec2fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2fParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2fActorProperty *ap = static_cast<const dtDAL::Vec2fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2fParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec2fActorProperty *ap = static_cast<dtDAL::Vec2fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2dParameter::NamedVec2dParameter(const dtUtil::RefString& name,
      const osg::Vec2d& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec2d>(dtDAL::DataType::VEC2D, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec2dParameter::~NamedVec2dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2dParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec2dActorProperty *ap = static_cast<const dtDAL::Vec2dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec2dParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec2dActorProperty *ap = static_cast<dtDAL::Vec2dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3Parameter::NamedVec3Parameter(const dtUtil::RefString& name,
      const osg::Vec3& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec3>(dtDAL::DataType::VEC3, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3Parameter::NamedVec3Parameter(DataType& dataType, const dtUtil::RefString& name,
      const osg::Vec3& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec3>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3Parameter::~NamedVec3Parameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3Parameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3ActorProperty *ap = static_cast<const dtDAL::Vec3ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3Parameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec3ActorProperty *ap = static_cast<dtDAL::Vec3ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedRGBColorParameter::NamedRGBColorParameter(const dtUtil::RefString& name,
      const osg::Vec3& defaultValue, bool isList) :
         NamedVec3Parameter(dtDAL::DataType::RGBCOLOR, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedRGBColorParameter::~NamedRGBColorParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3fParameter::NamedVec3fParameter(const dtUtil::RefString& name,
      const osg::Vec3f& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec3f>(dtDAL::DataType::VEC3F, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3fParameter::~NamedVec3fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3fParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3fActorProperty *ap = static_cast<const dtDAL::Vec3fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3fParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec3fActorProperty *ap = static_cast<dtDAL::Vec3fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3dParameter::NamedVec3dParameter(const dtUtil::RefString& name,
      const osg::Vec3d& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec3d>(dtDAL::DataType::VEC3D, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec3dParameter::~NamedVec3dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3dParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec3dActorProperty *ap = static_cast<const dtDAL::Vec3dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec3dParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec3dActorProperty *ap = static_cast<dtDAL::Vec3dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4Parameter::NamedVec4Parameter(const dtUtil::RefString& name,
      const osg::Vec4& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec4>(dtDAL::DataType::VEC4, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4Parameter::NamedVec4Parameter(DataType& dataType, const dtUtil::RefString& name,
      const osg::Vec4& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec4>(dataType, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4Parameter::~NamedVec4Parameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4Parameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4ActorProperty *ap = static_cast<const dtDAL::Vec4ActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4Parameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec4ActorProperty *ap = static_cast<dtDAL::Vec4ActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedRGBAColorParameter::NamedRGBAColorParameter(const dtUtil::RefString& name,
      const osg::Vec4& defaultValue, bool isList)
      : NamedVec4Parameter(dtDAL::DataType::RGBACOLOR, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedRGBAColorParameter::~NamedRGBAColorParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4fParameter::NamedVec4fParameter(const dtUtil::RefString& name,
      const osg::Vec4f& defaultValue, bool isList)
      : NamedVecParameter<osg::Vec4f>(dtDAL::DataType::VEC4F, name, defaultValue, isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4fParameter::~NamedVec4fParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4fParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4fActorProperty *ap = static_cast<const dtDAL::Vec4fActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4fParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec4fActorProperty *ap = static_cast<dtDAL::Vec4fActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4dParameter::NamedVec4dParameter(const dtUtil::RefString& name,
      const osg::Vec4d& defaultValue, bool isList) :
         NamedVecParameter<osg::Vec4d>(dtDAL::DataType::VEC4D, name,defaultValue,isList)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedVec4dParameter::~NamedVec4dParameter()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4dParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::Vec4dActorProperty *ap = static_cast<const dtDAL::Vec4dActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedVec4dParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::Vec4dActorProperty *ap = static_cast<dtDAL::Vec4dActorProperty*> (&property);
      ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   NamedResourceParameter::NamedResourceParameter(dtDAL::DataType& type,  const dtUtil::RefString& name,
      bool isList) : NamedParameter(type, name,isList)
   {
      if (IsList())
         mValueList = new std::vector<dtDAL::ResourceDescriptor>();
      else
         mValueList = NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      ValidatePropertyType(property);

      const dtDAL::ResourceActorProperty *ap = static_cast<const dtDAL::ResourceActorProperty*> (&property);
      SetValue(ap->GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      ValidatePropertyType(property);

      dtDAL::ResourceActorProperty *vap = static_cast<dtDAL::ResourceActorProperty*> (&property);
      if (GetValue() != NULL)
      {
         dtDAL::ResourceDescriptor newValue(*GetValue());
         vap->SetValue(&newValue);
      }
      else
      {
         vap->SetValue(NULL);
      }

      //dtDAL::ResourceActorProperty *ap = static_cast<dtDAL::ResourceActorProperty*> (&property);
      //ap->SetValue(GetValue());
   }

   ///////////////////////////////////////////////////////////////////////////////
   NamedResourceParameter::~NamedResourceParameter()
   {
      delete mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::ToDataStream(dtUtil::DataStream &stream) const
   {
      if (IsList())
      {
         stream << mValueList->size();
         for (unsigned int i=0; i<mValueList->size(); i++)
         {
            stream << (*mValueList)[i].GetResourceIdentifier();
            stream << (*mValueList)[i].GetDisplayName();
         }
      }
      else
      {
         stream << mDescriptor.GetResourceIdentifier();
         stream << mDescriptor.GetDisplayName();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::FromDataStream(dtUtil::DataStream &stream)
   {
      bool okay = true;

      std::string resourceId;
      std::string resourceDisplayName;

      if (IsList())
      {
         mValueList->clear();

         unsigned int listSize;
         stream >> listSize;
         for (unsigned int i=0; i<listSize; i++)
         {
            stream >> resourceId;
            stream >> resourceDisplayName;
            mValueList->push_back(dtDAL::ResourceDescriptor(resourceDisplayName, resourceId));
         }
      }
      else
      {
         stream >> resourceId;
         stream >> resourceDisplayName;
         mDescriptor = dtDAL::ResourceDescriptor(resourceDisplayName, resourceId);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::string NamedResourceParameter::ToString() const
   {
      std::ostringstream stream;

      if (IsList())
      {
         const std::vector<dtDAL::ResourceDescriptor> &values =
            GetValueList();

         stream << values[0].GetDisplayName() << "/" << values[0].GetResourceIdentifier();
         for (unsigned int i=1; i<values.size(); i++)
            stream << GetParamDelimeter() << values[i].GetDisplayName() <<
            "/" << values[i].GetResourceIdentifier();
      }
      else
      {
         const dtDAL::ResourceDescriptor* r = GetValue();
         if (r != NULL)
            stream << r->GetDisplayName() << "/" << r->GetResourceIdentifier();
      }

      return stream.str();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::FromString(const std::string& value)
   {
      bool result = true;
      if (value.empty() || value == "NULL")
      {
         if (IsList())
            mValueList->clear();
         else
            SetValue(NULL);

         return true;
      }

      std::string displayName;
      std::string identifier;
      dtUtil::StringTokenizer<dtUtil::IsSlash> stok;
      std::vector<std::string> tokens;
      if (IsList())
      {
         std::vector<std::string> result;
         unsigned int i;
         dtUtil::IsDelimeter delimCheck(GetParamDelimeter());
         dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(result,value,delimCheck);

         mValueList->clear();
         for (i=0; i<result.size(); i++)
         {
            tokens.clear();

            #ifdef DELTA_WIN32
            stok = stok;
            #endif
            stok.tokenize(tokens,result[i]);
            if (tokens.size() == 2)
            {
               displayName = tokens[0];
               identifier = tokens[1];
            }
            else
            {
               //assume the value is a descriptor and use it for both the
               //data and the display name.
               displayName = tokens[0];
               identifier = tokens[0];
            }

            dtUtil::Trim(identifier);
            dtUtil::Trim(displayName);
            mValueList->push_back(dtDAL::ResourceDescriptor(displayName, identifier));
         }
      }
      else
      {
         stok.tokenize(tokens,value);
         if (tokens.size() == 2)
         {
            displayName = tokens[0];
            identifier = tokens[1];
         }
         else
         {
            //assume the value is a descriptor and use it for both the
            //data and the display name.
            displayName = tokens[0];
            identifier = tokens[0];
         }

         dtUtil::Trim(identifier);
         dtUtil::Trim(displayName);

         dtDAL::ResourceDescriptor descriptor(displayName,identifier);
         SetValue(&descriptor);
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::CopyFrom(const NamedParameter& otherParam)
   {
      const NamedResourceParameter *param =
         dynamic_cast<const NamedResourceParameter*>(&otherParam);


      //First make sure this parameter does not have a list if the
      //other parameter does and vice versa.
      if ((IsList() && !otherParam.IsList()) ||(!IsList() && otherParam.IsList()))
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot assign two parameters with one being a list of values and the other not.",
         __FILE__, __LINE__);

      if (param != NULL)
      {
         if (!IsList())
            SetValue(param->GetValue());
         else
            SetValueList(param->GetValueList());
      }
      else
      {
         if (!FromString(otherParam.ToString()))
            LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetValue(const dtDAL::ResourceDescriptor* descriptor)
   {
      if (IsList())
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot call SetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      mDescriptor = descriptor == NULL ? dtDAL::ResourceDescriptor("","") : *descriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::ResourceDescriptor* NamedResourceParameter::GetValue() const
   {
      if (IsList())
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot call GetValue() on message parameter with a list of values.", __FILE__, __LINE__);

      if (mDescriptor.GetResourceIdentifier().empty())
         return NULL;
      else
         return &mDescriptor;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const std::vector<dtDAL::ResourceDescriptor> &NamedResourceParameter::GetValueList() const
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::vector<dtDAL::ResourceDescriptor> &NamedResourceParameter::GetValueList()
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
      return *mValueList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedResourceParameter::SetValueList(const std::vector<dtDAL::ResourceDescriptor> &newValues)
   {
      if (!IsList())
         throw dtUtil::Exception(ExceptionEnum::BaseException,
         "Cannot set a list of new values on a parameter that is not a list.", __FILE__, __LINE__);

      *mValueList = newValues;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedResourceParameter::operator==(const NamedParameter& toCompare) const
   {
      if (GetDataType() == toCompare.GetDataType())
      {
         const NamedResourceParameter& rpToCompare = static_cast<const NamedResourceParameter&>(toCompare);

         if (GetValue() != NULL && rpToCompare.GetValue() != NULL)
            return *GetValue() == *rpToCompare.GetValue();
         else
            return GetValue() == rpToCompare.GetValue();
      }
      return false;
   }
}
