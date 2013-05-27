#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include "Node.h"
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

	~LibElement(void);
	//methods

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

