#include <dtGame/trackedmessage.h>
#include <dtGame/messagefactory.h>

namespace dtGame
{



        //Create Parameter Names all data tracked message will contain
        const std::string TrackedMessage::ID_PARAMETER = "ID";
        const std::string TrackedMessage::INVIC_PRESS_PARAMETER = "Invincibility Altered";
        const std::string TrackedMessage::FIRE_PARAMETER = "FIRE";
       
        const std::string TrackedMessage::ACT_TRANS_PARAMETER = "Actor Translation";
        const std::string TrackedMessage::ACT_ROT_PARAMETER = "Actor Rotation";
        const std::string TrackedMessage::ACT_ANG_PARAMETER = "Actor Angle";
       
        const std::string TrackedMessage::GUN_TRANS_PARAMETER = "Gun Translation";
        const std::string TrackedMessage::GUN_ROT_PARAMETER = "Gun Rotation";
        const std::string TrackedMessage::GUN_ANG_PARAMETER = "Gun Angle";
       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TrackedMessage::TrackedMessage()
        {
                //Add parameters TrackedMessage
                AddParameter(new dtGame::DoubleMessageParameter(ID_PARAMETER));
                AddParameter(new dtGame::BooleanMessageParameter(INVIC_PRESS_PARAMETER));
                AddParameter(new dtGame::BooleanMessageParameter(FIRE_PARAMETER));
               
                AddParameter(new dtGame::Vec3fMessageParameter(ACT_TRANS_PARAMETER));
                AddParameter(new dtGame::Vec3fMessageParameter(ACT_ROT_PARAMETER));
                AddParameter(new dtGame::DoubleMessageParameter(ACT_ANG_PARAMETER));
               
                AddParameter(new dtGame::Vec3fMessageParameter(GUN_TRANS_PARAMETER));
                AddParameter(new dtGame::Vec3fMessageParameter(GUN_ROT_PARAMETER));
                AddParameter(new dtGame::DoubleMessageParameter(GUN_ANG_PARAMETER));
               
                //Initiate Fire parameter to false
                bool fire = false;
                dtGame::BooleanMessageParameter* param = static_cast<dtGame::BooleanMessageParameter*>(GetParameter(FIRE_PARAMETER));
                param->SetValue(fire);
                
                //Initiate Fire parameter to false
                bool press = false;
                dtGame::BooleanMessageParameter* param_2 = static_cast<dtGame::BooleanMessageParameter*>(GetParameter(INVIC_PRESS_PARAMETER));
                param_2->SetValue(press);

        }      

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TrackedMessage::~TrackedMessage(void)
        {
        }
       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
        void TrackedMessage::SetID(const int& Id)
        {
                dtGame::IntMessageParameter* param = static_cast<dtGame::IntMessageParameter*>(GetParameter(ID_PARAMETER));
                param->SetValue(Id);
        }
               
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
           
        void TrackedMessage::SetInvinciblePress(const bool& press)
        {
                dtGame::BooleanMessageParameter* param = static_cast<dtGame::BooleanMessageParameter*>(GetParameter(INVIC_PRESS_PARAMETER));
                param->SetValue(press);
        }
               
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
        void TrackedMessage::SetFire(const bool& fire)
        {
                dtGame::BooleanMessageParameter* param = static_cast<dtGame::BooleanMessageParameter*>(GetParameter(FIRE_PARAMETER));
                param->SetValue(fire);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void TrackedMessage::SetActorTrans(const osg::Vec3f& actTrans)
        {
                dtGame::Vec3fMessageParameter* param = static_cast<dtGame::Vec3fMessageParameter*>(GetParameter(ACT_TRANS_PARAMETER));
                param->SetValue(actTrans);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void TrackedMessage::SetActorRot(const osg::Vec3f& actRot)
        {
                dtGame::Vec3fMessageParameter* param = static_cast<dtGame::Vec3fMessageParameter*>(GetParameter(ACT_ROT_PARAMETER));
                param->SetValue(actRot);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void TrackedMessage::SetActorAng(const double& actAng)
        {
                dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(ACT_ANG_PARAMETER));
                param->SetValue(actAng);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
        void TrackedMessage::SetGunTrans(const osg::Vec3f& gunTrans)
        {
                dtGame::Vec3fMessageParameter* param = static_cast<dtGame::Vec3fMessageParameter*>(GetParameter(GUN_TRANS_PARAMETER));
                param->SetValue(gunTrans);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void TrackedMessage::SetGunRot(const osg::Vec3f& gunRot)
        {
                dtGame::Vec3fMessageParameter* param = static_cast<dtGame::Vec3fMessageParameter*>(GetParameter(GUN_ROT_PARAMETER));
                param->SetValue(gunRot);
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void TrackedMessage::SetGunAng(const double& gunAng)
        {
                dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(GUN_ANG_PARAMETER));
                param->SetValue(gunAng);
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////               
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       
        const int TrackedMessage::GetID() const
        {
                return static_cast<const dtGame::IntMessageParameter*>(GetParameter(ID_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
           
        const bool TrackedMessage::GetInvinciblePress() const
        {
        		return static_cast<const dtGame::BooleanMessageParameter*>(GetParameter(INVIC_PRESS_PARAMETER))->GetValue();
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        const bool TrackedMessage::GetFire() const
        {
                return static_cast<const dtGame::BooleanMessageParameter*>(GetParameter(FIRE_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const osg::Vec3f TrackedMessage::GetActorTrans() const
        {
                return static_cast<const dtGame::Vec3fMessageParameter*>(GetParameter(ACT_TRANS_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const osg::Vec3f TrackedMessage::GetActorRot() const
        {
                return static_cast<const dtGame::Vec3fMessageParameter*>(GetParameter(ACT_ROT_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const double TrackedMessage::GetActorAng() const
        {
                return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(ACT_ANG_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const osg::Vec3f TrackedMessage::GetGunTrans() const
        {
                return static_cast<const dtGame::Vec3fMessageParameter*>(GetParameter(GUN_TRANS_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const osg::Vec3f TrackedMessage::GetGunRot() const
        {
                return static_cast<const dtGame::Vec3fMessageParameter*>(GetParameter(GUN_ROT_PARAMETER))->GetValue();
        }
                       
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const double TrackedMessage::GetGunAng() const
        {
                return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(GUN_ANG_PARAMETER))->GetValue();
        }

}

