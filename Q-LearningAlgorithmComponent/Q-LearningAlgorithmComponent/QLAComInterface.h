#ifndef _QLAComInterface_h_
#define _QLAComInterface_h_

_declspec(selectany) GUID CLSID_QLA = { 0xc0a9401b, 0xeb07, 0x4df4, { 0xa2, 0xb1, 0xad, 0x78, 0x7b, 0x98, 0x69, 0x01 } };

interface __declspec(uuid("f2d6125c-c474-4c52-9bfb-05973c7ad2e5")) IQLAUnknown
{
public:
	virtual bool QueryInterface(REFIID riid, void** pValue) = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

interface __declspec(uuid("9a574129-acac-40d9-9f0b-8c33bb3fed12")) IQLAClassFactory : public IQLAUnknown
{
public:
	virtual bool CreateInstance(REFIID riid, void** pValue) = 0;
};

interface __declspec(uuid("6ab345b1-15e1-452b-be5e-fe292632c337")) IQLearningAlgorithm : public IQLAUnknown
{
public:
	virtual void InitVariables(int _matrixDimension, int _maxEpisode, double _alpha, double _gamma) = 0;
	virtual void TrainingModel(int initState, bool isPrintProcess) = 0;
	virtual int InferenceBestAction(int nowState) = 0;
	virtual void DeleteHeapMemory() = 0;
private:
	virtual void InitQMatrix() = 0;
	virtual void InitRandomRMatrix() = 0;
	virtual void GetPossibleAction(int state) = 0;
	virtual double GetMaxQ(double** qMatrix, int state) = 0;
	virtual int EpisodeIterator(int initState, double** qMatrix, bool isPrintProcess) = 0;
};

interface __declspec(uuid("ea7013c3-f312-4cc7-96ca-468edccbc507")) ICommonUtils : public IQLAUnknown
{
public:
	virtual void PrintMatrix(double** matrix, int rows, int columns) = 0;
	virtual void PrintUsage() = 0;
};

#endif
