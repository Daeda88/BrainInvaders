#ifndef __OpenViBEPlugins_BoxAlgorithm_StimulationTransformer_H__
#define __OpenViBEPlugins_BoxAlgorithm_StimulationTransformer_H__

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <deque>
#include <vector>

#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>

#define OVP_ClassId_BoxStimulationTransformer       OpenViBE::CIdentifier(0x2E0D2C8D, 0x1DA84D36)
#define OVP_ClassId_BoxStimulationTransformerDesc   OpenViBE::CIdentifier(0x1BD92639, 0x39B5671A)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		/*
		  Converts stimulations from one to another (changes stimulation code) and redirects to specific output channel 
		*/
		class CBoxStimulationTransformer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxStimulationTransformer);

			struct ConversionEntry //maps incoming stimulations to output stimulations with the channel requested
			{
				OpenViBE::uint64 InputStimulation;
				OpenViBE::uint64 OutputStimulation;
				OpenViBE::uint32 OutputChannel;

				ConversionEntry(OpenViBE::uint64 pInputStimulation,OpenViBE::uint64 pOutputStimulation,OpenViBE::uint32 pOutputChannel) : InputStimulation(pInputStimulation), OutputStimulation(pOutputStimulation), OutputChannel(pOutputChannel)
				{

				}
			};

			std::vector < ConversionEntry > m_vConversionEntries;

		protected:

			//stimulation input 1
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoderTrigger;
			OpenViBE::Kernel::TParameterHandler <const OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecodeTrigger;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSetTrigger;

			//Stimulation 1
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationsToEncode1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer1;

			//Stimulation 2
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder2;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationsToEncode2;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer2;

			OpenViBE::CString m_sConversionListFileLocation;

			//Example file:
			//0x00008100,0x00008101,1
            //0x0000810A,0x00008102,2
            //0x0000810B,0x00008103,1
			OpenViBE::boolean CBoxStimulationTransformer::ReadConversionList();
		};

		class CBoxStimulationTransformerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation Transformer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Anton Andreev"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("GIPSA-Lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Replaces stimulations and/or redirects them."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Enables you to replace one stimulation with another one (from a list) and to redirect stimulations to another channel."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-jump-to"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxStimulationTransformer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxStimulationTransformer; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input stimulation channel",		OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Output stimulations 1",	        OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Output stimulations 2",	        OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Transformation file list location:",OV_TypeId_Filename,"");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxStimulationTransformerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StimulationTransformer_H__
