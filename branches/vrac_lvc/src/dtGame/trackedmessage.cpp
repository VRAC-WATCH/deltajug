#include <dtGame/trackedmessage.h>
#include <dtGame/messagefactory.h>

namespace dtGame
{


	//Create Parameter Names all data tracked message will contain
	const std::string TrackedMessage::ID_PARAMETER = "ID";
	const std::string TrackedMessage::MATRIX00_PARAMETER = "Matrix_00";
	const std::string TrackedMessage::MATRIX01_PARAMETER = "Matrix_01";
	const std::string TrackedMessage::MATRIX02_PARAMETER = "Matrix_02";
	const std::string TrackedMessage::MATRIX03_PARAMETER = "Matrix_03";
	const std::string TrackedMessage::MATRIX10_PARAMETER = "Matrix_10";
	const std::string TrackedMessage::MATRIX11_PARAMETER = "Matrix_11";
	const std::string TrackedMessage::MATRIX12_PARAMETER = "Matrix_13";
	const std::string TrackedMessage::MATRIX13_PARAMETER = "Matrix_14";
	const std::string TrackedMessage::MATRIX20_PARAMETER = "Matrix_20";
	const std::string TrackedMessage::MATRIX21_PARAMETER = "Matrix_21";
	const std::string TrackedMessage::MATRIX22_PARAMETER = "Matrix_22";
	const std::string TrackedMessage::MATRIX23_PARAMETER = "Matrix_23";
	const std::string TrackedMessage::MATRIX30_PARAMETER = "Matrix_30";
	const std::string TrackedMessage::MATRIX31_PARAMETER = "Matrix_31";
	const std::string TrackedMessage::MATRIX32_PARAMETER = "Matrix_32";	
	const std::string TrackedMessage::MATRIX33_PARAMETER = "Matrix_33";
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TrackedMessage::TrackedMessage()
	{
		//Add parameters TrackedMessage
		AddParameter(new dtGame::DoubleMessageParameter(ID_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX00_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX01_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX02_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX03_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX10_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX11_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX12_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX13_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX20_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX21_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX22_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX23_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX30_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX31_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX32_PARAMETER));
		AddParameter(new dtGame::DoubleMessageParameter(MATRIX33_PARAMETER));

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
	
	void TrackedMessage::SetMatrix_00(const double& matrix_00)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX00_PARAMETER));
		param->SetValue(matrix_00);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_01(const double& matrix_01)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX01_PARAMETER));
		param->SetValue(matrix_01);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_02(const double& matrix_02)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX02_PARAMETER));
		param->SetValue(matrix_02);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_03(const double& matrix_03)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX03_PARAMETER));
		param->SetValue(matrix_03);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_10(const double& matrix_10)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX10_PARAMETER));
		param->SetValue(matrix_10);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_11(const double& matrix_11)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX11_PARAMETER));
		param->SetValue(matrix_11);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_12(const double& matrix_12)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX12_PARAMETER));
		param->SetValue(matrix_12);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_13(const double& matrix_13)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX13_PARAMETER));
		param->SetValue(matrix_13);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_20(const double& matrix_20)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX20_PARAMETER));
		param->SetValue(matrix_20);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_21(const double& matrix_21)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX21_PARAMETER));
		param->SetValue(matrix_21);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_22(const double& matrix_22)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX22_PARAMETER));
		param->SetValue(matrix_22);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_23(const double& matrix_23)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX23_PARAMETER));
		param->SetValue(matrix_23);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_30(const double& matrix_30)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX30_PARAMETER));
		param->SetValue(matrix_30);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_31(const double& matrix_31)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX31_PARAMETER));
		param->SetValue(matrix_31);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_32(const double& matrix_32)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX32_PARAMETER));
		param->SetValue(matrix_32);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void TrackedMessage::SetMatrix_33(const double& matrix_33)
	{
		dtGame::DoubleMessageParameter* param = static_cast<dtGame::DoubleMessageParameter*>(GetParameter(MATRIX33_PARAMETER));
		param->SetValue(matrix_33);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const int TrackedMessage::GetID() const
	{
		return static_cast<const dtGame::IntMessageParameter*>(GetParameter(ID_PARAMETER))->GetValue();
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_00() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX00_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_01() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX01_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_02() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX02_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_03() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX03_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_10() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX10_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_11() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX11_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_12() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX12_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_13() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX13_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_20() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX20_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_21() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX21_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_22() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX22_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_23() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX23_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_30() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX30_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_31() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX31_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_32() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX32_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const double TrackedMessage::GetMatrix_33() const
	{
		return static_cast<const dtGame::DoubleMessageParameter*>(GetParameter(MATRIX33_PARAMETER))->GetValue();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
