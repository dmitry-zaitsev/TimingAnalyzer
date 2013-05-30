#pragma once
#include "Graph.h"
class Analyzer
{
public:

	//constructors
	Analyzer(Graph * gr);
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

	virtual void CalcDelaysRecursive(Node * in, double tt)
	{
		vector<Edge *> * edFromIn = _Graph->getEdges(in);
		vector<Edge *> * edFromOut = NULL;
		Node * out = NULL;
		float resDelay = 0.0;

		for (int i = 0; i < edFromIn->size(); i++)
		{
			Edge * iEd = (*edFromIn)[i];

			if (iEd->StartNode == in)
			{
				double cap;
				out = iEd->EndNode;
				edFromOut = _Graph->getEdges(out);
				
				for (int j = 0; j  < edFromOut->size(); j ++)
				{
					/*
					Calc load cap
					*/
					cap = 0.0;
					Edge * jEd = (*edFromOut)[j];
					if (jEd->StartNode == out)
					{
						cap = cap + jEd->Cap + jEd->EndNode->GetPin().capacitance;
					}
				}

				double fallC = 0.0, riseC = 0.0, fallT = 0.0, riseT = 0.0;
				LibElement * le = out->getType();
				vector<LibParserTimingInfo> vTI = le->GetArcs();
				
				for (int j = 0; j < vTI.size(); j++)
				{
					/*
					Get values from LUT Tables
					*/
					if(vTI[j].fromPin == in->getName())
					{
						fallC = le->GetValueFromTable(cap, tt, vTI[j].fallDelay);
						riseC = le->GetValueFromTable(cap, tt, vTI[j].riseDelay);
						
						fallC >= riseC ? iEd->Delay = fallC : iEd->Delay = riseC;

						resDelay = iEd->Delay;
						
						fallT = le->GetValueFromTable(cap, tt, vTI[j].fallTransition);
						riseT = le->GetValueFromTable(cap, tt, vTI[j].riseTransition);
						
						fallT >= riseT ? tt = fallT : tt = riseT;

						break;
					}
				}
			}
			else
			{
				float nAAT = iEd->StartNode->getAAT();
				if(nAAT > in->getAAT())
				{
					in->setAAT(nAAT);
				}
			}
		}
		float resAAT = in->getAAT() + resDelay;
		if (out->getAAT() < resAAT)
		{
			out->setAAT(resAAT);
		}
		if (out != _Graph->GetEndNode())
		{
			CalcDelaysRecursive(out, tt);
		}
	}

private:

	//fields
	Graph * _Graph;

	//methods
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

