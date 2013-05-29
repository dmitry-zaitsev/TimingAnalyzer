#pragma once
#include "Graph.h"
class Analyzer
{
public:
	
	//constructors
	Analyzer(void);
	~Analyzer(void);
	
	//methods
	virtual void CheckEdgesDirection(circuit * c)
	{
		for (circuit::iterator i = c->begin(); i != c->end(); i++)
		{
			Node * n = i->first;
			if (n->getType() != NULL)
			{
				if (n->GetPin().isInput)
				{
					vector<Edge *> * v = i->second;
					for (int iEdge = 0; iEdge < v->size(); iEdge++)
					{
						Edge * ed = (*v)[iEdge];
						if(ed->StartNode != n)
						{
							if (ed->StartNode->getName() == n->getName())
							{
								ed->EndNode = ed->StartNode;
								ed->StartNode = n;
							}
						}
						else
						{
							if (ed->EndNode->getName() != n->getName())
							{
								ed->StartNode = ed->EndNode;
								ed->EndNode = n;
							}	
						}
					}
				}
			}
		}
	}

	virtual void CalcAAT(Graph* gr)
	{
		for (int i = 0; i < gr->InsCount(); i++)
		{
			Node * inp = gr->getInput(i);
			RecursiveCalcAAT(inp, gr);
		}
	}

	virtual void CalcRAT(Graph * gr, float clk)
	{
		for (int i = 0; i < gr->OutsCount(); i++)
		{
			Node * otp = gr->getOutput(i);
			otp->setRAT(clk);
			RecursiveCalcRAT(otp, gr);
		}
	}
private:
	virtual void RecursiveCalcAAT(Node * nd, Graph* gr)
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

	virtual void RecursiveCalcRAT(Node * nd, Graph * gr)
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
};

