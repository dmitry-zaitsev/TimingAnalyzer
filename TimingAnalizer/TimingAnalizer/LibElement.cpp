#include "stdafx.h"
#include "LibElement.h"


LibElement::LibElement(string name, double area, double leakage, bool isSequential, bool dontTouch)
{
	_Name = name;
	_Area = area;
	_Leakage = leakage;
	_IsSeq = isSequential;
	_DontT = dontTouch;
}


LibElement::~LibElement(void)
{
}
