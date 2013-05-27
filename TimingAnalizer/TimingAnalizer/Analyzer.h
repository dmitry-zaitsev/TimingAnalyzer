#pragma once
#include "Graph.h"
class Analyzer
{
public:
	
	//constructors
	Analyzer(void);
	~Analyzer(void);
	
	//methods
	void CalcAAT(circuit cir);
	void CalcRAT(circuit cir);
private:
};

