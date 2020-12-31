#include <iostream>
#include <windows.h>
#include <time.h>
#include "QLAComInterface.h"

using namespace std;
long theCounter = 0;

class QLearningAlgorithm : public IQLearningAlgorithm, public ICommonUtils
{
public:
	QLearningAlgorithm()
	{
		theCounter++;
	}

	virtual ~QLearningAlgorithm()
	{
		theCounter--;
	}

	virtual bool QueryInterface(REFIID riid, void** pValue)
	{
		bool isUUIDFound = false;

		if (IsEqualIID(riid, __uuidof(IQLAUnknown)) || IsEqualIID(riid, __uuidof(IQLearningAlgorithm)))
		{
			*pValue = static_cast<IQLearningAlgorithm*>(this);
			isUUIDFound = true;
		}
		else if (IsEqualIID(riid, __uuidof(ICommonUtils)))
		{
			*pValue = static_cast<ICommonUtils*>(this);
			isUUIDFound = true;
		}

		if(isUUIDFound) this->AddRef();

		return isUUIDFound;
	}

	virtual void AddRef()
	{
		theCounter++;
	}

	virtual void Release()
	{
		if (theCounter > 0) theCounter--;

		if (theCounter == 0) delete this;
	}

	virtual void PrintMatrix(double** matrix, int rows, int columns)
	{
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < columns; ++j)
			{
				cout << matrix[i][j] << "\t";
			}

			cout << endl;
		}
	}

	virtual void PrintUsage()
	{
		cout << "+------------------------------------------------------------------------------------+" << endl;
		cout << "|                                                                                    |" << endl;
		cout << "|    A component of Q-learning algorithm implemented in C++  (2019534002-###@###)    |" << endl;
		cout << "|                                                                                    |" << endl;
		cout << "+--------+--------+-----------------+------------+--------+--------+-----------------+" << endl;
		cout << "| Usage: | client | matrixDimension | maxEpisode | alpha  | gamma  | isPrintProgress |" << endl;
		cout << "+--------+--------+-----------------+------------+--------+--------+-----------------+" << endl;
		cout << "|  Type: | string |     integer     |   integer  | double | double |     boolean     |" << endl;
		cout << "+--------+--------+-----------------+------------+--------+--------+-----------------+" << endl;
		cout << "|        |   Without print progress |     [QLAComponentClient] 6 1000 0.5 0.9 0      |" << endl;
		cout << "|        +--------------------------+------------------------------------------------+" << endl;
		cout << "|  e.g.: |      With print progress |     [QLAComponentClient] 6 1000 0.5 0.9 1      |" << endl;
		cout << "|        +--------------------------+------------------------------------------------+" << endl;
		cout << "|        | Output progress log file | [QLAComponentClient] 6 1000 0.5 0.9 1 >> q.log |" << endl;
		cout << "+--------+--------------------------+------------------------------------------------+" << endl;
		cout << "|  Tips: | Run this program from windows PowerShell (Recommended) or command prompt. |" << endl;
		cout << "+--------+---------------------------------------------------------------------------+" << endl;
	}

	virtual void InitVariables(int _matrixDimension, int _maxEpisode, double _alpha, double _gamma)
	{
		matrixRow = _matrixDimension;
		matrixColumn = _matrixDimension;
		stateNum = _matrixDimension;
		actionNum = _matrixDimension;
		destinationState = _matrixDimension - 1;
		possibleActionArrayLength = 2 * _matrixDimension;
		possibleActionArray = new int[possibleActionArrayLength];
		maxEpisode = _maxEpisode;
		alpha = _alpha;
		gamma = _gamma;
		InitQMatrix();
		InitRandomRMatrix();
	}

	virtual void TrainingModel(int initState, bool isPrintProcess)
	{
		int initialState = initState;
		srand((unsigned)time(NULL));

		if (isPrintProcess)
		{
			cout << "[INFO] Start training..." << endl;

			for (int i = 0; i < maxEpisode; ++i)
			{
				cout << "[INFO] Episode: " << i << endl;
				initialState = EpisodeIterator(initialState, qMatrix, isPrintProcess);
				cout << "Updated matrix Q: " << endl;
				PrintMatrix(qMatrix, matrixRow, matrixColumn);
			}
		}
		else
		{
			for (int i = 0; i < maxEpisode; ++i) initialState = EpisodeIterator(initialState, qMatrix, isPrintProcess);
		}

		cout << "Convergent matrix Q:" << endl;
		PrintMatrix(qMatrix, matrixRow, matrixColumn);
	}

	virtual int InferenceBestAction(int nowState)
	{
		double tempMaxQ = 0;
		int bestAction = 0;

		for (int i = 0; i < actionNum; ++i)
		{
			if (qMatrix[nowState][i] > tempMaxQ)
			{
				tempMaxQ = qMatrix[nowState][i];
				bestAction = i;
			}
		}

		return bestAction;
	}

	virtual void DeleteHeapMemory()
	{
		delete[]possibleActionArray;

		for (int i = 0; i < matrixRow; ++i)
		{
			delete[]qMatrix[i];
			delete[]rMatrix[i];
		}

		delete[]qMatrix;
		delete[]rMatrix;
		cout << "Delete requested heap memory." << endl;
	}

private:
	int matrixRow = 6, matrixColumn = 6, maxEpisode = 1000;
	int stateNum = 0, actionNum = 0, destinationState = 5;
	int* possibleActionArray = {};
	int possibleActionArrayLength = 12;
	int possibleActionNum = 0;
	double** qMatrix = {}, ** rMatrix = {};
	double alpha = 0.5, gamma = 0.9;

	virtual void InitQMatrix()
	{
		qMatrix = new double* [matrixRow];

		for (int i = 0; i < matrixRow; i++)
		{
			qMatrix[i] = new double[matrixColumn];

			for (int j = 0; j < matrixColumn; j++) qMatrix[i][j] = 0;
		}

		cout << "Initial matrix Q:" << endl;
		PrintMatrix(qMatrix, matrixRow, matrixColumn);
	}

	virtual void InitRandomRMatrix()
	{
		rMatrix = new double* [matrixRow];
		srand((unsigned)time(NULL));

		for (int i = 0; i < matrixRow; i++)
		{
			rMatrix[i] = new double[matrixColumn];

			for (int j = 0; j < matrixColumn; j++) rMatrix[i][j] = -1;

			int c = rand() % (matrixColumn - 1);
			rMatrix[i][c] = 0;

			while (true)
			{
				int c = rand() % (matrixColumn - 1);

				if (rMatrix[i][c] == -1)
				{
					rMatrix[i][c] = 0;
					break;
				}
			}
		}

		int r = rand() % matrixRow;
		rMatrix[r][matrixColumn - 1] = 100;
		int num = 1;

		while (true)
		{
			int r = rand() % matrixRow;

			if (rMatrix[r][matrixColumn - 1] == -1)
			{
				rMatrix[r][matrixColumn - 1] = 100;

				if (++num == 3) break;
			}
		}

		cout << "Initial random matrix R:" << endl;
		PrintMatrix(rMatrix, matrixRow, matrixColumn);
	}

	virtual void GetPossibleAction(int state)
	{
		possibleActionNum = 0;

		for (int i = 0; i < actionNum; i++)
		{
			if (rMatrix[state][i] >= 0)
			{
				possibleActionArray[possibleActionNum] = i;
				possibleActionNum++;
			}
		}
	}

	virtual double GetMaxQ(double** qMatrix, int state)
	{
		double tempMax = 0;

		for (int i = 0; i < actionNum; ++i)
		{
			if ((rMatrix[state][i] >= 0) && (qMatrix[state][i] > tempMax)) tempMax = qMatrix[state][i];
		}

		return tempMax;
	}

	virtual int EpisodeIterator(int initState, double** qMatrix, bool isPrintProcess)
	{
		double maxQ, qBefore;
		int nextAction, step = 0;

		if (isPrintProcess)
		{
			while (true)
			{
				cout << ">> Step " << step << ": init state: " << initState << endl;
				memset(possibleActionArray, 0, possibleActionArrayLength * sizeof(int));
				GetPossibleAction(initState);
				nextAction = possibleActionArray[rand() % possibleActionNum];
				cout << ">> Step " << step << ": next action: " << nextAction << endl;
				maxQ = GetMaxQ(qMatrix, nextAction);
				qBefore = qMatrix[initState][nextAction];
				qMatrix[initState][nextAction] = (1 - alpha) * qBefore + alpha * (rMatrix[initState][nextAction] + gamma * maxQ);

				if (nextAction == destinationState)
				{
					initState = rand() % stateNum;
					break;
				}
				else initState = nextAction;

				step++;
			}
		}
		else
		{
			while (true)
			{
				memset(possibleActionArray, 0, possibleActionArrayLength * sizeof(int));
				GetPossibleAction(initState);
				nextAction = possibleActionArray[rand() % possibleActionNum];
				maxQ = GetMaxQ(qMatrix, nextAction);
				qBefore = qMatrix[initState][nextAction];
				qMatrix[initState][nextAction] = (1 - alpha) * qBefore + alpha * (rMatrix[initState][nextAction] + gamma * maxQ);

				if (nextAction == destinationState)
				{
					initState = rand() % stateNum;
					break;
				}
				else initState = nextAction;

				step++;
			}
		}

		return initState;
	}
};

class Factory : public IQLAClassFactory
{
public:
	Factory()
	{
		theCounter++;
	}

	virtual ~Factory()
	{
		theCounter--;
	}

	virtual bool QueryInterface(REFIID riid, void** pValue)
	{
		if (IsEqualIID(riid, __uuidof(IQLAUnknown)) || IsEqualIID(riid, __uuidof(IQLAClassFactory)))
		{
			*pValue = static_cast<IQLAClassFactory*>(this);
			this->AddRef();

			return true;
		}

		return false;
	}

	virtual void AddRef()
	{
		theCounter++;
	}

	virtual void Release()
	{
		if (theCounter > 0) theCounter--;

		if (theCounter == 0) delete this;
	}

	virtual bool CreateInstance(REFIID riid, void** pValue)
	{
		QLearningAlgorithm* pQLearningAlgorithm = new QLearningAlgorithm;

		return pQLearningAlgorithm->QueryInterface(riid, pValue);
	}
};

HMODULE g_hModule;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	g_hModule = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ FAR LPVOID* pValue)
{
	if (IsEqualIID(rclsid, CLSID_QLA) && IsEqualGUID(riid, __uuidof(IQLAClassFactory)))
	{
		IQLAClassFactory* pFactory = new Factory;

		if(pFactory->QueryInterface(riid, pValue)) return S_OK;
	}

	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllRegisterServer(void)
{
	char chrModulePath[MAX_PATH] = {};
	GetModuleFileNameA(g_hModule, chrModulePath, ARRAYSIZE(chrModulePath));

	HKEY pHKey, pCLSIDKey;

	if (ERROR_SUCCESS == RegOpenKeyA(HKEY_CLASSES_ROOT, "CLSID", &pHKey))
	{
		if (ERROR_SUCCESS == RegCreateKeyA(pHKey, "{c0a9401b-eb07-4df4-a2b1-ad787b986901}", &pCLSIDKey))
		{
			HKEY pInSvr32Key;

			if (ERROR_SUCCESS == RegCreateKeyA(pCLSIDKey, "InprocServer32", &pInSvr32Key))
			{
				RegSetValueA(pInSvr32Key, NULL, REG_SZ, chrModulePath, strlen(chrModulePath) + 1);
				RegCloseKey(pInSvr32Key);
			}
			else cout << "Reg create sub key failed." << endl;

			RegCloseKey(pCLSIDKey);
		}
		else cout << "Reg create key failed." << endl;

		RegCloseKey(pHKey);
	}
	else cout << "Reg open key failed." << endl;

	if (ERROR_SUCCESS == RegCreateKeyA(HKEY_CLASSES_ROOT, "QLAComponent", &pHKey))
	{
		if (ERROR_SUCCESS == RegCreateKeyA(pHKey, "CLSID", &pCLSIDKey))
		{
			RegSetValueA(pCLSIDKey, NULL, REG_SZ, "{c0a9401b-eb07-4df4-a2b1-ad787b986901}", strlen("{c0a9401b-eb07-4df4-a2b1-ad787b986901}") + 1);
		}
		else cout << "Reg create key failed." << endl;
	}

	return 0;
}

STDAPI DllUnregisterServer(void)
{
	HKEY pHKey;

	if (ERROR_SUCCESS == RegOpenKeyA(HKEY_CLASSES_ROOT, "CLSID", &pHKey))
	{
		RegDeleteKeyA(pHKey, "{c0a9401b-eb07-4df4-a2b1-ad787b986901}\\InprocServer32");
		RegDeleteKeyA(pHKey, "{c0a9401b-eb07-4df4-a2b1-ad787b986901}");
		RegCloseKey(pHKey);
	}
	else cout << "Reg open key failed." << endl;

	if (ERROR_SUCCESS == RegOpenKeyA(HKEY_CLASSES_ROOT, "QLAComponent", &pHKey))
	{
		RegDeleteKeyA(pHKey, "CLSID");
	}
	else cout << "Reg open key failed." << endl;

	RegDeleteKeyA(HKEY_CLASSES_ROOT, "QLAComponent");

	return 0;
}

__control_entrypoint(DllExport)
STDAPI DllCanUnloadNow(void)
{
	if (0 == theCounter) return S_OK;

	return S_FALSE;
}
