#include "OVComm.h"
#include "OV_software_tagger.h"

OVComm::OVComm(eProtocolVersion ver) : m_eProtocol(ver)
{
	m_pport = new ParallelPort();

	m_targetBaseRow=100;
	m_targetBaseColumn=120;

	m_flashBaseRow=20;
	m_flashBaseColumn=40;

	//flash and target stimulations must be different
	//targets use the flash stimulation numbers + this variable
	m_software_tagging_target_stimulations_shift=20;

	if (ver == SoftwareTagging)
	{
		ST_Init();
	}
}

void OVComm::SetTargetRow(int rowNumber) //starts from 0
{
	int row = m_targetBaseRow + rowNumber;

	if (m_eProtocol==TwobitPP)
	{	
		
		m_pport->ppTAG(128);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(row);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		ST_TagNow(OVTK_StimulationId_Label_01 + m_software_tagging_target_stimulations_shift + rowNumber);
	}
}

void OVComm::SetTargetColumn(int columnNumber) //starts from 0
{
	int column = m_targetBaseColumn + columnNumber;

	if (m_eProtocol==TwobitPP)
	{	
		m_pport->ppTAG(128);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(column);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		ST_TagNow(OVTK_StimulationId_Label_07 + m_software_tagging_target_stimulations_shift + columnNumber);
	}
}

void OVComm::SetFlashRow(int rowNumber) //starts from 0
{
	int row = m_flashBaseRow + rowNumber;

	if (m_eProtocol==TwobitPP)
	{
		m_pport->ppTAG(64);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(row);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		ST_TagNow(OVTK_StimulationId_Label_01+rowNumber);
	}
}

void OVComm::SetFlashColumn(int columnNumber) //starts from 0
{
	int column = m_flashBaseColumn + columnNumber;

	if (m_eProtocol==TwobitPP)
	{
		m_pport->ppTAG(64);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(column);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		ST_TagNow(OVTK_StimulationId_Label_07 + columnNumber);
	}
}

//called after every other set method to finalize it
void OVComm::SetFinalized()
{
	if (m_eProtocol==TwobitPP)
	{
		m_pport->ppTAG(0);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(0);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		//software tagging does not need to perform anything here
	}
}

//called at the end of the training
void OVComm::SetBeginProcessing()
{
	if (m_eProtocol==TwobitPP)
	{
		m_pport->ppTAG(192);
	}
	else if (m_eProtocol==EightbitPP)
	{
		m_pport->ppTAG(192);
	}
	else if (m_eProtocol==SoftwareTagging)
	{
		ST_TagNow(OVTK_StimulationId_ExperimentStop);
	}
}

void OVComm::Train(int repetions,int flashCount)
{
	if (m_eProtocol==EightbitPP || m_eProtocol==SoftwareTagging)
	{
		throw new std::exception("Undefined!");

		for (int i=0;i<flashCount;i++)
		{
			//set target

			//set flashes
		}

		SetBeginProcessing();
		Sleep(100);
		SetFinalized();
		Sleep(50);
	}
	else
	{
		throw new std::exception("Training not supported in general for this protocol option.");
	}
}