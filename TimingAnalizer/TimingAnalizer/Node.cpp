#include "StdAfx.h"
#include "Node.h"

Node::Node(string name, LibElement * type)
{
	_AAT = 0.0;
	_RAT = 0.0;
	_Name = name;
	_Type = type;
}


Node::~Node(void)
{
}
