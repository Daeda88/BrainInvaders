#ifndef ov_ov_comm_h
#define ov_ov_comm_h

#include "ParallelPort\ParallelPort.h"

//use multibyte character code required in VS
using namespace OpenViBEVRDemos;

#include <Windows.h>
#include <iostream>

//for rows
#define OVTK_StimulationId_Label_01                          0x00008101

//for columns
#define OVTK_StimulationId_Label_07                          0x00008107

//train stimulation
#define OVTK_StimulationId_ExperimentStop                    0x00008002

//every protocol policy requires different scenarios in OpenVibe!
enum eProtocolVersion
{
	TwobitPP=1, //Protocol based on 4 values 0,64,128,192 used with Mitsar amplifier

	EightbitPP, //Protocol that uses values from 0..255

	SoftwareTagging, //Does not use parallel port, but IPC (interprocess communication)
};

class OVComm
{

  public:
	OVComm(eProtocolVersion ver);

	//Setting P300 target requires thse 2 methods + SetFinalized after each one
	void SetTargetRow(int);//zero based
    void SetTargetColumn(int);//zero based

    //A single flash requires ONE of these methods + SetFinalized
    void SetFlashRow(int);
    void SetFlashColumn(int);

	void SetFinalized();

	//activates training, usually marks that input data has been provided and processing (xDawn filter trainging, classifier training) should begin
	//historically 192 on parallel port, also requires SetFinalized
    void SetBeginProcessing();  

    void Train(int repetions,int flashCount);//does everything, no SetFinalized required

   private:

	  eProtocolVersion m_eProtocol;

	  ParallelPort* m_pport;

	  int m_targetBaseRow;
	  int m_targetBaseColumn;

	  int m_flashBaseRow;
	  int m_flashBaseColumn;

	  int m_software_tagging_target_stimulations_shift;
};


#endif