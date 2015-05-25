#ifndef __OpenViBEApplication_ParallelPort_H__
#define __OpenViBEApplication_ParallelPort_H__

#include "stdafx.h"

#define _PPort_DLLFileName_ "inpout32.dll"
#define ParallelPortAdr 0x378

namespace OpenViBEVRDemos {

	class ParallelPort{

	public:

		ParallelPort();
		void ppTAG(unsigned int x);

	private:

		int instantiateParallelPort();

		typedef int (__stdcall *PPort_IN)(short PortAddress);
		typedef int (__stdcall *PPort_OUT)(short PortAddress, short data);

		HINSTANCE g_hPPortInstance;
		PPort_IN g_fpPPort_IN;
		PPort_OUT g_fpPPort_OUT;

	};

};
#endif