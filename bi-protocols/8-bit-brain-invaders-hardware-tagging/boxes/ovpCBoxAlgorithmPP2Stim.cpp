#include "ovpCBoxAlgorithmPP2Stim.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

#include <iostream>

namespace
{
	class _AutoCast_
	{
	public:
		_AutoCast_(IBox& rBox, IConfigurationManager& rConfigurationManager, const uint32 ui32Index) : m_rConfigurationManager(rConfigurationManager) { rBox.getSettingValue(ui32Index, m_sSettingValue); }
		operator uint64 (void) { return m_rConfigurationManager.expandAsUInteger(m_sSettingValue); }
		operator int64 (void) { return m_rConfigurationManager.expandAsInteger(m_sSettingValue); }
		operator float64 (void) { return m_rConfigurationManager.expandAsFloat(m_sSettingValue); }
		operator OpenViBE::boolean (void) { return m_rConfigurationManager.expandAsBoolean(m_sSettingValue); }
		operator const CString (void) { return m_sSettingValue; }
	protected:
		IConfigurationManager& m_rConfigurationManager;
		CString m_sSettingValue;
	};
};

OpenViBE::boolean CBoxAlgorithmPP2Stim::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	//> init Trigger input
	m_pSignalDecoderTrigger=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pSignalDecoderTrigger->initialize();
	ip_pMemoryBufferToDecodeTrigger.initialize(m_pSignalDecoderTrigger->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pDecodedMatrixTrigger.initialize(m_pSignalDecoderTrigger->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRateTrigger.initialize(m_pSignalDecoderTrigger->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	
	//> init stimulation output 1
	m_pStimulationEncoder1=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder1->initialize();
	ip_pStimulationsToEncode1.initialize(m_pStimulationEncoder1->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pEncodedMemoryBuffer1.initialize(m_pStimulationEncoder1->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	//> init stimulation output 2
	m_pStimulationEncoder2=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStimulationEncoder2->initialize();
	ip_pStimulationsToEncode2.initialize(m_pStimulationEncoder2->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	op_pEncodedMemoryBuffer2.initialize(m_pStimulationEncoder2->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	//> get conversion list and parallel port policy
	m_sConversionListFileLocation=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui32Policy=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	ReadConversionList();

	m_f64MemFront==0;

	return true;
}

OpenViBE::boolean CBoxAlgorithmPP2Stim::uninitialize(void)
{
	// uninit trigger input
	m_pSignalDecoderTrigger->uninitialize();
	ip_pMemoryBufferToDecodeTrigger.uninitialize();
	op_pDecodedMatrixTrigger.uninitialize();
	op_ui64SamplingRateTrigger.uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalDecoderTrigger);
	
	// uninit stimulation output 1 
	m_pStimulationEncoder1->uninitialize();
	ip_pStimulationsToEncode1.uninitialize();
	op_pEncodedMemoryBuffer1.uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder1);

	// uninit stimulation output 2 
	m_pStimulationEncoder2->uninitialize();
	ip_pStimulationsToEncode2.uninitialize();
	op_pEncodedMemoryBuffer2.uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationEncoder2);

	return true;
}

OpenViBE::boolean CBoxAlgorithmPP2Stim::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

OpenViBE::boolean CBoxAlgorithmPP2Stim::process(void)
{
	//> Get dynamic box context
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	//> extraire les dates de stimuli par rapport au trigger
	for(uint32 j=0; j<l_pDynamicBoxContext->getInputChunkCount(0); j++)
	  {
		ip_pMemoryBufferToDecodeTrigger=l_pDynamicBoxContext->getInputChunk(0,j);
		m_pSignalDecoderTrigger->process();

		ip_pStimulationsToEncode1->clear();//st 1
		ip_pStimulationsToEncode2->clear();//st 2

		if(m_pSignalDecoderTrigger->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		  {
			uint64 l_ui64ChunkStartTime=l_pDynamicBoxContext->getInputChunkStartTime(0, j);
			uint64 l_ui64ChunkEndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, j);

			for(uint32 i=0; i<op_pDecodedMatrixTrigger->getDimensionSize(1); i++)
			{
				//get value 
				float64 l_f64value=op_pDecodedMatrixTrigger->getBuffer()[i];
			
				//two policies can be avaialble 

				//polycy 1 - values are sperated by 0
				//rise stimulation on the rising edge from zero to some number
				if ( (l_f64value != 0) && (l_f64value != m_f64MemFront) && (m_f64MemFront==0))
				{
					OpenViBE::uint64 identifier;
				    
					std::vector<ConversionEntry>::const_iterator cii;

	                for(cii = m_vConversionEntries.begin(); cii!=m_vConversionEntries.end(); cii++)
					{
						uint64 l_ui64sampleTime = l_ui64ChunkStartTime+ uint64((i/float64(op_ui64SamplingRateTrigger))*(1LL<<32));
						if ((*cii).Number == l_f64value)
						{
							if ((*cii).OutputChannel==1)
							{
							   //l_oStimulationSet.appendStimulation(identifier,( uint64(iSample) << 32) / m_oHeader.getSamplingFrequency(),0);*/
								ip_pStimulationsToEncode1->appendStimulation((*cii).StimulationCode,l_ui64sampleTime, 0); //add to st1
							}
							else if ((*cii).OutputChannel==2)
							{
								ip_pStimulationsToEncode2->appendStimulation((*cii).StimulationCode,l_ui64sampleTime, 0); //add to st1
							}
						}
					}
				}

				//policy 2 - values are not sperated by 0
				//		if(m_f64MemFront!=l_f64value) //nouveau front
				//		{
				//			//sample index to time OV :
				//			uint64 l_ui64sampleTime=l_ui64ChunkStartTime+ uint64((i/float64(op_ui64SamplingRateTrigger))*(1LL<<32)); //a verifier
				//			//Stim id :
				//			uint64 l_StimId=0;
				//			if(l_f64value==0){l_StimId=m_ui64StimulationToTrigDown;}  //front descendant
				//			else {l_StimId=m_ui64StimulationToTrigUp;}  //front montant
				//			//save
				//			ip_pStimulationsToEncode1->appendStimulation(l_StimId,l_ui64sampleTime, 0); //add to st1
				            
				//            m_f64MemFront=l_f64value;
				//		}

				m_f64MemFront = l_f64value;
			}

			//if empty add empty adds some fake stimulation for stim1
			if(ip_pStimulationsToEncode1->getStimulationCount()<=0)
			  {ip_pStimulationsToEncode1->appendStimulation(0,l_ui64ChunkStartTime+(l_ui64ChunkEndTime-l_ui64ChunkStartTime)/2, 0);}

			//output stimulation set 1
		    op_pEncodedMemoryBuffer1=l_pDynamicBoxContext->getOutputChunk(0);
			m_pStimulationEncoder1->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
			l_pDynamicBoxContext->markOutputAsReadyToSend(0,l_ui64ChunkStartTime ,l_ui64ChunkEndTime );

			//if empty add empty adds some fake stimulation for stim2
			if(ip_pStimulationsToEncode2->getStimulationCount()<=0)
			  {ip_pStimulationsToEncode2->appendStimulation(0,l_ui64ChunkStartTime+(l_ui64ChunkEndTime-l_ui64ChunkStartTime)/2, 0);}

			//output stimulation set 2
		    op_pEncodedMemoryBuffer2=l_pDynamicBoxContext->getOutputChunk(1);
			m_pStimulationEncoder2->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
			l_pDynamicBoxContext->markOutputAsReadyToSend(1,l_ui64ChunkStartTime ,l_ui64ChunkEndTime );
		  }

		l_pDynamicBoxContext->markInputAsDeprecated(0,j);
	  }

	return true;
}// End Process

OpenViBE::boolean CBoxAlgorithmPP2Stim::ReadConversionList()
{
  using namespace std;

  string line;
  ifstream myfile(m_sConversionListFileLocation);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);

	  if (line=="") continue;
      
	  //parse line
	  std::vector<std::string> tokens;
      boost::split(tokens, line, boost::is_any_of(","));

	  //forward data
	  if (tokens.size()!=3)
		  this->getLogManager() << LogLevel_Error << "Bad file format in: " << m_sConversionListFileLocation << "\n"; 

      std::istringstream iss(tokens[1]);
      uint64 stimulation=0;
      if(!(iss>>std::hex>>stimulation))throw exception("Invalid argument");

	  ConversionEntry e(atoi(tokens[0].c_str()),stimulation,atoi(tokens[2].c_str()));
	  m_vConversionEntries.push_back(e);
    }
     myfile.close();
	 return true;
  } else this->getLogManager() << LogLevel_Error << "File not found: " << m_sConversionListFileLocation << "\n"; 
  
  return false;


}
