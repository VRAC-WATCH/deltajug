/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 04/20/2007
 */

#ifndef __DELTA_ANIMATABLE_H__
#define __DELTA_ANIMATABLE_H__

#include <dtAnim/export.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <string>

namespace dtAnim
{
   class Cal3DModelWrapper;
   class Animatable;

   typedef dtUtil::Functor<void, TYPELIST_1(const dtAnim::Animatable&) > AnimationCallback;

   /**
    * This class is used to specify the base class of an object which has semantics for
    * animating.
    */
   class DT_ANIM_EXPORT Animatable: public osg::Referenced
   {

      public:
         Animatable();

         Animatable(const Animatable& pAnim);
         Animatable& operator=(const Animatable& pAnim);

      protected:
         virtual ~Animatable();

      public:

         /**
          * The start time is the time in seconds this animation will start playing
          * after it was added to an AnimationSequence.
          */
         float GetStartTime() const;

         /**
          * The start delay specifies how long in seconds this animation will delay once
          * its parent has started playing.
          */
         void SetStartDelay(float t);
         float GetStartDelay() const;

         /**
          * The end time is the time in seconds that this animation will start
          * fading out relative to when it was added to an AnimationSequence.
          */
         float GetEndTime() const;

         /**
          * The elapsed time is the time in seconds since this animation has been
          * added to the play list.
          */
         float GetElapsedTime() const;

         /**
          * The FadeIn time, is the amount of time takes for an animation to blend
          * linearally from a weight of 0 to the weight specified by BaseWeight
          * after the animation starts playing specified by StartTime.
          */
         float GetFadeIn() const;
         void SetFadeIn(float f);

         /**
          * The FadeOut time is the amount of time it will take for an animation to
          * blend linearly from its BaseWeight to 0 after the EndTime.
          */
         float GetFadeOut() const;
         void SetFadeOut(float f);

         /**
          * The BaseWeight specifies the weight of this animation without
          * being affected by blending.
          */
         float GetBaseWeight() const;
         void SetBaseWeight(float f);

         /**
          * The CurrentWeight is the weight of this animation in the current frame
          * this is calculated from the BaseWeight, linear blending from fades, and the parent weight
          */
         float GetCurrentWeight() const;

         /**
          * An animation is active if it is currently playing.
          */
         bool IsActive() const;

         /**
          * The speed of an animation is the percentage relative to the actual speed
          * of playback.  It defaults to 1.0, a speed of 2.0 would play twice as fast.
          */
         float GetSpeed() const;
         void SetSpeed(float speed);

         /**
          * This flag specifies whether or not this animation has stopped playing
          */
         bool ShouldPrune() const;


         /**
          * This virtual function is called before this animation is removed from
          * the system
          */
         virtual void Prune() = 0;

         /**
          * This function is used to copy Animatables
          */
         virtual dtCore::RefPtr<Animatable> Clone(Cal3DModelWrapper* modelWrapper) const = 0;


         /**
          * The virtual update, should be called every frame
          *
          * @param delta time
          */
         virtual void Update(float dt) = 0;

         /**
          * ForceFadeOut will ignore the EndTime and automatically fade out
          * this animation over the time specified.
          *
          * @param the time to fade out over
          */
         virtual void ForceFadeOut(float time) = 0;

         /**
          * @return the name of this animation
          */
         const std::string& GetName() const;

         /**
          * @param the name to set this animation to
          */
         void SetName(const std::string&);


         /**
          * These functions are only to be used by AnimationController
          * on Update() and Recalculate
          */
         void SetStartTime(float t);
         void SetCurrentWeight(float weight);
         void SetElapsedTime(float t);
         virtual void Recalculate() = 0;

         /**
          * Set the functor to be called when the animation ends.
          * @param callback The functor to be called when this animation ends.
          *        The Animatable will be passed into the callback so that
          *        the callee can determine what animation has ended.
          */
         void SetEndCallback(AnimationCallback callback);

      protected:

         /**
          * When this flag is set the parent sequence will call Prune() at the end of its update
          * and then delete this animation.
          */
         void SetPrune(bool b);

         void SetEndTime(float t);
         void SetActive(bool b);

      private:


         // user editable fields are: fade in, fade out, base weight, and speed

         float mSpeed;
         float mStartTime, mStartDelay, mEndTime;
         float mFadeIn, mFadeOut;
         float mElapsedTime;
         float mBaseWeight, mCurrentWeight;

         std::string mName;

         bool mActive, mShouldPrune;
         
         AnimationCallback mEndCallback;

   };

} // namespace dtAnim

#endif // __DELTA_ANIMATABLE_H__

