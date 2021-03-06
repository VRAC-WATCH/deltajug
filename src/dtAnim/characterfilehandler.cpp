/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 * Erik Johnson
 * Bradley G Anderegg
 */

#include <dtAnim/characterfilehandler.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>


XERCES_CPP_NAMESPACE_USE;


namespace dtAnim
{

const std::string CharacterFileHandler::CHARACTER_XML_LOGGER("characterfilehandler.cpp");

const std::string CharacterFileHandler::CHARACTER_ELEMENT("character");
const std::string CharacterFileHandler::ANIMATION_ELEMENT("animation");
const std::string CharacterFileHandler::MORPH_ANIMATION_ELEMENT("morph");
const std::string CharacterFileHandler::SKELETON_ELEMENT("skeleton");
const std::string CharacterFileHandler::MESH_ELEMENT("mesh");
const std::string CharacterFileHandler::MATERIAL_ELEMENT("material");
const std::string CharacterFileHandler::CHANNEL_ELEMENT("animationChannel");
const std::string CharacterFileHandler::SEQUENCE_ELEMENT("animationSequence");
const std::string CharacterFileHandler::POSEMESH_ELEMENT("poseMesh");
const std::string CharacterFileHandler::NAME_ELEMENT("name");
const std::string CharacterFileHandler::FILENAME_ELEMENT("fileName");
const std::string CharacterFileHandler::ANIMATION_NAME_ELEMENT("animationName");

const std::string CharacterFileHandler::START_DELAY_ELEMENT("startDelay");
const std::string CharacterFileHandler::FADE_IN_ELEMENT("fadeIn");
const std::string CharacterFileHandler::FADE_OUT_ELEMENT("fadeOut");
const std::string CharacterFileHandler::SPEED_ELEMENT("speed");
const std::string CharacterFileHandler::BASE_WEIGHT_ELEMENT("baseWeight");
const std::string CharacterFileHandler::MAX_DURATION_ELEMENT("maxDuration");
const std::string CharacterFileHandler::IS_LOOPING_ELEMENT("isLooping");
const std::string CharacterFileHandler::IS_ACTION_ELEMENT("isAction");
const std::string CharacterFileHandler::CHILD_ELEMENT("child");

const std::string CharacterFileHandler::SKINNING_SHADER_ELEMENT("skinningShader");
const std::string CharacterFileHandler::SHADER_GROUP_ELEMENT("shaderGroup");
const std::string CharacterFileHandler::SHADER_NAME_ELEMENT("shaderName");
const std::string CharacterFileHandler::SHADER_MAX_BONES_ELEMENT("maxBones");

const std::string CharacterFileHandler::LOD_ELEMENT("LOD");
const std::string CharacterFileHandler::LOD_START_DISTANCE_ELEMENT("lodStartDistance");
const std::string CharacterFileHandler::LOD_END_DISTANCE_ELEMENT("lodEndDistance");
const std::string CharacterFileHandler::MAX_VISIBLE_DISTANCE_ELEMENT("maxVisibleDistance");

const std::string CharacterFileHandler::SCALE_ELEMENT("scale");
const std::string CharacterFileHandler::SCALE_FACTOR_ELEMENT("scalingFactor");

////////////////////////////////////////////////////////////////////////////////
CharacterFileHandler::AnimatableStruct::AnimatableStruct()
   : mStartDelay(0.0f)
   , mFadeIn(0.0f)
   , mFadeOut(0.0f)
   , mSpeed(0.0f)
   , mBaseWeight(0.0f)
{
}

////////////////////////////////////////////////////////////////////////////////
CharacterFileHandler::AnimationChannelStruct::AnimationChannelStruct()
   : mMaxDuration(0.0f)
   , mIsLooping(false)
   , mIsAction(false)
{
}

////////////////////////////////////////////////////////////////////////////////
CharacterFileHandler::AnimationSequenceStruct::AnimationSequenceStruct()
{
}

////////////////////////////////////////////////////////////////////////////////
CharacterFileHandler::CharacterFileHandler()
   : mName()
   , mAnimations()
   , mMorphAnimations()
   , mMaterials()
   , mMeshes()
   , mShaderMaxBones(72)
   , mLODStartDistance(10.0)
   , mLODEndDistance(500.0)
   , mLODMaxVisibleDistance(1000.0)
   , mFoundLODOptions(false)
   , mFoundScale(false)
   , mScale(1.0f)
   , mAnimationChannels()
   , mSkeletonFilename()
   , mInSkinningShader(false)
   , mInLOD(false)
   , mInScale(false)
   , mInChannel(false)
   , mInSequence(false)
   , mLogger(NULL)
{
   mLogger = &dtUtil::Log::GetInstance(CHARACTER_XML_LOGGER);
}

////////////////////////////////////////////////////////////////////////////////
CharacterFileHandler::~CharacterFileHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::startDocument()
{
   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Begin Parsing File");
   }
   mName.clear();
   mAnimations.clear();
   mMorphAnimations.clear();
   mMaterials.clear();
   mMeshes.clear();
   mShaderGroup.clear();
   mShaderName.clear();
   mAnimationChannels.clear();
   mAnimationSequences.clear();
   mSkeletonFilename.clear();

   mFoundLODOptions = false;
   mFoundScale = false;

   mInSkinningShader = false;
   mInLOD = false;
   mInScale = false;
   mInChannel = false;
   mInSequence = false;
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::endDocument()
{
   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "End Parsing File");
   }
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                                 const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;
   results = search(attrs);

   std::string errorString;

   if (elementStr == CHARACTER_ELEMENT)
   {
      resultIter = results.find(NAME_ELEMENT);

      if (resultIter != results.end())
      {
          mName = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }
   }
   else if (elementStr == SKELETON_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         mSkeletonFilename = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <character> missing <name> child");
      }
   }
   else if (elementStr == POSEMESH_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         mPoseMeshFilename = resultIter->second;
      }
      else
      {
         errorString = std::string("Invalid XML format: <poseMesh> missing 'fileName' attribute");
      }
   }
   else if (elementStr == ANIMATION_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the name of the animation to be the filename
         std::string name = resultIter->second;

         resultIter = results.find(NAME_ELEMENT);
         if (resultIter != results.end() )
         {
            name = resultIter->second;
         }
         AnimationStruct anim;
         anim.mFileName = filename;
         anim.mName = name;
         mAnimations.push_back(anim);
      }
      else
      {
         errorString = std::string("Invalid XML format: <animation> missing <filename> child");
      }
   }
   else if (elementStr == MORPH_ANIMATION_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the name of the animation to be the filename
         std::string name = resultIter->second;
         resultIter = results.find(NAME_ELEMENT);
         if (resultIter != results.end() )
         {
            name = resultIter->second;
         }
         MorphAnimationStruct morph;
         morph.mFileName = filename;
         morph.mName = name;
         mMorphAnimations.push_back(morph);
      }
      else
      {
         errorString = std::string("Invalid XML format: <morph> missing <filename> child");
      }
   }
   else if (elementStr == MESH_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the mesh name to be the same as the filename
         std::string meshName = filename;

         resultIter = results.find(NAME_ELEMENT);
         if (resultIter != results.end() )
         {
            meshName = resultIter->second;
         }

         MeshStruct mesh;
         mesh.mFileName = filename;
         mesh.mName = meshName;

         mMeshes.push_back(mesh);
      }
      else
      {
         errorString = std::string("Invalid XML format: <mesh> missing <filename> child");
      }
   }
   else if (elementStr == MATERIAL_ELEMENT)
   {
      resultIter = results.find(FILENAME_ELEMENT);

      if (resultIter != results.end())
      {
         std::string filename = resultIter->second;

         //default the material name to be the same as the filename
         std::string matName = filename;

         resultIter = results.find(NAME_ELEMENT);
         if (resultIter != results.end() )
         {
            matName = resultIter->second;
         }

         MaterialStruct mat;
         mat.mFileName = filename;
         mat.mName = matName;

         mMaterials.push_back(mat);
      }
      else
      {
         errorString = std::string("Invalid XML format: <material> missing <filename> child");
      }
   }
   else if (elementStr == SKINNING_SHADER_ELEMENT)
   {
      mInSkinningShader = true;
   }
   else if (elementStr == LOD_ELEMENT)
   {
      mInLOD = true;
      mFoundLODOptions = true;
   }
   else if (elementStr == SCALE_ELEMENT)
   {
      mInScale = true;
      mFoundScale = true;
   }
   else if (elementStr == CHANNEL_ELEMENT)
   {
      mInChannel = true;
      mAnimationChannels.push_back(AnimationChannelStruct());
   }
   else if (elementStr == SEQUENCE_ELEMENT)
   {
      mInSequence = true;
      mAnimationSequences.push_back(AnimationSequenceStruct());
   }

   if (!errorString.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, errorString);
   }

   //push elemet
   mElements.push(elementStr);
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::endElement(const XMLCh* const uri,
                                      const XMLCh* const localname,
                                      const XMLCh* const qname)
{

   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Attempting to pop elements off of stack and the stack is empty."
                           "it should at least contain element %s.",
                           dtUtil::XMLStringConverter(localname).c_str());
      return;
   }

   const char* lname = mElements.top().c_str();

   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Ending element: \"%s\"", lname);
   }

   std::string elementStr = dtUtil::XMLStringConverter(localname).ToString();

   if (elementStr == SKINNING_SHADER_ELEMENT)
   {
      mInSkinningShader = false;
   }
   else if (elementStr == LOD_ELEMENT)
   {
      mInLOD = false;
   }
   else if (elementStr == SCALE_ELEMENT)
   {
      mInScale = false;
   }
   else if (elementStr == CHANNEL_ELEMENT)
   {
      mInChannel = false;
   }
   else if (elementStr == SEQUENCE_ELEMENT)
   {
      mInSequence = false;
   }

   mElements.pop();
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::characters(const XMLCh* const chars,
                                      const unsigned int length)
{
   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Characters should not be found outside of an element: \"%s\"",
                           dtUtil::XMLStringConverter(chars).c_str());
      return;
   }

   if (mInSkinningShader)
   {
      SkinningShaderCharacters(chars);
   }
   else if (mInLOD)
   {
      LODCharacters(chars);
   }
   else if (mInScale)
   {
      ScaleCharacters(chars);
   }
   else if (mInChannel)
   {
      AnimChannelCharacters(chars);
   }
   else if (mInSequence)
   {
      AnimSequenceCharacters(chars);
   }
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::SkinningShaderCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();
   if (topEl == SHADER_GROUP_ELEMENT)
   {
      mShaderGroup = dtUtil::XMLStringConverter(chars).ToString();
   }
   else if (topEl == SHADER_NAME_ELEMENT)
   {
      mShaderName = dtUtil::XMLStringConverter(chars).ToString();
   }
   else if (topEl == SHADER_MAX_BONES_ELEMENT)
   {
      mShaderMaxBones = dtUtil::ToUnsignedInt(dtUtil::XMLStringConverter(chars).ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::LODCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();

   double value = dtUtil::ToType<double>(dtUtil::XMLStringConverter(chars).ToString());

   if (topEl == LOD_START_DISTANCE_ELEMENT)
   {
      mLODStartDistance = value;
   }
   else if (topEl == LOD_END_DISTANCE_ELEMENT)
   {
      mLODEndDistance = value;
   }
   else if (topEl == MAX_VISIBLE_DISTANCE_ELEMENT)
   {
      mLODMaxVisibleDistance = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::ScaleCharacters(const XMLCh* const chars)
{
   mScale = dtUtil::ToType<float>(dtUtil::XMLStringConverter(chars).ToString());
   std::string& topEl = mElements.top();
   assert (topEl == SCALE_FACTOR_ELEMENT);
}

////////////////////////////////////////////////////////////////////////////////
bool CharacterFileHandler::AnimatableCharacters(const XMLCh* const chars, AnimatableStruct& animatable)
{
   bool result = true;

   std::string& topEl = mElements.top();

   if (topEl == NAME_ELEMENT)
   {
      animatable.mName = dtUtil::XMLStringConverter(chars).ToString();
   }
   else if (topEl == START_DELAY_ELEMENT)
   {
      std::string delay = dtUtil::XMLStringConverter(chars).ToString();
      animatable.mStartDelay = dtUtil::ToType<float>(delay);
   }
   else if (topEl == FADE_IN_ELEMENT)
   {
      std::string fade_in = dtUtil::XMLStringConverter(chars).ToString();
      animatable.mFadeIn = dtUtil::ToType<float>(fade_in);
   }
   else if (topEl == FADE_OUT_ELEMENT)
   {
      std::string fade_out = dtUtil::XMLStringConverter(chars).ToString();
      animatable.mFadeOut = dtUtil::ToType<float>(fade_out);
   }
   else if (topEl == SPEED_ELEMENT)
   {
      std::string speed = dtUtil::XMLStringConverter(chars).ToString();
      animatable.mSpeed = dtUtil::ToType<float>(speed);
   }
   else if (topEl == BASE_WEIGHT_ELEMENT)
   {
      std::string base_weight = dtUtil::XMLStringConverter(chars).ToString();
      animatable.mBaseWeight = dtUtil::ToType<float>(base_weight);
   }
   else
   {
      result = false;
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::AnimChannelCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();
   AnimationChannelStruct& pChannel = mAnimationChannels.back();

   if (!AnimatableCharacters(chars, pChannel))
   {
      if (topEl == ANIMATION_NAME_ELEMENT)
      {
         pChannel.mAnimationName = dtUtil::XMLStringConverter(chars).ToString();
      }
      else if (topEl == MAX_DURATION_ELEMENT)
      {
         std::string max_duration = dtUtil::XMLStringConverter(chars).ToString();
         pChannel.mMaxDuration = dtUtil::ToType<float>(max_duration);
      }
      else if (topEl == IS_LOOPING_ELEMENT)
      {
         std::string is_looping = dtUtil::XMLStringConverter(chars).ToString();
         pChannel.mIsLooping = dtUtil::ToType<bool>(is_looping);
      }
      else if (topEl == IS_ACTION_ELEMENT)
      {
         std::string is_action = dtUtil::XMLStringConverter(chars).ToString();
         pChannel.mIsAction = dtUtil::ToType<bool>(is_action);
      }
      else if (topEl != CHANNEL_ELEMENT)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                              "Found characters for unknown element \"%s\" \"%s\"",
                              topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Found characters for element \"%s\" \"%s\"", topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void CharacterFileHandler::AnimSequenceCharacters(const XMLCh* const chars)
{
   std::string& topEl = mElements.top();
   AnimationSequenceStruct& pSequence = mAnimationSequences.back();

   if (mElements.empty())
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                           "Attempting to pop elements off of stack and the stack is empty."
                           "it should at least contain element %s.",
                           dtUtil::XMLStringConverter(chars).c_str());
      return;
   }

   if (!AnimatableCharacters(chars, pSequence))
   {
      if (topEl == CHILD_ELEMENT)
      {
         std::string childName = dtUtil::XMLStringConverter(chars).ToString();
         pSequence.mChildNames.push_back(childName);
      }
      else if (topEl != SEQUENCE_ELEMENT)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                              "Found characters for unknown element \"%s\" \"%s\"",
                              topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
                           "Found characters for element \"%s\" \"%s\"",
                           topEl.c_str(), dtUtil::XMLStringConverter(chars).c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace dtAnim
