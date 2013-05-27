#include "StdAfx.h"
#include "GateSizer.h"

HJSizer::HJSizer(Library* lib, Graph* graph)
{
	_lib = lib;
	_graph = graph;
}


HJSizer::~HJSizer(void)
{
}


LibElement* HJSizer::findBetterImpl(Node* node)
{
	return 0;	//TODO implement
}


void HJSizer::fastRecalcNeighbors(Node* node, LibElement* newImpl)
{
	//TODO implement
}


bool HJSizer::resizePath(Node* start)
{
	bool negativeSlack = false;

	if(start->getSlack() < 0) {
		LibElement* better = findBetterImpl(start);

		if(better) {
			fastRecalcNeighbors(start, better);
			start->setImplementation(better);
		}

		negativeSlack = true;
	}

	vector<Edge*> edges = *_graph->getEdges(start);

	const int edgesCnt = edges.size();
	for(int i=0; i<edgesCnt; i++) {
		Edge* edge = edges[i];

		if(edge->EndNode) {
			negativeSlack |= resizePath(edge->EndNode);
		}
	}

	return negativeSlack;
}


void HJSizer::resize()
{
	bool negativeSlack;

	do {
		negativeSlack = false;

		const int insCnt = _graph->InsCount();

		for(int i=0; i<insCnt; i++) {
			negativeSlack |= resizePath(_graph->getInput(i));
		}
	} while(negativeSlack);
}