#include "stdafx.h"
#include "Analyzer.h"
#include "Graph.h"
#include <vector>


Analyzer::Analyzer(void)
{
}


Analyzer::~Analyzer(void)
{
}

void RecursiveCalcAAT(Node * nd, Graph* gr)
{
	vector <Edge *> * edges = gr->getEdges(nd);
	float aat = 0.0;
	for (unsigned int i = 0; i < edges->size(); i++)
	{
		Edge * ed = edges->at(i);
		if (ed->EndNode != nd)
		{
			aat = ed->Delay + nd->getAAT();
			if (ed->EndNode->getAAT() < aat)
			{
				ed->EndNode->setAAT(aat);
			}
			RecursiveCalcAAT(ed->EndNode, gr);
		}
	}
}

void CalcAAT(Graph* gr)
{
	for (int i = 0; i < gr->InsCount(); i++)
	{
		Node * inp = gr->getInput(i);
		RecursiveCalcAAT(inp, gr);
	}
}

void RecursiveCalcRAT(Node * nd, Graph * gr)
{
	vector <Edge *> * edges = gr->getEdges(nd);
	float rat = 0.0;
	for (unsigned int i = 0; i < edges->size(); i++)
	{
		Edge * ed = edges->at(i);
		if (ed->StartNode != nd)
		{
			rat = nd->getRAT() - ed->Delay;
			if (ed->StartNode->getRAT() > rat)
			{
				ed->StartNode->setRAT(rat);
			}
			RecursiveCalcRAT(ed->StartNode, gr);
		}
	}
}

void CalcRAT(Graph * gr, float clk)
{
	for (int i = 0; i < gr->OutsCount(); i++)
	{
		Node * otp = gr->getOutput(i);
		otp->setRAT(clk);
		RecursiveCalcRAT(otp, gr);
	}
}