#include <iostream>
#include <windows.h>
#include "QLAComInterface.h"

using namespace std;

int main(int argc, char** argv)
{
	HRESULT hResult = CoInitialize(NULL);
	if (S_OK != hResult) { cout << "CoInitialize failed." << endl; return -1; }

	GUID CLSID_QLA;
	hResult = CLSIDFromProgID(L"QLAComponent", &CLSID_QLA);
	if (S_OK != hResult) { cout << "Can't find CLSID." << endl; return -2; }
	
	HINSTANCE com = ::LoadLibraryA("Q-LearningAlgorithmComponent.dll");
	if (com == NULL) { cout << "Load component dll failed." << endl; return -3; }

	HRESULT (__stdcall *pFunc)(REFCLSID, REFIID, void**);
	pFunc = (HRESULT (__stdcall *)(REFCLSID, REFIID, void**)) ::GetProcAddress(com, "DllGetClassObject");
	if (pFunc == NULL) { cout << "Get function DllGetClassObject failed." << endl; return -4; }

	IQLAClassFactory* pIFactory;
	HRESULT hr = pFunc(CLSID_QLA, __uuidof(IQLAClassFactory), (void**)&pIFactory);
	if (hr != S_OK) { cout << "Get interface IQLAClassFactory failed." << endl; return -5; }

	IQLAUnknown* pIUnknown;
	if(!pIFactory->CreateInstance(__uuidof(IQLAUnknown), (void**)&pIUnknown))
	{ cout << "Get interface IQLAUnknown failed." << endl; return -6; }

	ICommonUtils* pUtils;
	if (!pIUnknown->QueryInterface(__uuidof(ICommonUtils), (void**)&pUtils))
	{ cout << "Get interface ICommonUtils failed." << endl; return -7; }

	IQLearningAlgorithm* pQLA;
	if (!pIUnknown->QueryInterface(__uuidof(IQLearningAlgorithm), (void**)&pQLA))
	{ cout << "Get interface IQLearningAlgorithm failed." << endl; return -8; }

	if (argc != 6) pUtils->PrintUsage();
	else
	{
		int matrixDimension = atoi(argv[1]);
		int maxEpisode = atoi(argv[2]);
		double alpha = atof(argv[3]);
		double gamma = atof(argv[4]);
		int isPrintProgress = atoi(argv[5]);
		int position;

		pQLA->InitVariables(matrixDimension, maxEpisode, alpha, gamma);
		pQLA->TrainingModel(1, isPrintProgress);

		while (true)
		{
			cout << "Input the location of the robot: [0-" << matrixDimension - 1 << "], ";;
			cout << "or number " << matrixDimension << " to exit this program." << endl;
			cin >> position;

			if (position == matrixDimension) break;
			else if (position < matrixDimension && position >= 0)
			{
				cout << position << "->";

				while (true)
				{
					int bestAction = pQLA->InferenceBestAction(position);
					cout << bestAction << "->";

					if (bestAction == matrixDimension - 1) 
					{
						cout << "escaped." << endl; 
						break;
					}
					else position = bestAction;
				}
			}
		}

		pQLA->DeleteHeapMemory();
	}

	pQLA->Release();
	pUtils->Release();
	pIUnknown->Release();
	pIFactory->Release();
	::FreeLibrary(com);
	CoUninitialize();

	return 0;
}
