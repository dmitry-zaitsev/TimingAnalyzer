#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include "parser_helper.h"

using namespace std;

class LibElement
{
private:
	string _Name;
	double _Area;
	double _Leakage;
	bool _IsSeq;
	bool _DontT;
	vector<vector<float>> _Delay;
	vector<LibParserPinInfo> _Pins;
	vector<LibParserTimingInfo> _TimingInfo;
public:
	//constructors
	LibElement(string name, double area, double leakage,
		bool isSequential, bool dontTouch, 
		vector<LibParserPinInfo> p,
		vector<LibParserTimingInfo> t);
	LibElement(void);
	~LibElement(void);
	//methods

	virtual vector<LibParserPinInfo> GetPins()
	{
		return _Pins;
	}

	virtual vector<LibParserTimingInfo> GetArcs()
	{
		return _TimingInfo;
	}

	virtual double GetValueFromTable(double cap, double tt, LibParserLUT table)
	{
		double value;
		int cap1, cap2, tt1, tt2;

		for (int i = 0; i < table.loadIndices.size(); i++)
		{
			double curCap = table.loadIndices[i];
			if (curCap < cap)
			{
				cap1 = i;
			}
			else if (curCap > cap)
			{
				cap2 = i;
			}
			else
			{
				cap2 = i;
				cap1 = i;
			}
		}

		for (int i = 0; i < table.transitionIndices.size(); i++)
		{
			double curT = table.transitionIndices[i];
			if (curT < tt)
			{
				tt1 = i;
			}
			else if (curT > tt)
			{
				tt2 = i;
			}
			else
			{
				tt2 = i;
				tt1 = i;
			}
		}
		double val1, val2, val3, val4;
		val1 = table.tableVals[cap1][tt1];
		val2 = table.tableVals[cap2][tt1];
		val3 = table.tableVals[cap1][tt2];
		val4 = table.tableVals[cap2][tt2];
		value = (val1 + val2 + val3 + val4) / 4.0;
		return value;
	}

	virtual string GetName()
	{
		return _Name;
	}

	virtual void SetName(string n)
	{
		_Name = n;
	}

	virtual double GetArea()
	{
		return _Area;
	}

	virtual void SetArea(double a)
	{
		_Area = a;
	}

	virtual double GetLeakage()
	{
		return _Leakage;
	}

	virtual void SetLeakage(double a)
	{
		_Leakage = a;
	}

	virtual bool IsSequential()
	{
		return _IsSeq;
	}

	virtual bool DontTouch()
	{
		return _DontT;
	}
};

