#ifndef __OpenViBEPlugins_BoxAlgorithm_PP2Stim_H__
#define __OpenViBEPlugins_BoxAlgorithm_PP2Stim_H__

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

#define OVP_ClassId_BoxAlgorithm_PP2Stim     OpenViBE::CIdentifier(0x64BA3B50, 0x69804FCB)
#define OVP_ClassId_BoxAlgorithm_PP2StimDesc OpenViBE::CIdentifier(0x764A32D9, 0x7877536A)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmPP2Stim : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_PP2Stim);

			struct ConversionEntry //maps incoming number values to output stimulation and selects output channel
			{
				OpenViBE::uint64 Number;
				OpenViBE::uint64 StimulationCode;
				OpenViBE::uint32 OutputChannel;

				ConversionEntry(OpenViBE::uint64 pNumber,OpenViBE::uint64 pStimulationCode,OpenViBE::uint32 pOutputChannel) : Number(pNumber), StimulationCode(pStimulationCode), OutputChannel(pOutputChannel)
				{

				}
			};

			std::vector < ConversionEntry > m_vConversionEntries;

		protected:

			//trigger input
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalDecoderTrigger;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecodeTrigger;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pDecodedMatrixTrigger;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRateTrigger;

			//Stimulation 1
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationsToEncode1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer1;

			//Stimulation 2
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder2;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationsToEncode2;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer2;

			//OpenViBE::uint64 m_ui64StimulationToTrigUp;
			//OpenViBE::uint64 m_ui64StimulationToTrigDown;
			OpenViBE::CString m_sConversionListFileLocation;
			OpenViBE::uint64 m_ui32Policy;
			OpenViBE::float64 m_f64MemFront;

			//Example file:
			//64,0x00008101,1
            //128,0x00008102,2
            //192,0x00008103,1
			OpenViBE::boolean CBoxAlgorithmPP2Stim::ReadConversionList();

		};

		class CBoxAlgorithmPP2StimDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Parallel Port to Stimulation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Goyat Matthieu"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("GIPSA-Lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Convert Trigger to stimulation"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-jump-to"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_PP2Stim; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmPP2Stim; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Trigger channel",		OV_TypeId_Signal		);
				rBoxAlgorithmPrototype.addOutput ("Output stimulations 1",	OV_TypeId_Stimulations	);
				rBoxAlgorithmPrototype.addOutput ("Output stimulations 2",	OV_TypeId_Stimulations	);

				/*rBoxAlgorithmPrototype.addSetting("Stimulation for rising edge",    OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00"	);
				rBoxAlgorithmPrototype.addSetting("Stimulation for downward edge",  OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00"	);*/
				rBoxAlgorithmPrototype.addSetting("Conversion list location:",OV_TypeId_Filename,"");
				rBoxAlgorithmPrototype.addSetting("Policy:",OV_TypeId_Integer, "1");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PP2StimDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_Stim2Signal_H__
