#pragma once
#include "LibElement.h"
#include <vector>
class Library
{
public:
	Library(vector<LibElement *> * es);
	~Library(void);
	virtual vector<LibElement *> * GetElements()
	{
		return _Elements;
	}
private:
	vector<LibElement *> * _Elements;
};

