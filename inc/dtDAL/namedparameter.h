/* -*-c++-*-
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
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef DELTA_NAMED_PARAMETER
#define DELTA_NAMED_PARAMETER

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <osg/Referenced>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec2d>
#include <osg/Vec3d>
#include <osg/Vec4d>
#include <map>
#include <dtUtil/stringutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/datastream.h>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/abstractparameter.h>

//using dtUtil::dtUtil::DataStream;

namespace dtDAL
{
   class ActorProperty;

   /**
    * @class MessageParameter
    */
   class DT_DAL_EXPORT NamedParameter : public AbstractParameter
   {
      public:

         static const char DEFAULT_DELIMETER;

         NamedParameter(DataType& dataType, const dtUtil::RefString& name, bool isList = false);

         /**
          * @param otherParam the message parameter to make this one match.
          */
         virtual void CopyFrom(const NamedParameter& otherParam) = 0;

         ///Writes pertinent data about this parameter to the given logger and log level.
         void WriteToLog(dtUtil::Log& logger, dtUtil::Log::LogMessageType level = dtUtil::Log::LOG_DEBUG) const;

         /**
          * @return True if the paramater contains a list of
          *    data, false if this parameter only stores one value.
          */
         bool IsList() const;

         /**
          * Creates a message parameter that will hold the given type.
          * @param type the datatype the parameter should hold.
          * @param name the name of the parameter to create.
          * @throw dtUtil::Exception with dtGame::ExceptionEnum::INVALID_PARAMETER if the type is unsupported.
          */
         static dtCore::RefPtr<NamedParameter> CreateFromType(
                  dtDAL::DataType& type,
                  const dtUtil::RefString& name, bool isList = false);

         /**
          * Gets the character used as a delimeter between parameters when serializing
          * a message parameter containing a list of values to a string.
          * @return The delimeter.  By default, this is equal to "|".
          */
         char GetParamDelimeter() const;

         /**
          * Allows one to override the default delimeter used to serialize message
          * parmeters to a string.
          * @param delim The new delimeter.  This should NOT be the space character!
          *    Default value for the delimeter is the pipe character. '|'
          * @see GetParamDelimeter
          */
         void SetParamDelimeter(char delim);


         /**
          * Sets the message parameter's value from the actor property's value
          */
         virtual void SetFromProperty(const dtDAL::ActorProperty& property);

         /**
          * Sets the actor property's value from the message parameter's value
          */
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const = 0;
         bool operator!=(const NamedParameter& toCompare) const { return !(*this == toCompare); }

      protected:

         virtual ~NamedParameter() { }

         /**
          * Helper method for each specific parameter type to be able to
          * validate the property type before acting on it
          */
         void ValidatePropertyType(const dtDAL::ActorProperty& property) const;

      private:
         //This value is used as a delimeter between list data elements
         //when converting to and from a string.
         char mParamListDelimeter;

         bool mIsList;
   };

   class DT_DAL_EXPORT NamedGroupParameter : public NamedParameter
   {
      public:
         typedef std::map<dtUtil::RefString, dtCore::RefPtr<NamedParameter> > ParameterList;

         NamedGroupParameter(const dtUtil::RefString& name);

         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         virtual bool FromDataStream(dtUtil::DataStream& stream);

         virtual const std::string ToString() const;

         virtual bool FromString(const std::string& value);

         virtual void CopyFrom(const NamedParameter& otherParam);

         /**
          * Adds a parameter to the group
          * @param name The name of the parameter to add
          * @param type The type of parameter it is, corresponding with dtDAL::DataType
          * @return A pointer to the parameter
          * @see dtDAL::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         NamedParameter* AddParameter(const dtUtil::RefString& name, dtDAL::DataType& type);

         /**
          * Removes a parameter to the group
          * @param name The name of the parameter to remove
          * @return A ref pointer to the parameter removed or NULL if nothing was removed.  It returns a ref ptr in case
          *         this group is the only object holding on to a reference.
          */
         dtCore::RefPtr<NamedParameter> RemoveParameter(const dtUtil::RefString& name);

         /**
          * Adds a parameter to the group
          * @param param the new parameter to add
          * @see dtDAL::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         void AddParameter(NamedParameter& newParam);

         /**
          * Retrieves the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @return A pointer to the parameter or NULL if no such parameter exists
          */
         NamedParameter* GetParameter(const dtUtil::RefString& name);

         /**
          * Retrieves const pointer to the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @return A const pointer to the parameter or NULL if no such parameter exists
          */
         const NamedParameter* GetParameter(const dtUtil::RefString& name) const;

         /**
          * Retrieves all of the parameters in this group.
          * @param toFill The vector to fill with the parameters
          */
         void GetParameters(std::vector<NamedParameter*>& toFill);

         /**
          * Retrieves all of the parameters in this group as const.
          * @param toFill The vector to fill with the parameters
          */
         void GetParameters(std::vector<const NamedParameter*>& toFill) const;

         template <class UnaryFunction>
         void ForEachParameter(UnaryFunction function) const
         {
            /*std::for_each(mParameterList.begin(), mParameterList.end(),
                  std::compose1(std::select2nd<std::pair<std::string, dtCore::RefPtr<NamedParameter> > >(),
                        function));*/
            ParameterList::const_iterator i = mParameterList.begin();
            ParameterList::const_iterator end = mParameterList.end();
            for (; i != end; ++i)
            {
               function(i->second);
            }
         }

         /**
          * Return the amount of iters in the group list
          * @return the size of the internal messageparam map size
          */
         unsigned int GetParameterCount() const {return mParameterList.size();}

         /**
          * Sets the message parameter's value from the actor property's value
          */
         virtual void SetFromProperty(const dtDAL::ActorProperty& property);

         /**
          * Sets the actor property's value from the message parameter's value
          */
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const;

      private:
         ParameterList mParameterList;
   };

   template <class ParamType>
   class NamedGenericParameter : public NamedParameter
   {
      public:
         NamedGenericParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue, bool isList = false)
            : NamedParameter(dataType, name, isList)
            , mValue(defaultValue)
            , mNumberPrecision(17)
         {
            if (IsList())
            {
               mValueList = new std::vector<ParamType>();
               mValueList->push_back(defaultValue);
            }
            else
            {
               mValueList = NULL;
            }
         }

         /**
          * Sets the precision of floating point number as they are
          * converted to and from string values.
          * @param precision The amount of numeric percision.
          * @note This value defaults to 16.
          */
         inline void SetNumberPrecision(unsigned int precision)
         {
            mNumberPrecision = precision;
         }

         /**
          * Gets the current floating point precision value on this
          * parameter.
          * @return The current number precision.
          */
         inline unsigned int GetNumberPrecision() const
         {
            return mNumberPrecision;
         }

         inline virtual void CopyFrom(const NamedParameter& otherParam)
         {
            const NamedGenericParameter<ParamType>* param =
                static_cast<const NamedGenericParameter<ParamType>*>(&otherParam);

            //First make sure this parameter does not have a list if the
            //other parameter does and vice versa.
            if ((IsList() && !otherParam.IsList()) ||(!IsList() && otherParam.IsList()))
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot assign two parameters with one being a list of values and the other not.", __FILE__, __LINE__);
            }

            if (param != NULL)
            {
               if (!IsList())
               {
                 SetValue(param->GetValue());
               }
               else
               {
                  SetValueList(param->GetValueList());
               }
            }
            else
            {
               if (!FromString(otherParam.ToString()))
               {
                  LOGN_ERROR("MessageParameter", "Parameter types are incompatible. Cannot copy the value.");
               }
            }
         }

         inline virtual void SetValue(const ParamType& value)
         {
            if (IsList())
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot call SetValue() on message parameter with a list of values.", __FILE__, __LINE__);
            }

            mValue = value;
         }

         inline virtual const ParamType& GetValue() const
         {
            if (IsList())
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot call GetValue() on message parameter with a list of values.", __FILE__, __LINE__);
            }

            return mValue;
         }

         inline const std::vector<ParamType>& GetValueList() const
         {
            if (!IsList())
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
            }
            return *mValueList;
         }

         inline std::vector<ParamType>& GetValueList()
         {
            if (!IsList())
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot retrieve the parameters value list.  Parameter does not contain a list.", __FILE__, __LINE__);
            }
            return *mValueList;
         }

         inline void SetValueList(const std::vector<ParamType>& newValues)
         {
            if (!IsList())
            {
               throw dtUtil::Exception(ExceptionEnum::BaseException,
                  "Cannot set a list of new values on a parameter that is not a list.", __FILE__, __LINE__);
            }

            *mValueList = newValues;
         }

         inline virtual void ToDataStream(dtUtil::DataStream& stream) const
         {
            if (IsList())
            {
               stream << mValueList->size();
               for (unsigned int i=0; i<mValueList->size(); i++)
               {
                  stream << (*mValueList)[i];
               }
            }
            else
            {
               stream << mValue;
            }
         }

         inline virtual bool FromDataStream(dtUtil::DataStream& stream)
         {
            bool okay = true;

            if (IsList())
            {
               mValueList->clear();

               unsigned int listSize;
               stream >> listSize;
               for (unsigned int i = 0; i < listSize; ++i)
               {
                  ParamType newElement;
                  stream >> newElement;
                  mValueList->push_back(newElement);
               }
            }
            else
            {
               stream >> mValue;
            }

            return okay;
         }

         virtual const std::string ToString() const { return std::string(); }
         virtual bool FromString(const std::string& value) { return false; }

         virtual bool operator==(const NamedParameter& toCompare) const
         {
            if (GetDataType() == toCompare.GetDataType())
            {
               return GetValue() == static_cast<const NamedGenericParameter<ParamType>&>(toCompare).GetValue();
            }
            return false;
         }

      protected:
         ///Keep destructors protected to ensure property smart pointer management.
         virtual ~NamedGenericParameter() { delete mValueList; }

      private:
         std::vector<ParamType>* mValueList;
         ParamType mValue;
         unsigned int mNumberPrecision;
   };

   /**
    * @class NamedPODParameter
    * This is a template for the single piece of data types to make reading data to and from a string easier.
    */
   template <class ParamType>
   class NamedPODParameter: public NamedGenericParameter<ParamType>
   {
      public:
         NamedPODParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue, bool isList = false)
            : NamedGenericParameter<ParamType>(dataType, name, defaultValue, isList)
         {
         }

         virtual const std::string ToString() const
         {
            std::ostringstream stream;
            stream.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());
            if (NamedGenericParameter<ParamType>::IsList())
            {
               const std::vector<ParamType>& values =
                  NamedGenericParameter<ParamType>::GetValueList();

               stream << values[0];
               for (unsigned int i = 1; i < values.size(); ++i)
               {
                  stream << NamedGenericParameter<ParamType>::GetParamDelimeter()
                     << values[i];
               }
            }
            else
            {
              stream << NamedGenericParameter<ParamType>::GetValue();
            }

            return stream.str();
         }

         virtual bool FromString(const std::string& value)
         {
            std::istringstream stream;
            stream.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());

            if (NamedGenericParameter<ParamType>::IsList())
            {
               std::vector<ParamType>& result =
                  NamedGenericParameter<ParamType>::GetValueList();

               ParamType actual;
               result.clear();
               stream.str(value);
               stream >> actual;
               result.push_back(actual);
               while (stream)
               {
                  char sep;
                  stream >> sep;
                  if (!stream)
                  {
                     break;
                  }

                  stream >> actual;
                  result.push_back(actual);
               }
            }
            else
            {
               ParamType newValue;
               stream.str(value);
               stream >> newValue;
               NamedGenericParameter<ParamType>::SetValue(newValue);
            }

            return true;
         }

      protected:
         virtual ~NamedPODParameter() {}
   };

   /**
    * @class NamedBooleanParameter
    */
   class DT_DAL_EXPORT NamedBooleanParameter: public NamedPODParameter<bool>
   {
      public:
         NamedBooleanParameter(const dtUtil::RefString& name, bool defaultValue = false,
            bool isList = false);

         ///overridden to return "true" and "false"
         virtual const std::string ToString() const;

         ///overridden to accept "true" and other such strings as well as numbers.
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedBooleanParameter();
   };

   /**
     * @class UnsignedCharMessageParameter
     */
   class DT_DAL_EXPORT NamedUnsignedCharParameter: public NamedPODParameter<unsigned char>
   {
      public:
         NamedUnsignedCharParameter(const dtUtil::RefString& name,
             unsigned char defaultValue = 0, bool isList = false);
      protected:
         virtual ~NamedUnsignedCharParameter();
   };

   /**
     * @class ShortIntMessageParameter
     */
   class DT_DAL_EXPORT NamedShortIntParameter: public NamedPODParameter<short>
   {
      public:
         NamedShortIntParameter(const dtUtil::RefString& name, short defaultValue = 0, bool isList = false);

      protected:
         virtual ~NamedShortIntParameter();
   };

   /**
     * @class UnsignedShortIntMessageParameter
     */
   class DT_DAL_EXPORT NamedUnsignedShortIntParameter: public NamedPODParameter<unsigned short>
   {
      public:
         NamedUnsignedShortIntParameter(const dtUtil::RefString& name,
            unsigned short defaultValue = 0, bool isList = false);
      protected:
         virtual ~NamedUnsignedShortIntParameter();
   };

   /**
     * @class UnsignedIntMessageParameter
     */
   class DT_DAL_EXPORT NamedUnsignedIntParameter: public NamedPODParameter<unsigned int>
   {
      public:
         NamedUnsignedIntParameter(const dtUtil::RefString& name, unsigned int defaultValue = 0, bool isList = false);

      protected:
         virtual ~NamedUnsignedIntParameter();
   };

   /**
     * @class IntMessageParameter
     */
   class DT_DAL_EXPORT NamedIntParameter: public NamedPODParameter<int>
   {
      public:
         NamedIntParameter(const dtUtil::RefString& name, int defaultValue = 0, bool isList = false);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedIntParameter();
   };

   /**
    * @class UnsignedLongIntMessageParameter
    */
   class DT_DAL_EXPORT NamedUnsignedLongIntParameter: public NamedPODParameter<unsigned long>
   {
      public:
         NamedUnsignedLongIntParameter(const dtUtil::RefString& name,
            unsigned long defaultValue = 0, bool isList = false);

      protected:
         virtual ~NamedUnsignedLongIntParameter();
   };

   /**
     * @class LongIntMessageParameter
     */
   class DT_DAL_EXPORT NamedLongIntParameter: public NamedPODParameter<long>
   {
      public:
         NamedLongIntParameter(const dtUtil::RefString& name, long defaultValue = 0, bool isList = false);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedLongIntParameter();
   };

   /**
     * @class FloatMessageParameter
     */
   class DT_DAL_EXPORT NamedFloatParameter: public NamedPODParameter<float>
   {
      public:
         NamedFloatParameter(const dtUtil::RefString& name, float defaultValue = 0.0f, bool isList = false);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedFloatParameter();
   };

   /**
     * @class DoubleMessageParameter
     */
   class DT_DAL_EXPORT NamedDoubleParameter: public NamedPODParameter<double>
   {
      public:
         NamedDoubleParameter(const dtUtil::RefString& name, double defaultValue = 0.0, bool isList = false);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedDoubleParameter();
   };

   /**
     * @class StringMessageParameter
     */
   class DT_DAL_EXPORT NamedStringParameter: public NamedGenericParameter<std::string>
   {
      public:
         NamedStringParameter(const dtUtil::RefString& name, const std::string& defaultValue = "",
             bool isList=false);

         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         NamedStringParameter(DataType& dataType, const dtUtil::RefString& name, const std::string& defaultValue = "",
             bool isList = false);
         virtual ~NamedStringParameter();
   };

   /**
   * @class ArrayMessageParameter
   */
   class DT_DAL_EXPORT NamedArrayParameter: public NamedGenericParameter<std::string>
   {
      public:
         NamedArrayParameter(const dtUtil::RefString& name);

         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         NamedArrayParameter(DataType& dataType, const dtUtil::RefString& name);
         virtual ~NamedArrayParameter();
   };

   /**
   * @class ContainerMessageParameter
   */
   class DT_DAL_EXPORT NamedContainerParameter: public NamedGenericParameter<std::string>
   {
   public:
      NamedContainerParameter(const dtUtil::RefString& name);

      virtual const std::string ToString() const;
      virtual bool FromString(const std::string& value);

      virtual void SetFromProperty(const dtDAL::ActorProperty& property);
      virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

   protected:
      NamedContainerParameter(DataType& dataType, const dtUtil::RefString& name);
      virtual ~NamedContainerParameter();
   };

   /**
     * @class EnumMessageParameter
     * The EnumMessageParameter works like the string message parameter because there is no way for the message to
     * efficiently know how to fetch proper enumeration object when the data is populated from a string or stream.
     */
   class DT_DAL_EXPORT NamedEnumParameter: public NamedStringParameter
   {
      public:
         NamedEnumParameter(const dtUtil::RefString& name, const std::string& defaultValue = "", bool isList=false);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedEnumParameter();
   };

   /**
     * @class ActorMessageParameter
     * The ActorMessageParameter stores a unique id that can be used to retrieve the necessary
     * GameActorProxy from the game manager.
    */
   class DT_DAL_EXPORT NamedActorParameter: public NamedGenericParameter<dtCore::UniqueId>
   {
      public:
         NamedActorParameter(const dtUtil::RefString& name,
          const dtCore::UniqueId& defaultValue = dtCore::UniqueId(""), bool isList = false);

         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedActorParameter();
   };

   /**
    * @class GameEventMessageParameter
    * The GameEventMessageParameter stores a unique id that can be used to get the associated
    * game event from the EventManager.
    * @see #dtDAL::GameEventManager
    */
   class DT_DAL_EXPORT NamedGameEventParameter : public NamedGenericParameter<dtCore::UniqueId>
   {
      public:
         NamedGameEventParameter(const dtUtil::RefString& name,
                                   const dtCore::UniqueId& defaultValue = dtCore::UniqueId(""),
                                   bool isList = false);
         virtual const std::string ToString() const;
         virtual bool FromString(const std::string& value);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedGameEventParameter();
   };

   /**
    * @class NamedVecParameter
    * This is a template for the vector types to make reading data from a string easier.
    */
   template <class ParamType>
   class NamedVecParameter: public NamedGenericParameter<ParamType>
   {
      public:
         virtual const std::string ToString() const
         {
            std::ostringstream ss;
            ss.precision(NamedGenericParameter<ParamType>::GetNumberPrecision());

            if (NamedGenericParameter<ParamType>::IsList())
            {
               const std::vector<ParamType>& values =
                  NamedGenericParameter<ParamType>::GetValueList();

               ss << values[0];
               for (unsigned int i = 1; i < values.size(); ++i)
               {
                  ss << NamedGenericParameter<ParamType>::GetParamDelimeter() <<
                     values[i];
               }
               return ss.str();
            }
            else
            {
               ss << NamedGenericParameter<ParamType>::GetValue();
               return ss.str();
            }
         }

      protected:
         NamedVecParameter(DataType& dataType, const dtUtil::RefString& name, const ParamType& defaultValue,
            bool isList = false) : NamedGenericParameter<ParamType>(dataType, name, defaultValue, isList)
         {
               NamedGenericParameter<ParamType>::SetNumberPrecision(2 * sizeof(defaultValue[0]) + 1);
         }
         virtual ~NamedVecParameter() {}

         bool InternalFromString(const std::string& value, unsigned size)
         {
            if (NamedGenericParameter<ParamType>::IsList())
            {
               std::vector<ParamType>& result =
                  NamedGenericParameter<ParamType>::GetValueList();

               std::vector<std::string> stringList;
               dtUtil::IsDelimeter delimCheck(NamedGenericParameter<ParamType>::GetParamDelimeter());
               dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(stringList, value, delimCheck);

               result.clear();
               for (unsigned int i = 0; i < stringList.size(); ++i)
               {
                  ParamType v;
                  if (!dtUtil::ParseVec<ParamType>(stringList[i], v, size))
                  {
                     return false;
                  }
                  result.push_back(v);
               }

               return true;
            }
            else
            {
               ParamType newValue;
               if (dtUtil::ParseVec<ParamType>(value, newValue, size))
               {
                  NamedGenericParameter<ParamType>::SetValue(newValue);
                  return true;
               }
               else
               {
                  return false;
               }
            }
         }
   };

   /**
    * @class Vec2MessageParameter
    */
   class DT_DAL_EXPORT NamedVec2Parameter: public NamedVecParameter<osg::Vec2>
   {
      public:
         NamedVec2Parameter(const dtUtil::RefString& name,
             const osg::Vec2& defaultValue = osg::Vec2(0.0,0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 2); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec2Parameter();
   };

   /**
     * @class Vec2fMessageParameter
     */
   class DT_DAL_EXPORT NamedVec2fParameter: public NamedVecParameter<osg::Vec2f>
   {
      public:
         NamedVec2fParameter(const dtUtil::RefString& name,
             const osg::Vec2f& defaultValue = osg::Vec2f(0.0,0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 2); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec2fParameter();
   };

   /**
     * @class Vec2dMessageParameter
     */
   class DT_DAL_EXPORT NamedVec2dParameter: public NamedVecParameter<osg::Vec2d>
   {
      public:
         NamedVec2dParameter(const dtUtil::RefString& name,
               const osg::Vec2d& defaultValue = osg::Vec2d(0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 2); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec2dParameter();
   };

   /**
     * @class Vec3MessageParameter
     */
   class DT_DAL_EXPORT NamedVec3Parameter: public NamedVecParameter<osg::Vec3>
   {
      public:
         NamedVec3Parameter(const dtUtil::RefString& name,
               const osg::Vec3& defaultValue = osg::Vec3(0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 3); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         NamedVec3Parameter(DataType& dataType, const dtUtil::RefString& name,
               const osg::Vec3& defaultValue = osg::Vec3(0.0, 0.0, 0.0), bool isList = false);
         virtual ~NamedVec3Parameter();
   };

   /**
     * @class RGBColorMessageParameter
     */
   class DT_DAL_EXPORT NamedRGBColorParameter: public NamedVec3Parameter
   {
      public:
         NamedRGBColorParameter(const dtUtil::RefString& name,
             const osg::Vec3& defaultValue = osg::Vec3(0.0, 0.0, 0.0), bool isList = false);

      protected:
         virtual ~NamedRGBColorParameter();
   };


   /**
     * @class Vec3fMessageParameter
     */
   class DT_DAL_EXPORT NamedVec3fParameter: public NamedVecParameter<osg::Vec3f>
   {
      public:
         NamedVec3fParameter(const dtUtil::RefString& name,
               const osg::Vec3f& defaultValue = osg::Vec3f(0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 3); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec3fParameter();
   };

   /**
     * @class Vec3dMessageParameter
     */
   class DT_DAL_EXPORT NamedVec3dParameter: public NamedVecParameter<osg::Vec3d>
   {
      public:
         NamedVec3dParameter(const dtUtil::RefString& name,
             const osg::Vec3d& defaultValue = osg::Vec3d(0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 3); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec3dParameter();
   };

   /**
     * @class Vec4MessageParameter
     */
   class DT_DAL_EXPORT NamedVec4Parameter: public NamedVecParameter<osg::Vec4>
   {
      public:
         NamedVec4Parameter(const dtUtil::RefString& name,
             const osg::Vec4& defaultValue = osg::Vec4(0.0, 0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 4); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         NamedVec4Parameter(DataType& dataType, const dtUtil::RefString& name,
             const osg::Vec4& defaultValue = osg::Vec4(0.0, 0.0, 0.0, 0.0), bool isList = false);
         virtual ~NamedVec4Parameter();
   };

   /**
     * @class RGBAColorMessageParameter
     */
   class DT_DAL_EXPORT NamedRGBAColorParameter: public NamedVec4Parameter
   {
      public:
         NamedRGBAColorParameter(const dtUtil::RefString& name,
             const osg::Vec4& defaultValue = osg::Vec4(0.0, 0.0, 0.0, 0.0), bool isList = false);

      protected:
         virtual ~NamedRGBAColorParameter();
   };

   /**
    * @class Vec4fMessageParameter
    */
   class DT_DAL_EXPORT NamedVec4fParameter: public NamedVecParameter<osg::Vec4f>
   {
      public:
         NamedVec4fParameter(const dtUtil::RefString& name,
          const osg::Vec4f& defaultValue = osg::Vec4f(0.0, 0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value)  {  return InternalFromString(value, 4); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec4fParameter();
   };

   /**
    * @class Vec4dMessageParameter
    */
   class DT_DAL_EXPORT NamedVec4dParameter: public NamedVecParameter<osg::Vec4d>
   {
      public:
         NamedVec4dParameter(const dtUtil::RefString& name,
            const osg::Vec4d& defaultValue = osg::Vec4d(0.0, 0.0, 0.0, 0.0), bool isList = false);

         virtual bool FromString(const std::string& value) { return InternalFromString(value, 4); }

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

      protected:
         virtual ~NamedVec4dParameter();
   };

   /**
    * @class ResourceMessageParameter
    */
   class DT_DAL_EXPORT NamedResourceParameter: public NamedParameter
   {
      public:
         NamedResourceParameter(dtDAL::DataType& type, const dtUtil::RefString& name,
            bool isList = false);

         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         virtual bool FromDataStream(dtUtil::DataStream& stream);

         virtual const std::string ToString() const;

         virtual bool FromString(const std::string& value);

         virtual void CopyFrom(const NamedParameter& otherParam);

         /**
          * Sets the resource descriptor.  This value copied if not null.
          * You should create one on the stack, and then pass it with &.
          * The method takes NULL so that the parameter can have an empty value.
          */
         void SetValue(const dtDAL::ResourceDescriptor* descriptor);

         /**
          * This returns a pointer to a stack created data member, so you should copy it
          * via copy constructor and hold onto it that way.  If you hold onto the
          * point, you could get deleted out from under you.
          * @return the resource descriptor or NULL.
          */
         const dtDAL::ResourceDescriptor* GetValue() const;

         const std::vector<dtDAL::ResourceDescriptor>& GetValueList() const;

         std::vector<dtDAL::ResourceDescriptor>& GetValueList();

         void SetValueList(const std::vector<dtDAL::ResourceDescriptor>& newValues);

         virtual void SetFromProperty(const dtDAL::ActorProperty& property);
         virtual void ApplyValueToProperty(dtDAL::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const;

      protected:
         virtual ~NamedResourceParameter();

      private:
         const dtDAL::DataType* mDataType;
         std::vector<dtDAL::ResourceDescriptor>* mValueList;
         dtDAL::ResourceDescriptor mDescriptor;
   };


}

#endif
