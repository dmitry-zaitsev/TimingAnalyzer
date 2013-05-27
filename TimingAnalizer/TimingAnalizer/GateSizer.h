#pragma once

#include "Graph.h"
#include "Library.h"

class GateSizerAlgorithm
{
public:
	virtual void resize() = 0;
};

class HJSizer : public GateSizerAlgorithm
{
private:
	Library* _lib;
	Graph* _graph;
protected:
	virtual bool resizePath(Node*);
	virtual LibElement* findBetterImpl(Node*);
	virtual void fastRecalcNeighbors(Node*, LibElement*);
public:
	HJSizer(Library*, Graph*);
	~HJSizer(void);

	virtual void resize();
};