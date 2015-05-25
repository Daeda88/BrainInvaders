#include "ParallelPort.h"
using namespace OpenViBEVRDemos;

#include <iostream>
using namespace std;

ParallelPort::ParallelPort(){
	int err = instantiateParallelPort();
	if ( err!=0 )
	{
		std::cout << "INIT port parallele ERROR : " << err << std::endl;
		system("pause");
	}
	else
	{
		std::cout << "Library loaded successfully\n" << std::endl;
	}
}

int ParallelPort::instantiateParallelPort(){
	g_hPPortInstance=::LoadLibrary(_PPort_DLLFileName_);
	if(!g_hPPortInstance) {
		return 1;
	}
	g_fpPPort_IN =(PPort_IN) GetProcAddress (g_hPPortInstance, "Inp32");
	g_fpPPort_OUT=(PPort_OUT)GetProcAddress (g_hPPortInstance, "Out32");
	
	if(!g_fpPPort_IN || !g_fpPPort_OUT)
	{
		return 2;
	}
	return 0;
}

void ParallelPort::ppTAG(unsigned int x)
{
	g_fpPPort_OUT(ParallelPortAdr, x);
}