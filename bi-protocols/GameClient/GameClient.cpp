// GameClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OVComm.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout<< "Start:"<<endl;

	OVComm comm(FourbitPP);

	comm.SetFlashColumn(3);
	Sleep(100);
	comm.SetFinalized();
	Sleep(50);

	cout << "Done."<<endl;

	return 0;
}

