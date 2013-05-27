#include "stdafx.h"
#include "Library.h"
#include <vector>

Library::Library(vector<LibElement *> * es)
{
	_Elements = es;
}


Library::~Library(void)
{
}
