#pragma once
#include "LibElement.h"
#include <vector>
class Library
{
public:
	Library(vector<LibElement *> * es);
	~Library(void);
private:
	vector<LibElement *> * _Elements;
};

