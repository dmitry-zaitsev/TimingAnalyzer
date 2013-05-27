#include "stdafx.h"
#include "LibElement.h"
#include "parser_helper.h"

LibElement::LibElement(string name, double area, double leakage, 
					   bool isSequential, bool dontTouch, vector<LibParserPinInfo> p,
					   vector<LibParserTimingInfo> tInfo)
{
	_Name = name;
	_Area = area;
	_Leakage = leakage;
	_IsSeq = isSequential;
	_DontT = dontTouch;
	_Pins = p;
	_TimingInfo = tInfo;
}


LibElement::~LibElement(void)
{
}
