/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* David Guthrie
* William Johnson
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtCore/refptr.h>
#include <dtCore/globals.h>

#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/datatype.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/gameeventmanager.h>
#include <dtDAL/project.h>

#include <dtActors/engineactorregistry.h>

#include <dtUtil/log.h>

#include <iostream>

using dtDAL::DataType;
using dtCore::RefPtr;

class ProxyTest : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ProxyTest);
   CPPUNIT_TEST(TestProxies);
   CPPUNIT_TEST(TestBezierProxies);
   CPPUNIT_TEST_SUITE_END();
   
   public:
         
      ProxyTest();
      virtual ~ProxyTest();
      
      virtual void setUp();
      virtual void tearDown();
      void TestProxies();
      void TestBezierProxies();
      
   private:
      dtDAL::LibraryManager &libMgr;
      std::vector<const dtDAL::ActorType*> actors;
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
      static const std::string mExampleLibraryName;
      
      void testProps(dtDAL::ActorProxy& proxy);
      void testProp(dtDAL::ActorProxy& proxy, dtDAL::ActorProperty* prop, bool isElement = false);
      void compareProxies(dtDAL::ActorProxy& ap1, dtDAL::ActorProxy& ap2);
      void compareProperties(dtDAL::ActorProperty* prop1, dtDAL::ActorProperty* prop2);
};

CPPUNIT_TEST_SUITE_REGISTRATION(ProxyTest);

const std::string ProxyTest::mExampleLibraryName="testActorLibrary";

ProxyTest::ProxyTest() : libMgr(dtDAL::LibraryManager::GetInstance())
{
}

ProxyTest::~ProxyTest()
{
}

void ProxyTest::setUp()
{
   //dtUtil::Log::GetInstance("librarymanager.cpp").SetLogLevel(dtUtil::Log::LOG_DEBUG);
   libMgr.LoadActorRegistry(mExampleLibraryName);
   libMgr.GetActorTypes(actors);
   CPPUNIT_ASSERT(actors.size() > 0);
}

void ProxyTest::tearDown()
{
   //dtUtil::Log::GetInstance("librarymanager.cpp").SetLogLevel(dtUtil::Log::LOG_WARNING);
   proxies.clear();
   actors.clear();
   libMgr.UnloadActorRegistry(mExampleLibraryName);
   dtDAL::GameEventManager::GetInstance().ClearAllEvents();
}

void ProxyTest::testProps(dtDAL::ActorProxy& proxy)
{
   std::vector<dtDAL::ActorProperty*> props;
   proxy.GetPropertyList(props);
   
   for (unsigned int i = 0; i < props.size(); i++)
   {
      props[i]->SetNumberPrecision(24);
      CPPUNIT_ASSERT(props[i]->GetNumberPrecision() == 24);
      props[i]->SetNumberPrecision(16);
      CPPUNIT_ASSERT(props[i]->GetNumberPrecision() == 16);

      testProp(proxy, props[i]);
   }
}

static void SimpleStringToFromDataStreamCheck(dtUtil::DataStream& ds, dtDAL::ActorProperty& p)
{
   if (p.IsReadOnly())
   {
      return;
   }

   ds.Rewind();
   p.ToDataStream(ds);

   std::string expected = p.ToString();

   ds.Rewind();
   try
   {
      CPPUNIT_ASSERT_MESSAGE("From datastream should succeed on non-readonly properties: " + p.GetName() + " " + p.GetDataType().GetName(),
               p.FromDataStream(ds));
   }
   catch (...)
   {
      throw;
   }
   std::string actual = p.ToString();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("To and from datastream should be lossless, so ToString should return the same value"
            " both times", expected, actual);
}

void ProxyTest::testProp(dtDAL::ActorProxy& proxy, dtDAL::ActorProperty* prop, bool isElement)
{
   std::string proxyTypeName = proxy.GetActorType().GetName();
   std::string name = prop->GetName();
   const float epsilon = 0.0001f;

   static char* buffer = new char[512];
   dtUtil::DataStream ds(buffer, 512, false);

   if(prop->IsReadOnly())
   {
      // Test and make sure you can't set the property
      dtDAL::ActorProperty* p = prop;
      const std::string& str = p->ToString();
      bool shouldBeFalse = p->FromString(str);
      CPPUNIT_ASSERT_MESSAGE("Should not have been able to set the string value on an read only property", !shouldBeFalse);

      ds.Rewind();
      CPPUNIT_ASSERT_MESSAGE("From datastream should fail on readonly properties", !p->FromDataStream(ds));
      return;
   }
   
   if (prop->GetDataType() == DataType::FLOAT)
   {
      dtDAL::FloatActorProperty* prop1 = NULL;

      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::FloatActorProperty*>(prop);
      }
      else
      {
         /// Call the GetProperty method so that we can test every type with the templated getter.
         proxy.GetProperty(prop->GetName(), prop1);
      }

      float value1 = 0.3323233f;
      prop1->SetValue(value1);
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 0.3323233f, but it is: " + prop->ToString(),
                             osg::equivalent(prop1->GetValue(), value1, float(epsilon)));
      
      std::string stringValue = prop1->ToString();
      //set some other value so we can test the string can set it back.
      float value2 = 17.3238392f;
      prop1->SetValue(value2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 1.3323233f, but it is: " + prop->ToString(),
                             osg::equivalent(prop1->GetValue(), value1, float(epsilon)));
   }
   else if (prop->GetDataType() == DataType::DOUBLE)
   {
      dtDAL::DoubleActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::DoubleActorProperty*>(prop);
      }
      else
      {
         /// Call the GetProperty method so that we can test every type with the templated getter.
         proxy.GetProperty(prop->GetName(), prop1);
      }

      double value1 = 1.3323233;
      prop1->SetValue(value1);
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 1.3323233, but it is: " + prop->ToString(),
                             osg::equivalent(prop1->GetValue(), value1, double(epsilon)));
      
      std::string stringValue = prop1->ToString();
      //set some other value so we can test the string can set it back.
      double value2 = 17.3238392;
      prop1->SetValue(value2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 1.3323233, but it is: " + prop->ToString(),
                             osg::equivalent(prop1->GetValue(), value1, double(epsilon)));
   }
   else if (prop->GetDataType() == DataType::INT)
   {
      SimpleStringToFromDataStreamCheck(ds, *prop);
      dtDAL::IntActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::IntActorProperty*>(prop);
      }
      else
      {
         /// Call the GetProperty method so that we can test every type with the templated getter.
         proxy.GetProperty(prop->GetName(), prop1);
      }

      prop1->SetValue(3);
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 3, but it is: " + prop1->ToString(),
                             prop1->GetValue() == 3);
   }
   else if (prop->GetDataType() == DataType::LONGINT)
   {
      SimpleStringToFromDataStreamCheck(ds, *prop);

      dtDAL::LongActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::LongActorProperty*>(prop);
      }
      else
      {
         /// Call the GetProperty method so that we can test every type with the templated getter.
         proxy.GetProperty(prop->GetName(), prop1);
      }

      prop1->SetValue(3);
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value 3, but it is: " + prop1->ToString(),
                             prop1->GetValue() == 3);
   }
   else if (prop->GetDataType() == DataType::STRING)
   {
      SimpleStringToFromDataStreamCheck(ds, *prop);

      // This one property is in UTC format for time. This test would fail
      if(prop->GetName() == "Time and Date")
      {
         return;
      }
      else if(prop->GetName() == "ShaderGroup")
      {
         return;
      }
      dtDAL::StringActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::StringActorProperty*>(prop);
      }
      else
      {
         /// Call the GetProperty method so that we can test every type with the templated getter.
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      prop1->SetValue("cache");
      CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                             + " should have value \"cache\", but it is: " + prop1->ToString(),
                             prop1->GetValue() == "cache");
   }
   else if (prop->GetDataType() == DataType::BOOLEAN)
   {
      SimpleStringToFromDataStreamCheck(ds, *prop);

      //there have been some problems with Normal Rescaling
      if (name != "Normal Rescaling" && name != "Collision Geometry")
      {
         dtDAL::BooleanActorProperty* boolProp = NULL;
         if (isElement)
         {
            boolProp = dynamic_cast<dtDAL::BooleanActorProperty*>(prop);
         }
         else
         {
            proxy.GetProperty(prop->GetName(), boolProp);
         }

         boolProp->SetValue(false);
         CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxy.GetName() + " should be false, but it's not." ,
                                !boolProp->GetValue());
         boolProp->SetValue(true);
         CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxy.GetName() + " should be true, but it's not." ,
                                boolProp->GetValue());
      }
   }
   else if (prop->GetDataType() == DataType::ENUMERATION)
   {
      SimpleStringToFromDataStreamCheck(ds, *prop);

      dtDAL::AbstractEnumActorProperty* eap = NULL;
      if (isElement)
      {
         eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), eap);
      }

      CPPUNIT_ASSERT(eap != NULL);

      //set it to second value because it's less likely to the be second than the first.
      eap->SetEnumValue(const_cast<dtUtil::Enumeration&>(**(eap->GetList().begin()+1)));

      CPPUNIT_ASSERT_MESSAGE(std::string("Value should be ") + (*(eap->GetList().begin()+1))->GetName()
                             + " but it is " + eap->GetEnumValue().GetName(),
                             eap->GetEnumValue() == **(eap->GetList().begin()+1));

      //set it back to the first value to make sure it really got set.
      eap->SetEnumValue(const_cast<dtUtil::Enumeration&>(**(eap->GetList().begin())));

      CPPUNIT_ASSERT_MESSAGE(std::string("Value should be ") + (*(eap->GetList().begin()))->GetName()
                             + " but it is " + eap->GetEnumValue().GetName(),
                             eap->GetEnumValue() == **(eap->GetList().begin()));
   }
   else if (prop->GetDataType() == DataType::ACTOR)
   {
      dtDAL::ActorActorProperty* aap = NULL;
      dtDAL::ActorIDActorProperty* aidap = NULL;
      if (isElement)
      {
         aap = dynamic_cast<dtDAL::ActorActorProperty*>(prop);
         if (aap == NULL)
         {
            aidap = dynamic_cast<dtDAL::ActorIDActorProperty*>(prop);
            CPPUNIT_ASSERT(aidap != NULL);
         }

      }
      else
      {
         proxy.GetProperty(prop->GetName(), aap);
      }

      if (aap != NULL)
      {
         CPPUNIT_ASSERT_MESSAGE("The ActorActorProperty should not be NULL", aap);

         aap->SetValue(NULL);

         CPPUNIT_ASSERT_MESSAGE(std::string("Value should be NULL, but it's not"), aap->GetValue() == NULL);

         const std::string &actorClass = aap->GetDesiredActorClass();
         dtDAL::ActorProxy *tempProxy  = NULL;

         for(unsigned int i = 0; i < proxies.size(); ++i)
         {
            if(proxies[i]->IsInstanceOf(actorClass))
            {
               tempProxy = proxies[i].get();
               break;
            }
         }

         CPPUNIT_ASSERT_MESSAGE("TempProxy should not be NULL", tempProxy != NULL);

         aap->SetValue(tempProxy);
         dtDAL::ActorProxy *ap = aap->GetValue();

         CPPUNIT_ASSERT_MESSAGE("GetValue should return what it was set to", ap == tempProxy);

         ap->SetName("Bob");
         CPPUNIT_ASSERT_MESSAGE("The proxy's name should be Bob", ap->GetName() == "Bob");

         aap->SetValue(NULL);
         CPPUNIT_ASSERT_MESSAGE("The property's value should be NULL", !aap->GetValue());
      }
      else
      {
         SimpleStringToFromDataStreamCheck(ds, *aidap);
      }
   }
   else if (prop->GetDataType() == DataType::VEC3)
   {
      dtDAL::Vec3ActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }

      ///TODO  Note: this unit value is just temporary because anything else
      ///uncovers a problem with dtCore::Object due to multiple matrices affecting
      ///the end translation/rotation.
      //osg::Vec3 test(9.0f, 2.0f, 7.34f);
      osg::Vec3 test(1.f, 1.f, 1.f);

      //The character is currently broken.
      if (proxyTypeName == "Character")
      {
         std::cout << "Skipping property " << name << " on actor " << proxyTypeName << std::endl;
         return;
      }

      if (name == "Direction")
         //Direction ignores the y rotation because you can't roll a vector.
         test.y() = 0.0f;
      
      prop1->SetValue(test);
      osg::Vec3 result = prop1->GetValue();
      
      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec3 test2(7.0f, 3.0f, -9.25f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::VEC3F)
   {
      dtDAL::Vec3fActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec3fActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec3f test(9.037829f, 2.02322f, 7.34324f);
      if (name == "Direction")
         //Direction ignores the y rotation because you can't roll a vector.
         test.y() = 0.0f;
      
      prop1->SetValue(test);
      osg::Vec3f result = prop1->GetValue();
      
      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vecf[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec3f test2(7.0667f, 3.08595f, -9.2555950f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "f] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::VEC3D)
   {
      dtDAL::Vec3dActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec3dActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec3d test(9.037829, 2.02322, 7.34324);
      if (name == "Direction")
         //Direction ignores the y rotation because you can't roll a vector.
         test.y() = 0.0;

      prop1->SetValue(test);
      osg::Vec3d result = prop1->GetValue();

      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vecd[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
      osg::Vec3d test2(7.0, 3.0, -9.25);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));

      result = prop1->GetValue();

      for (int x = 0; x < 3; x++)
      {
         if (x == 1 && name == "Direction")
            return;
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "d] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
   }
   else if (prop->GetDataType() == DataType::VEC4)
   {
      dtDAL::Vec4ActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec4ActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec4 test(9.037829f, 2.02322f, 7.34324f, 7.2936299f);
      prop1->SetValue(test);
      osg::Vec4 result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec4 test2(7.0f, 3.0f, 8.0f, 2.1f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::VEC4F)
   {
      dtDAL::Vec4fActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec4fActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec4f test(9.037829f, 2.02322f, 7.34324f, 7.2936299f);
      prop1->SetValue(test);
      osg::Vec4f result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec4f test2(7.0f, 3.0f, 8.0f, 2.1f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "f] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      
   }
   else if (prop->GetDataType() == DataType::VEC4D)
   {
      dtDAL::Vec4dActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec4dActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec4d test(9.037829, 2.02322, 7.34324, 7.2936299);
      prop1->SetValue(test);
      osg::Vec4d result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
      osg::Vec4d test2(7.0, 3.0, 8.0, 2.1);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "d] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
      
   }
   else if (prop->GetDataType() == DataType::VEC2)
   {
      dtDAL::Vec2ActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec2ActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec2 test(9.35320f, 2.0323f);
      prop1->SetValue(test);
      osg::Vec2 result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec2 test2(7.0f, 3.0f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::VEC2F)
   {
      dtDAL::Vec2fActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec2fActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec2 test(9.35320f, 2.0323f);
      prop1->SetValue(test);
      osg::Vec2f result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec2f test2(7.0f, 3.0f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "f] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::VEC2D)
   {
      dtDAL::Vec2dActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::Vec2dActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      osg::Vec2 test(9.35320, 2.0323);
      prop1->SetValue(test);
      osg::Vec2d result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
      osg::Vec2d test2(7.0, 3.0);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 2; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "d] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(double(result[x]), double(test[x]), double(epsilon)));
      }
   }
   else if (prop->GetDataType() == DataType::RGBACOLOR)
   {
      dtDAL::ColorRgbaActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::ColorRgbaActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }

      osg::Vec4 test(6.1335543f, 0.3523333f, 5.05345f, 7323.3f);
      prop1->SetValue(test);
      osg::Vec4 result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
            << "] property: " << name << " expected: " << test << " - actual: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
      osg::Vec4 test2(7.0f, 3.0f, 8.0f, 2.1f);
      std::string stringValue = prop1->ToString();
      //change the value so we can change it back.
      prop1->SetValue(test2);
      CPPUNIT_ASSERT(prop1->FromString(stringValue));
      
      result = prop1->GetValue();
      
      for (int x = 0; x < 4; x++)
      {
         std::ostringstream ss;
         ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
            << "] property: " << name << " - Value: " << result;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                osg::equivalent(result[x], test[x], epsilon));
      }
   }
   else if (prop->GetDataType() == DataType::GAME_EVENT)
   {
      dtDAL::GameEventActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::GameEventActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }
      
      RefPtr<dtDAL::GameEvent> event = new dtDAL::GameEvent("TestEvent","This is a test game event.");
      dtDAL::GameEventManager::GetInstance().AddEvent(*event);
      prop1->SetValue(event.get());

      SimpleStringToFromDataStreamCheck(ds, *prop);

      dtDAL::GameEvent *eventToCheck = prop1->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Game Event name was equal.",
                             eventToCheck->GetName() == event->GetName());
      CPPUNIT_ASSERT_MESSAGE("Game Event descriptions were not equal.",
                             eventToCheck->GetDescription() == event->GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Game Event ids were not equal.",
                             eventToCheck->GetUniqueId() == event->GetUniqueId());

      std::string stringValue = prop1->ToString();
      CPPUNIT_ASSERT_MESSAGE("Resulting string value was not correct on the GAME_EVENT property.",
                             stringValue == event->GetUniqueId().ToString());

      CPPUNIT_ASSERT_MESSAGE("Should be able to set the string value of a game event.",
                             prop1->FromString(event->GetUniqueId().ToString()));

      eventToCheck = prop1->GetValue();
      CPPUNIT_ASSERT_MESSAGE("Game Event name was equal.",
                             eventToCheck->GetName() == event->GetName());
      CPPUNIT_ASSERT_MESSAGE("Game Event descriptions were not equal.",
                             eventToCheck->GetDescription() == event->GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Game Event ids were not equal.",
                             eventToCheck->GetUniqueId() == event->GetUniqueId());
   }
   else if (prop->GetDataType() == DataType::GROUP)
   {
      if (prop->GetName() == "TestGroup")
      {
         dtDAL::GroupActorProperty* prop1 = NULL;
         if (isElement)
         {
            prop1 = dynamic_cast<dtDAL::GroupActorProperty*>(prop);
         }
         else
         {
            proxy.GetProperty(prop->GetName(), prop1);
         }
         
         RefPtr<dtDAL::NamedGroupParameter> param = new dtDAL::NamedGroupParameter("TestGroup");

         param->AddParameter(*new dtDAL::NamedIntParameter("horse", 4));
         param->AddParameter(*new dtDAL::NamedFloatParameter("cow", 4.3f));
         param->AddParameter(*new dtDAL::NamedStringParameter("pig", "hello"));

         prop1->SetValue(*param);

         SimpleStringToFromDataStreamCheck(ds, *prop);

         RefPtr<dtDAL::NamedGroupParameter> valParam = prop1->GetValue();
         CPPUNIT_ASSERT(valParam.valid());
         CPPUNIT_ASSERT_EQUAL(valParam->GetName(), param->GetName());
         CPPUNIT_ASSERT_EQUAL(valParam->GetParameterCount(), param->GetParameterCount());

         dtDAL::NamedIntParameter* intParam = dynamic_cast<dtDAL::NamedIntParameter*>(valParam->GetParameter("horse"));
         CPPUNIT_ASSERT(intParam != NULL);
         CPPUNIT_ASSERT_EQUAL(4, intParam->GetValue());

         dtDAL::NamedFloatParameter* floatParam = dynamic_cast<dtDAL::NamedFloatParameter*>(valParam->GetParameter("cow"));
         CPPUNIT_ASSERT(floatParam != NULL);
         CPPUNIT_ASSERT_EQUAL(4.3f, floatParam->GetValue());

         dtDAL::NamedStringParameter* stringParam = dynamic_cast<dtDAL::NamedStringParameter*>(valParam->GetParameter("pig"));
         CPPUNIT_ASSERT(stringParam != NULL);
         CPPUNIT_ASSERT_EQUAL(std::string("hello"), stringParam->GetValue());
         
         std::string stringValue = prop1->ToString();
         
         CPPUNIT_ASSERT_MESSAGE("Should be able to set the string value of a group property.",
                                prop1->FromString(stringValue));
      }
   }
   else if (prop->GetDataType() == DataType::ARRAY)
   {
      dtDAL::ArrayActorPropertyBase* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::ArrayActorPropertyBase*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }

      // Iterate between each element in the array.
      int size = prop1->GetArraySize();
      for (int index = 0; index < size; index++)
      {
         prop1->SetIndex(index);
         testProp(proxy, prop1->GetArrayProperty(), true);
      }

      SimpleStringToFromDataStreamCheck(ds, *prop);
   }
   else if (prop->GetDataType() == DataType::CONTAINER)
   {
      dtDAL::ContainerActorProperty* prop1 = NULL;
      if (isElement)
      {
         prop1 = dynamic_cast<dtDAL::ContainerActorProperty*>(prop);
      }
      else
      {
         proxy.GetProperty(prop->GetName(), prop1);
      }

      // Iterate between each element in the container.
      int size = prop1->GetPropertyCount();
      for (int index = 0; index < size; index++)
      {
         testProp(proxy, prop1->GetProperty(index), true);
      }

      SimpleStringToFromDataStreamCheck(ds, *prop);
   }
}

void ProxyTest::compareProxies(dtDAL::ActorProxy& ap1, dtDAL::ActorProxy& ap2)
{
   std::vector<dtDAL::ActorProperty*> props;
   ap1.GetPropertyList(props);
   
   for(unsigned int i = 0; i < props.size(); i++)
   {
      std::string str = props[i]->GetName();
      dtDAL::ActorProperty* prop2 = ap2.GetProperty(str);
      
      CPPUNIT_ASSERT(prop2 != NULL);
      
      compareProperties(props[i], prop2);
   }
}

void ProxyTest::compareProperties(dtDAL::ActorProperty* prop1, dtDAL::ActorProperty* prop2)
{
   std::string str = prop1->GetName();
   const float epsilon = 0.01f;

   if(prop1->GetDataType() == DataType::FLOAT)
   {
      CPPUNIT_ASSERT(osg::equivalent(((dtDAL::FloatActorProperty*)prop1)->GetValue(),
                                     ((dtDAL::FloatActorProperty*)prop2)->GetValue(), (float)epsilon));
   }
   else if(prop1->GetDataType() == DataType::DOUBLE)
   {
      CPPUNIT_ASSERT(osg::equivalent(((dtDAL::DoubleActorProperty*)prop1)->GetValue(),
                                     ((dtDAL::DoubleActorProperty*)prop2)->GetValue(), (double)epsilon));
   }
   else if(prop1->GetDataType() == DataType::INT)
   {
      CPPUNIT_ASSERT(((dtDAL::IntActorProperty*)prop1)->GetValue() ==
                     ((dtDAL::IntActorProperty*)prop2)->GetValue());
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::LONGINT)
   {
      CPPUNIT_ASSERT(((dtDAL::LongActorProperty*)prop1)->GetValue() ==
                     ((dtDAL::LongActorProperty*)prop2)->GetValue());
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::STRING)
   {
      CPPUNIT_ASSERT(((dtDAL::StringActorProperty*)prop1)->GetValue() ==
                     ((dtDAL::StringActorProperty*)prop2)->GetValue());
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::BOOLEAN)
   {
      CPPUNIT_ASSERT(((dtDAL::BooleanActorProperty*)prop1)->GetValue() ==
                     ((dtDAL::BooleanActorProperty*)prop2)->GetValue());
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::ENUMERATION)
   {
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::ACTOR)
   {
      CPPUNIT_ASSERT(((dtDAL::ActorActorProperty*)prop1)->GetValue() ==
                     ((dtDAL::ActorActorProperty*)prop2)->GetValue());
      CPPUNIT_ASSERT(prop1->ToString() == prop2->ToString());
   }
   else if(prop1->GetDataType() == DataType::VEC3)
   {
      //if (true) continue;
      std::ostringstream ss;
      ss << ((dtDAL::Vec3ActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec3ActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec3ActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec3ActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[1], epsilon)
                             && osg::equivalent(((dtDAL::Vec3ActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[2], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC3F)
   {
      //if (true) continue;
      std::ostringstream ss;
      ss << ((dtDAL::Vec3fActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec3fActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec3fActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec3fActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec3fActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec3fActorProperty*)prop2)->GetValue()[1], epsilon)
                             && osg::equivalent(((dtDAL::Vec3fActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec3fActorProperty*)prop2)->GetValue()[2], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC3D)
   {
      //if (true) continue;
      std::ostringstream ss;
      ss << ((dtDAL::Vec3dActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec3dActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec3dActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec3dActorProperty*)prop2)->GetValue()[0], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec3dActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec3dActorProperty*)prop2)->GetValue()[1], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec3dActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec3dActorProperty*)prop2)->GetValue()[2], (double)epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC4)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec4ActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec4ActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec4ActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec4ActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[1], epsilon)
                             && osg::equivalent(((dtDAL::Vec4ActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[2], epsilon)
                             && osg::equivalent(((dtDAL::Vec4ActorProperty*)prop1)->GetValue()[3],
                                                ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[3], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC4F)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec4fActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec4fActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec4fActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec4fActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec4fActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec4fActorProperty*)prop2)->GetValue()[1], epsilon)
                             && osg::equivalent(((dtDAL::Vec4fActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec4fActorProperty*)prop2)->GetValue()[2], epsilon)
                             && osg::equivalent(((dtDAL::Vec4fActorProperty*)prop1)->GetValue()[3],
                                                ((dtDAL::Vec4fActorProperty*)prop2)->GetValue()[3], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC4D)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec4dActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec4dActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec4dActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec4dActorProperty*)prop2)->GetValue()[0], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec4dActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec4dActorProperty*)prop2)->GetValue()[1], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec4dActorProperty*)prop1)->GetValue()[2],
                                                ((dtDAL::Vec4dActorProperty*)prop2)->GetValue()[2], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec4dActorProperty*)prop1)->GetValue()[3],
                                                ((dtDAL::Vec4dActorProperty*)prop2)->GetValue()[3], (double)epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC2)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec2ActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec2ActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec2ActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec2ActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec2ActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec2ActorProperty*)prop2)->GetValue()[1], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC2F)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec2fActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec2fActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec2fActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec2fActorProperty*)prop2)->GetValue()[0], epsilon)
                             && osg::equivalent(((dtDAL::Vec2fActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec2fActorProperty*)prop2)->GetValue()[1], epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::VEC2D)
   {
      std::ostringstream ss;
      ss << ((dtDAL::Vec2dActorProperty*)prop1)->GetValue() << " vs " << ((dtDAL::Vec2dActorProperty*)prop2)->GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(((dtDAL::Vec2dActorProperty*)prop1)->GetValue()[0],
                                             ((dtDAL::Vec2dActorProperty*)prop2)->GetValue()[0], (double)epsilon)
                             && osg::equivalent(((dtDAL::Vec2dActorProperty*)prop1)->GetValue()[1],
                                                ((dtDAL::Vec2dActorProperty*)prop2)->GetValue()[1], (double)epsilon)
                             );
   }
   else if(prop1->GetDataType() == DataType::RGBACOLOR)
   {
      dtDAL::ColorRgbaActorProperty& colorProp1 = *static_cast<dtDAL::ColorRgbaActorProperty*>(prop1);
      dtDAL::ColorRgbaActorProperty& colorProp2 = *static_cast<dtDAL::ColorRgbaActorProperty*>(prop2);
      std::ostringstream ss;
      ss << colorProp1.GetValue() << " vs "
         << colorProp2.GetValue();
      CPPUNIT_ASSERT_MESSAGE(prop1->GetName() + " value should be the same: " + ss.str(),
                             osg::equivalent(colorProp1.GetValue()[0],
                                             colorProp2.GetValue()[0], epsilon)
                             && osg::equivalent(colorProp1.GetValue()[1],
                                                colorProp2.GetValue()[1], epsilon)
                             && osg::equivalent(colorProp1.GetValue()[2],
                                                colorProp2.GetValue()[2], epsilon)
                             && osg::equivalent(colorProp1.GetValue()[3],
                                                colorProp2.GetValue()[3], epsilon)
                             );
   }
   else if (prop1->GetDataType() == DataType::GAME_EVENT)
   {
      dtDAL::GameEventActorProperty& geProp1 = *static_cast<dtDAL::GameEventActorProperty*>(prop1);
      dtDAL::GameEventActorProperty& geProp2 = *static_cast<dtDAL::GameEventActorProperty*>(prop2);
      if (geProp1.GetValue() == NULL)
      {
         CPPUNIT_ASSERT(geProp2.GetValue() == NULL);
      }
      else
      {
         CPPUNIT_ASSERT(geProp2.GetValue() != NULL);
         
         CPPUNIT_ASSERT_MESSAGE("Game event properties should be equal.  But the first is " 
                                + geProp1.GetValue()->GetName() + "; the second is " + geProp2.GetValue()->GetName() + ".", 
                                geProp1.GetValue() == geProp2.GetValue());
      }
   }
   else if (prop1->GetDataType() == DataType::GROUP)
   {
      dtDAL::GroupActorProperty& gProp1 = *static_cast<dtDAL::GroupActorProperty*>(prop1);
      dtDAL::GroupActorProperty& gProp2 = *static_cast<dtDAL::GroupActorProperty*>(prop2);
      CPPUNIT_ASSERT_MESSAGE("Group actor properties must not return NULL", gProp1.GetValue().valid());
      CPPUNIT_ASSERT_MESSAGE("Group actor properties must not return NULL", gProp2.GetValue().valid());
      
      CPPUNIT_ASSERT_MESSAGE("The Group properties should be equal.", 
                             *gProp1.GetValue() == *gProp2.GetValue());
   }
   else if (prop1->GetDataType() == DataType::ARRAY)
   {
      dtDAL::ArrayActorPropertyBase& aProp1 = *static_cast<dtDAL::ArrayActorPropertyBase*>(prop1);
      dtDAL::ArrayActorPropertyBase& aProp2 = *static_cast<dtDAL::ArrayActorPropertyBase*>(prop2);

      // First check the size of the arrays.
      CPPUNIT_ASSERT(aProp1.GetArraySize() == aProp2.GetArraySize());
      CPPUNIT_ASSERT(aProp1.ToString()     == aProp2.ToString());

      // Now iterate between each element in the array and compare those.
      int size = aProp1.GetArraySize();
      for (int index = 0; index < size; index++)
      {
         aProp1.SetIndex(index);
         aProp2.SetIndex(index);
         compareProperties(aProp1.GetArrayProperty(), aProp2.GetArrayProperty());
      }
   }
   else if (prop1->GetDataType() == DataType::CONTAINER)
   {
      dtDAL::ContainerActorProperty& cProp1 = *static_cast<dtDAL::ContainerActorProperty*>(prop1);
      dtDAL::ContainerActorProperty& cProp2 = *static_cast<dtDAL::ContainerActorProperty*>(prop2);

      // First check the size of the containers.
      CPPUNIT_ASSERT(cProp1.GetPropertyCount() == cProp2.GetPropertyCount());
      CPPUNIT_ASSERT(cProp1.ToString()         == cProp2.ToString());

      // Now iterate between each element in the container and compare them.
      int size = cProp1.GetPropertyCount();
      for (int index = 0; index < size; index++)
      {
         compareProperties(cProp1.GetProperty(index), cProp2.GetProperty(index));
      }
   }
}

void ProxyTest::TestProxies()
{
   try
   {
      RefPtr<dtDAL::ActorProxy> proxy;
      
      for(unsigned int i = 0; i < actors.size(); i++)
      {
         // In order to keep the tests fasts, we skip the nasty slow ones.
         if (actors[i]->GetName() == dtActors::EngineActorRegistry::CLOUD_PLANE_ACTOR_TYPE->GetName() ||
             actors[i]->GetName() == dtActors::EngineActorRegistry::CLOUD_DOME_ACTOR_TYPE->GetName()  ||
             actors[i]->GetName() == dtActors::EngineActorRegistry::WEATHER_ENVIRONMENT_ACTOR_TYPE->GetName() || 
             actors[i]->GetName() == "Test Environment Actor" ||
             actors[i]->GetName() == "Sound Actor") //can't use the ActorType here; it adds a dependency to dtAudio, which might not exist
            continue;
         
         proxy = libMgr.CreateActorProxy(*actors[i]).get();
         CPPUNIT_ASSERT(proxy != NULL);
         proxies.push_back(proxy.get());
      }
      
      for(unsigned int i = 0; i < proxies.size(); i++)
      {
         proxy = proxies[i];
         CPPUNIT_ASSERT(proxy != NULL);
         LOGN_INFO("librarymanager.cpp", std::string("Testing proxy of type: ") + proxy->GetActorType().GetFullName());
         testProps(*proxy);
         
         compareProxies(*proxy, *proxy->Clone());

         CPPUNIT_ASSERT(!dtDAL::Project::GetInstance().GetEditMode());
         CPPUNIT_ASSERT_MESSAGE("The proxy should return false for being in STAGE", !proxy->IsInSTAGE());
         dtDAL::Project::GetInstance().SetEditMode(true);
         CPPUNIT_ASSERT(dtDAL::Project::GetInstance().GetEditMode());
         CPPUNIT_ASSERT_MESSAGE("The proxy should return true for being in STAGE", proxy->IsInSTAGE());
         dtDAL::Project::GetInstance().SetEditMode(false);
         CPPUNIT_ASSERT(!dtDAL::Project::GetInstance().GetEditMode());
         CPPUNIT_ASSERT_MESSAGE("The proxy should return false for being in STAGE", !proxy->IsInSTAGE());
      }
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

void ProxyTest::TestBezierProxies()
{
   try
   {
      dtDAL::LibraryManager &libMgr = dtDAL::LibraryManager::GetInstance();
      RefPtr<const dtDAL::ActorType>  at    = libMgr.FindActorType("dtcore.Curve", "Bezier Node");
      CPPUNIT_ASSERT(at != NULL);
      
      RefPtr<dtDAL::ActorProxy> one   = libMgr.CreateActorProxy(*at);
      RefPtr<dtDAL::ActorProxy> two   = libMgr.CreateActorProxy(*at);
      RefPtr<dtDAL::ActorProxy> three = libMgr.CreateActorProxy(*at);
      
      at = libMgr.FindActorType("dtcore.Curve", "Bezier Control Point");
      CPPUNIT_ASSERT(at != NULL);
      
      RefPtr<dtDAL::ActorProxy> entryBCP = libMgr.CreateActorProxy(*at);
      RefPtr<dtDAL::ActorProxy> exitBCP = libMgr.CreateActorProxy(*at);
      
      dtDAL::ActorActorProperty *prevProp1    = static_cast<dtDAL::ActorActorProperty*>(one->GetProperty("Previous Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", prevProp1 != NULL);
      dtDAL::ActorActorProperty *nextProp1    = static_cast<dtDAL::ActorActorProperty*>(one->GetProperty("Next Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", nextProp1 != NULL);
      dtDAL::ActorActorProperty *entryCtrlPntProp1 = static_cast<dtDAL::ActorActorProperty*>(one->GetProperty("Entry Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", entryCtrlPntProp1 != NULL);
      dtDAL::ActorActorProperty *exitCtrlPntProp1 = static_cast<dtDAL::ActorActorProperty*>(one->GetProperty("Exit Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", exitCtrlPntProp1 != NULL);
      
      dtDAL::ActorActorProperty *prevProp2    = static_cast<dtDAL::ActorActorProperty*>(two->GetProperty("Previous Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", prevProp2 != NULL);
      dtDAL::ActorActorProperty *nextProp2    = static_cast<dtDAL::ActorActorProperty*>(two->GetProperty("Next Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", nextProp2 != NULL);
      dtDAL::ActorActorProperty *entryCtrlPntProp2 = static_cast<dtDAL::ActorActorProperty*>(two->GetProperty("Entry Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", entryCtrlPntProp2 != NULL);
      dtDAL::ActorActorProperty *exitCtrlPntProp2 = static_cast<dtDAL::ActorActorProperty*>(two->GetProperty("Exit Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", exitCtrlPntProp2 != NULL);
      
      dtDAL::ActorActorProperty *prevProp3    = static_cast<dtDAL::ActorActorProperty*>(three->GetProperty("Previous Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", prevProp3 != NULL);
      dtDAL::ActorActorProperty *nextProp3    = static_cast<dtDAL::ActorActorProperty*>(three->GetProperty("Next Bezier Node"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", nextProp3 != NULL);
      dtDAL::ActorActorProperty *entryCtrlPntProp3 = static_cast<dtDAL::ActorActorProperty*>(three->GetProperty("Entry Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor property should not be NULL", entryCtrlPntProp3 != NULL);
      dtDAL::ActorActorProperty *exitCtrlPntProp3 = static_cast<dtDAL::ActorActorProperty*>(three->GetProperty("Exit Control Point"));
      CPPUNIT_ASSERT_MESSAGE("Actor type should not be NULL", exitCtrlPntProp3 != NULL);
      
      CPPUNIT_ASSERT_MESSAGE("The first proxy should not have a previous node", prevProp1->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The first proxy should not have a next node", nextProp1->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The first proxy should not have an enrty control point", entryCtrlPntProp1->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The first proxy should not have an exit control point", exitCtrlPntProp1->GetValue() == NULL);
      
      CPPUNIT_ASSERT_MESSAGE("The second proxy should not have a previous node", prevProp2->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The second proxy should not have a next node", nextProp2->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The second proxy should not have an enrty control point", entryCtrlPntProp2->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The second proxy should not have an exit control point", exitCtrlPntProp2->GetValue() == NULL);
      
      CPPUNIT_ASSERT_MESSAGE("The third proxy should not have a previous node", prevProp3->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The third proxy should not have a next node", nextProp3->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The third proxy should not have an enrty control point", entryCtrlPntProp3->GetValue() == NULL);
      CPPUNIT_ASSERT_MESSAGE("The third proxy should not have an exit control point", exitCtrlPntProp3->GetValue() == NULL);
      
      nextProp1->SetValue(two.get());
      nextProp2->SetValue(three.get());
      
      entryCtrlPntProp1->SetValue(entryBCP.get());
      exitCtrlPntProp1->SetValue(exitBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first node's next should be the second", nextProp1->GetValue() == two.get());
      CPPUNIT_ASSERT_MESSAGE("The first next property's real actor should be the node", nextProp1->GetRealActor() == nextProp1->GetValue()->GetActor());
      
      CPPUNIT_ASSERT_MESSAGE("The second node's next should be the third", nextProp2->GetValue() == three.get());
      CPPUNIT_ASSERT_MESSAGE("The second next property's real actor should be the node", nextProp2->GetRealActor() == nextProp2->GetValue()->GetActor());
      
      CPPUNIT_ASSERT_MESSAGE("The third node's next should be NULL", nextProp3->GetValue() == NULL);
      
      CPPUNIT_ASSERT_MESSAGE("The third node's previous should be the second", prevProp3->GetValue() == two.get());
      CPPUNIT_ASSERT_MESSAGE("The second node's previous should be the first", prevProp2->GetValue() == one.get());
      CPPUNIT_ASSERT_MESSAGE("The first node's previous should be NULL", prevProp1->GetValue() == NULL);
      
      prevProp3->SetValue(two.get());
      prevProp2->SetValue(one.get());
      
      CPPUNIT_ASSERT_MESSAGE("The third node's previous should be the node", prevProp3->GetRealActor() == prevProp3->GetValue()->GetActor());
      CPPUNIT_ASSERT_MESSAGE("The second node's previous should be the node", prevProp2->GetRealActor() == prevProp2->GetValue()->GetActor());
      
      CPPUNIT_ASSERT_MESSAGE("The third's previous should still be the second", prevProp3->GetValue() == two.get());
      CPPUNIT_ASSERT_MESSAGE("The second's previous should still be the first", prevProp2->GetValue() == one.get());
      CPPUNIT_ASSERT_MESSAGE("The first's previous should still be NULL", prevProp1->GetValue() == NULL);
      
      CPPUNIT_ASSERT_MESSAGE("The entry control point of the first node should still be set", entryCtrlPntProp1->GetValue() == entryBCP.get());
      CPPUNIT_ASSERT_MESSAGE("The exit control point of the first node should still be set", exitCtrlPntProp1->GetValue() == exitBCP.get());
      
      nextProp1->SetValue(NULL);
      nextProp1->SetReadOnly(true);
      nextProp1->SetValue(two.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first's next property is read only, the value should still be NULL", nextProp1->GetValue() == NULL);
      
      nextProp1->SetReadOnly(false);
      nextProp1->SetValue(two.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first's next property should have been set", nextProp1->GetValue() == two.get());
      
      entryCtrlPntProp1->SetValue(NULL);
      entryCtrlPntProp1->SetReadOnly(true);
      entryCtrlPntProp1->SetValue(entryBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first entry control point is read only, the value should still be NULL", entryCtrlPntProp1->GetValue() == NULL);
      
      entryCtrlPntProp1->SetReadOnly(false);
      entryCtrlPntProp1->SetValue(entryBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first entry control point's value should have been set", entryCtrlPntProp1->GetValue() == entryBCP.get());
      
      exitCtrlPntProp1->SetValue(NULL);
      exitCtrlPntProp1->SetReadOnly(true);
      exitCtrlPntProp1->SetValue(exitBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first exit control point is read only, the value should still be NULL", exitCtrlPntProp1->GetValue() == NULL);
      
      exitCtrlPntProp1->SetReadOnly(false);
      exitCtrlPntProp1->SetValue(exitBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The first exit control point's value should have been set", exitCtrlPntProp1->GetValue() == exitBCP.get());
      
      nextProp2->SetValue(NULL);
      nextProp2->SetReadOnly(true);
      nextProp2->SetValue(three.get());
      
      CPPUNIT_ASSERT_MESSAGE("The seconds's next property is read only, the value should still be NULL", nextProp2->GetValue() == NULL);
      
      nextProp2->SetReadOnly(false);
      nextProp2->SetValue(three.get());
      
      CPPUNIT_ASSERT_MESSAGE("The seconds's next property should have been set", nextProp2->GetValue() == three.get());
      
      entryCtrlPntProp2->SetValue(NULL);
      entryCtrlPntProp2->SetReadOnly(true);
      entryCtrlPntProp2->SetValue(entryBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The second's entry control point is read only, the value should still be NULL", entryCtrlPntProp2->GetValue() == NULL);
      
      entryCtrlPntProp2->SetReadOnly(false);
      entryCtrlPntProp2->SetValue(entryBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The second's entry control point's value should have been set", entryCtrlPntProp2->GetValue() == entryBCP.get());
      
      exitCtrlPntProp2->SetValue(NULL);
      exitCtrlPntProp2->SetReadOnly(true);
      exitCtrlPntProp2->SetValue(exitBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The second's exit control point is read only, the value should still be NULL", exitCtrlPntProp2->GetValue() == NULL);
      
      exitCtrlPntProp2->SetReadOnly(false);
      exitCtrlPntProp2->SetValue(exitBCP.get());
      
      CPPUNIT_ASSERT_MESSAGE("The second's exit control point's value should have been set", exitCtrlPntProp2->GetValue() == exitBCP.get());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}
