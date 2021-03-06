#include "ovpCBoxAlgorithmStim2Signal.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;


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

OpenViBE::boolean CBoxAlgorithmStim2Signal::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	//> init signal input
	m_pSignalDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
    m_pSignalDecoder->initialize();
    ip_pMemoryBufferToDecode1.initialize(m_pSignalDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
    op_pDecodedMatrix.initialize(m_pSignalDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRate.initialize(m_pSignalDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
    
	//> init stimlation input
	m_pStimulationDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
    m_pStimulationDecoder->initialize();
    ip_pMemoryBufferToDecode2.initialize(m_pStimulationDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSet.initialize(m_pStimulationDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	//> init signal output
	m_pSignalEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
    m_pSignalEncoder->initialize();
    ip_pMatrixToEncode.initialize(m_pSignalEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
    op_pEncodedMemoryBuffer.initialize(m_pSignalEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
    ip_ui64SamplingRate.initialize(m_pSignalEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	
	//> get user stimulation type
	m_ui64Stimulation     =this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, _AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 0));
	m_StimulationLabel = _AutoCast_(l_rStaticBoxContext, this->getConfigurationManager(), 0);

	//> Connect input data sampling rate to the output sampling rate. 
	ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);

	//> init flag start
	m_ui32FlagStart=0;

	//> init StimChannel State
	m_ui32StimChannelState=0;

	return true;
}

OpenViBE::boolean CBoxAlgorithmStim2Signal::uninitialize(void)
{
	// uninit signal input
	op_pDecodedMatrix.uninitialize();
    m_pSignalDecoder->uninitialize();
	ip_pMemoryBufferToDecode1.uninitialize();
	op_ui64SamplingRate.uninitialize();
    this->getAlgorithmManager().releaseAlgorithm(*m_pSignalDecoder);
	
	// uninit stimulation input
	op_pStimulationSet.uninitialize();
    m_pStimulationDecoder->uninitialize();
	ip_pMemoryBufferToDecode2.uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationDecoder);

	// uninit signal output
	ip_pMatrixToEncode.uninitialize();
    m_pSignalEncoder->uninitialize();
	op_pEncodedMemoryBuffer.uninitialize();
    ip_ui64SamplingRate.uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalEncoder);
	
	return true;
}

OpenViBE::boolean CBoxAlgorithmStim2Signal::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

OpenViBE::boolean CBoxAlgorithmStim2Signal::process(void)
{
	if(m_ui32FlagStart==0)//Headers process
	{
	//> Get dynamic box context
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	//> Process the same number of Stimulation_Chunks(nbChunkSignal) and Data_Chunks(nbChunkStimulation)
	uint32 nbChunkSignal=l_pDynamicBoxContext->getInputChunkCount(0);
	uint32 nbChunkStimulation=l_pDynamicBoxContext->getInputChunkCount(1);
	uint32 minChunkBothInput=nbChunkSignal<nbChunkStimulation ? nbChunkSignal : nbChunkStimulation;

	//> Init local variables
	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	uint64 l_ui64StimulationDate=0;


	//> For each ChunkSignal/ChunkStimulation
	for(uint32 j=0; j<minChunkBothInput; j++)
		{
			//> Get Signal chunks and uncode them.
			ip_pMemoryBufferToDecode1=l_pDynamicBoxContext->getInputChunk(0, j);
			m_pSignalDecoder->process();
			//> Get Stimulation chunks and uncode them.
			ip_pMemoryBufferToDecode2=l_pDynamicBoxContext->getInputChunk(1, j);
			m_pStimulationDecoder->process();

			//> If both chunks (Signal and Stimulation) are headers...
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader) 
				&& 
			   m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader)	)
				{					
					//>Get output chunk size
					OpenViBE::uint32 l_nbChannels1=op_pDecodedMatrix->getDimensionSize(0);
					OpenViBE::uint32 l_nbChannels2=l_nbChannels1+1;
					OpenViBE::uint32 l_nbSamples=op_pDecodedMatrix->getDimensionSize(1);

					//>Init output chunk
					ip_pMatrixToEncode->setDimensionCount(2);
					ip_pMatrixToEncode->setDimensionSize(0,l_nbChannels2);
					ip_pMatrixToEncode->setDimensionSize(1,l_nbSamples);

					//>Copy original channels names.
					const char* l_ChannelLabel;
					for (uint32 a=0; a<l_nbChannels1; a++)
					{
						l_ChannelLabel=op_pDecodedMatrix->getDimensionLabel(0,a);
						ip_pMatrixToEncode->setDimensionLabel(0,a,l_ChannelLabel);
					}
					//>Add stimulation label channel name
					ip_pMatrixToEncode->setDimensionLabel(0,l_nbChannels1,m_StimulationLabel.toASCIIString());
					
					//>Mark Headers as ready to send 
					op_pEncodedMemoryBuffer=l_pDynamicBoxContext->getOutputChunk(0);
					m_pSignalEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
					l_ui64StartTime=l_pDynamicBoxContext->getInputChunkStartTime(0, j);
					l_ui64EndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, j);
					l_pDynamicBoxContext->markOutputAsReadyToSend(0,l_ui64StartTime ,l_ui64EndTime );

					//> start ok
					m_ui32FlagStart=1;

					//> Mark both inputs as deprecated.
					l_pDynamicBoxContext->markInputAsDeprecated(0,j);
					l_pDynamicBoxContext->markInputAsDeprecated(1,j);
				}

		}


	}//End Headers process
	else // FlagStart=1 - Buffers process...
	{
	
	//> Get dynamic box context
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	//> Process the same number of Stimulation_Chunks(nbChunkSignal) and Data_Chunks(nbChunkStimulation)
	uint32 nbChunkSignal=l_pDynamicBoxContext->getInputChunkCount(0);
	uint32 nbChunkStimulation=l_pDynamicBoxContext->getInputChunkCount(1);
	uint32 TempnbChunkStimulation=nbChunkStimulation;
	uint32 minChunkBothInput=nbChunkSignal<nbChunkStimulation ? nbChunkSignal : nbChunkStimulation;

	//> Init local variables
	uint64 l_ui64StartTime=0;
	uint64 l_ui64ChunkStimStartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	uint64 l_ui64StimulationDate=0;

	if((nbChunkSignal!=0)&&(nbChunkStimulation!=0))
	{
			//> Get Signal first chunk and uncode them. (process header)
			ip_pMemoryBufferToDecode1=l_pDynamicBoxContext->getInputChunk(0, 0);
			m_pSignalDecoder->process();

			//if signal chunk is a buffer, create the new stimulation channel.
			if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
				{

					//>Get output chunk size
					OpenViBE::uint32 l_nbChannels1=op_pDecodedMatrix->getDimensionSize(0);
					OpenViBE::uint32 l_nbChannels2=l_nbChannels1+1;
					OpenViBE::uint32 l_nbSamples=op_pDecodedMatrix->getDimensionSize(1);

					//>Init output chunk
					ip_pMatrixToEncode->setDimensionCount(2);
					ip_pMatrixToEncode->setDimensionSize(0,l_nbChannels2);
					ip_pMatrixToEncode->setDimensionSize(1,l_nbSamples);

					//>Append output chunk data
					for (uint32 a=0; a<l_nbChannels2; a++)
					{
						if(a<l_nbChannels1) //>	 copy signal
						{
							for (uint32 b=0; b<l_nbSamples; b++)
							{
								ip_pMatrixToEncode->getBuffer()[a*l_nbSamples+b]=op_pDecodedMatrix->getBuffer()[a*l_nbSamples+b];
							}
						}
						else if(m_ui32StimChannelState==0)  //>  fill the stimulation channel with zeros. 
						{
							for (uint32 b=0; b<l_nbSamples; b++)
							{
								ip_pMatrixToEncode->getBuffer()[a*l_nbSamples+b]=0;
							}
						}
						else //>  fill the stimulation channel with ones. 
						{
							for (uint32 b=0; b<l_nbSamples; b++)
							{
								ip_pMatrixToEncode->getBuffer()[a*l_nbSamples+b]=1;
							}
						}

					}

					//> Check last stimulations chunks... don't process stim chunks not relative to the data chunk
					l_ui64EndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, 0);//> get data chunk end date (relative to the start time of the scenario)
					uint32 flagBreak=0;
					for(uint32 nbStimChunk=nbChunkStimulation-1;flagBreak==0;nbStimChunk--)
					{
						
						l_ui64ChunkStimStartTime=l_pDynamicBoxContext->getInputChunkStartTime(1, nbStimChunk);
						if(l_ui64EndTime<l_ui64ChunkStimStartTime)
						{
							TempnbChunkStimulation--;
						}
						else
						{
							nbChunkStimulation=TempnbChunkStimulation;
							flagBreak=1;
						}
					}//< End supplementary stim chunks filtering.
					

					uint64 l_ui64ChunkStimEndTime=l_pDynamicBoxContext->getInputChunkEndTime(1, nbChunkStimulation-1);
					if(l_ui64EndTime<=l_ui64ChunkStimEndTime)//> wait all necessary stim chunks for the signal chunk processing
					{
					///> Process Stimulations chunks
					for(uint32 nbStimChunk=0;nbStimChunk<nbChunkStimulation;nbStimChunk++)
					{
						uint32 l_ui32FlagPreserveChunk=0;

						////> Get Stimulation chunks and uncode them.
						ip_pMemoryBufferToDecode2=l_pDynamicBoxContext->getInputChunk(1, nbStimChunk);
						m_pStimulationDecoder->process();

						/////>if stim chunks are buffers.
						if(	m_pStimulationDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
						{
							//////> stimulation processing
							for(uint32 stim=0; stim<op_pStimulationSet->getStimulationCount(); stim++)
							{
								///////> check stimulation identifier - default stimulation :  OVTK_StimulationId_Label_01
								//std::cout<<"Stim : "<<op_pStimulationSet->getStimulationIdentifier(j)<<"-/-"<<m_ui64Stimulation<<std::endl;
								if(op_pStimulationSet->getStimulationIdentifier(stim)==m_ui64Stimulation)
								{
									
									//> get stimulation date (relative to the start time of the scenario)
									l_ui64StimulationDate=op_pStimulationSet->getStimulationDate(stim);
									//> get data chunk start date (relative to the start time of the scenario)
									l_ui64StartTime=l_pDynamicBoxContext->getInputChunkStartTime(0, 0);
									//> get data chunk end date (relative to the start time of the scenario)
									l_ui64EndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, 0);

									
									//> process stim_index = sample number wich correspond to the stimulation date
									uint32 stim_index=(uint32)((op_ui64SamplingRate*(l_ui64StimulationDate+(1LL<<32)/op_ui64SamplingRate))>>32);
									//> process chunk_index = sample number wich correspond to the start of the data chunk
									uint32 chunk_index=(uint32)((op_ui64SamplingRate*(l_ui64StartTime+(1LL<<32)/op_ui64SamplingRate))>>32);
									//> process chunk_relative_stim_index = data sample whom correspond to the stimulation.
									uint32 chunk_relative_stim_index=(stim_index-chunk_index);

									//if(stim_index==chunk_index)printf("Warning EQUAL \n");
									//if(stim_index<chunk_index)printf("Warning NEGATIVE \n");
									//if(chunk_relative_stim_index>l_nbSamples)printf("Warning OUT OF RANGE \n");
								
									if(chunk_relative_stim_index<=l_nbSamples)
									{
									//printf("stim %d \n",stim_index);
									if(m_ui32StimChannelState==0)
									{
										for (uint32 b=chunk_relative_stim_index; b<l_nbSamples; b++)
										{
											ip_pMatrixToEncode->getBuffer()[(l_nbChannels2-1)*l_nbSamples+b]=1;
										}
										m_ui32StimChannelState=1;
									}
									else
									{
										for (uint32 b=chunk_relative_stim_index; b<l_nbSamples; b++)
										{
											ip_pMatrixToEncode->getBuffer()[(l_nbChannels2-1)*l_nbSamples+b]=0;
										}
										m_ui32StimChannelState=0;
									}
									}
									else
									{
										l_ui32FlagPreserveChunk=1;
									}
									
								}///////< End stimulation type OK
						
							}//////< End stimulation processing
												
						}/////< End if stim buffer

						//> if all stim in the stim chunk has been used -> deprecate the stim chunk (nbStimChunk)
						if(l_ui32FlagPreserveChunk==0)l_pDynamicBoxContext->markInputAsDeprecated(1,nbStimChunk);
						
					}////< End process stim chunk

					//> mark output as ready to send
					op_pEncodedMemoryBuffer=l_pDynamicBoxContext->getOutputChunk(0);
					m_pSignalEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
					l_ui64StartTime=l_pDynamicBoxContext->getInputChunkStartTime(0, 0);
					l_ui64EndTime=l_pDynamicBoxContext->getInputChunkEndTime(0, 0);
					l_pDynamicBoxContext->markOutputAsReadyToSend(0,l_ui64StartTime ,l_ui64EndTime );
					
					//> deprecate the signal chunk
					l_pDynamicBoxContext->markInputAsDeprecated(0,0);

					}///< End if(l_ui64EndTime<l_ui64ChunkStimEndTime)
					
				}//< End if signal buffer



				//> If chunk (Signal) is footer...
				if(m_pSignalDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
				{
					
					l_pDynamicBoxContext->markInputAsDeprecated(0,0);
					
				}

			
			
		}
		}

	return true;
}// End Process stim2sig.



