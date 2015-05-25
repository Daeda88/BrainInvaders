#include "ovpCBoxStimulationTransformer.h"

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

OpenViBE::boolean CBoxStimulationTransformer::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	//> init input stimulation
	m_pStimulationDecoderTrigger=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
    m_pStimulationDecoderTrigger->initialize();
    ip_pMemoryBufferToDecodeTrigger.initialize(m_pStimulationDecoderTrigger->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
    op_pStimulationSetTrigger.initialize(m_pStimulationDecoderTrigger->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
	
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

	//> get conversion list
	m_sConversionListFileLocation=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	bool l_bConfigFileParsed = ReadConversionList();
	if (!l_bConfigFileParsed) return false;

	return true;
}

OpenViBE::boolean CBoxStimulationTransformer::uninitialize(void)
{
	// uninit input stimulation
	m_pStimulationDecoderTrigger->uninitialize();
    ip_pMemoryBufferToDecodeTrigger.uninitialize();
	op_pStimulationSetTrigger.uninitialize();
    this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoderTrigger);
	
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

OpenViBE::boolean CBoxStimulationTransformer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

OpenViBE::boolean CBoxStimulationTransformer::process(void)
{
	//> Get dynamic box context
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	for(uint32 j=0; j<l_pDynamicBoxContext->getInputChunkCount(0); j++)
	{
		ip_pMemoryBufferToDecodeTrigger=l_pDynamicBoxContext->getInputChunk(0, j);
		m_pStimulationDecoderTrigger->process();

		ip_pStimulationsToEncode1->clear();//st 1
		ip_pStimulationsToEncode2->clear();//st 2

		if(m_pStimulationDecoderTrigger->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		  {
			uint64 l_ui64ChunkStartTime=l_pDynamicBoxContext->getInputChunkStartTime(0, j);
			uint64 l_ui64ChunkEndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, j);

			for(uint32 stim=0; stim<op_pStimulationSetTrigger->getStimulationCount(); stim++)
			{
				std::vector<ConversionEntry>::const_iterator cii;

	            for(cii = m_vConversionEntries.begin(); cii!=m_vConversionEntries.end(); cii++)
				{
					OpenViBE::uint64 l_ui64CurrentIdentifier = op_pStimulationSetTrigger->getStimulationIdentifier(stim);
					uint64 l_ui64sampleTime = op_pStimulationSetTrigger->getStimulationDate(stim);

					if (l_ui64CurrentIdentifier>0)
						this->getLogManager() << LogLevel_Warning << "Valid stimulation detected: " << l_ui64CurrentIdentifier << "\n";

					if ((l_ui64CurrentIdentifier!=0) && ((*cii).InputStimulation == l_ui64CurrentIdentifier))
					{
						this->getLogManager() << LogLevel_Warning << "Transforming:" << l_ui64CurrentIdentifier << "\n";

						//set new stimulation code (output) and preferred channel
						if ((*cii).OutputChannel==1)
						{
							ip_pStimulationsToEncode1->appendStimulation((*cii).OutputStimulation,l_ui64sampleTime, 0); //add to st1
						}
						else if ((*cii).OutputChannel==2)
						{
							ip_pStimulationsToEncode2->appendStimulation((*cii).OutputStimulation,l_ui64sampleTime, 0); //add to st1
						}
					}
				}
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

OpenViBE::boolean CBoxStimulationTransformer::ReadConversionList()
{
  using namespace std;

  string line;
  ifstream myfile(m_sConversionListFileLocation);

  int i=0;
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
	  i++;

      getline (myfile,line);

	  if (line=="" || line.c_str()[0] == '#' || line.c_str()[0] == '\r' || line.c_str()[0] == '\n') continue;
      
	  //parse single line
	  std::vector<std::string> tokens;
      boost::split(tokens, line, boost::is_any_of(","));

	  //create entry
	  if (tokens.size()!=3)
	  {	  
		  this->getLogManager() << LogLevel_Error << "Number of values in transformation line is less than expected in: " << m_sConversionListFileLocation << "\n Conversion line (1 based) " << i << " ignored!";
		  continue;
	  }

	  //output stimulation
      std::istringstream iss(tokens[1]);
	  uint64 OutputStimulation=0;
	  if(!(iss>>std::hex>>OutputStimulation))
      {
		  this->getLogManager() << LogLevel_Error << "Bad output hex value in: " << m_sConversionListFileLocation << "\n Conversion line (1 based) " << i << " ignored!";
		  continue;
	  }

	   //input stimulation
      std::istringstream oss(tokens[0]);
	  uint64 InputStimulation=0;
	  if(!(oss>>std::hex>>InputStimulation))
      {
		  this->getLogManager() << LogLevel_Error << "Bad input hex value in: " << m_sConversionListFileLocation << "\n Conversion line (1 based) " << i << " ignored!";
		  continue;
	  }

	  ConversionEntry e(InputStimulation,OutputStimulation,atoi(tokens[2].c_str()));
	  m_vConversionEntries.push_back(e);
    }
    
	myfile.close();
	return true;

  } 
  else this->getLogManager() << LogLevel_Error << "File not found: " << m_sConversionListFileLocation << "\n"; 
  
  return false;
}
