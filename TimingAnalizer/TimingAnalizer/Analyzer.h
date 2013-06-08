#pragma once
#include "Graph.h"
class Analyzer
{
public:

	//constructors
	Analyzer(Graph * gr);
	~Analyzer(void);

	//methods
	virtual void SetEdgesDirection()
	{
		circuit * c = _Graph->getCircuit();
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
						Node * stN = ed->StartNode;
						Node * enN = ed->EndNode;
						if(stN != n)
						{
							if (stN->getName() == n->getName())
							{
								Node * buf = stN;
								enN = buf;
								stN = n;
							}
						}
						else
						{
							if (ed->EndNode->getName() != n->getName())
							{
								Node * buf = enN;
								stN = buf;
								enN = n;
							}	
						}
						ed->StartNode = stN;
						ed->EndNode = enN;
					}
				}
			}
		}
	}

	virtual void CalcAAT()
	{
		_Graph->SetZeroAAT();
		for (int i = 0; i < _Graph->InsCount(); i++)
		{
			Node * inp = _Graph->getInput(i);
			vector<Edge *> * v = _Graph->getEdges(inp);
			for (int j = 0; j < v->size(); j++)
			{
				Edge * e = (*v)[j];
				CalcDelaysRecursive(e->EndNode, inp->GetTT(), inp->getAAT());
			}
		}
		cout << "CalcAAT() is done." << endl;
	}

	virtual void CalcRAT(double clk, circuit * c)
	{
		for (circuit::iterator i = c->begin(); i != c->end(); i++)
		{
			i->first->setRAT(clk);
		}

		for (int i = 0; i < _Graph->OutsCount(); i++)
		{
			Node * otp = _Graph->getOutput(i);
			otp->setRAT(clk);
			RecursiveCalcRAT(otp);
		}
	}

	void TestCheckInputEdges();

	void TestFromInToOut();

private:

	//fields
	Graph * _Graph;
	
	//test methods
	void TestRecursiveForFITO(Node * n);

	//methods
	virtual void CalcDelaysRecursive(Node * in, double tt, double aat)
	{
		if (in->getAAT() < aat)
		{
			in->setAAT(aat);
		}

		vector<Edge *> * edFromIn = _Graph->getEdges(in);
		vector<Edge *> * edFromOut = NULL;
		Node * out = NULL;
		double resDelay = 0.0;

		for (int i = 0; i < edFromIn->size(); i++)
		{
			Edge * iEd = (*edFromIn)[i];
			if (iEd->StartNode == in)
			{
				double cap = 0.0;
				out = iEd->EndNode;
				edFromOut = _Graph->getEdges(out);

				for (int j = 0; j  < edFromOut->size(); j ++)
				{
					/*
					Calc load cap
					*/
					
					Edge * jEd = (*edFromOut)[j];
					if (jEd->StartNode == out)
					{
						cap = cap + jEd->Cap + jEd->EndNode->GetPin().capacitance;
					}
				}

				double fallC = 0.0, riseC = 0.0, fallT = 0.0, riseT = 0.0;
				if (out->getType() != NULL)
				{
					LibElement * le = out->getType();
					vector<LibParserTimingInfo> vTI = le->GetArcs();
					for (int j = 0; j < vTI.size(); j++)
					{
						/*
						Get values from LUT Tables
						*/
						if((vTI[j].fromPin == in->GetPin().name) 
							|| (in->getType()->IsSequential() && vTI[j].toPin == out->GetPin().name)
							)
						{
							fallC = le->GetValueFromTable(cap, tt, vTI[j].fallDelay);
							riseC = le->GetValueFromTable(cap, tt, vTI[j].riseDelay);
						
							fallC >= riseC ? iEd->Delay = fallC : iEd->Delay = riseC;

							resDelay = iEd->Delay;
							cout << in->getAAT() << endl;
							double resAAT = !out->getType()->IsSequential() ? in->getAAT() + resDelay : resDelay;
							if (out->getAAT() < resAAT)
							{
								out->setAAT(resAAT);
							}
						
							fallT = le->GetValueFromTable(cap, tt, vTI[j].fallTransition);
							riseT = le->GetValueFromTable(cap, tt, vTI[j].riseTransition);
						
							fallT >= riseT ? tt = fallT : tt = riseT;
							//
							cout << "Result Delay for Edge between " << in->getName() 
								<< "." << in->GetPin().name << " and " << out->getName()
								<< "." << out->GetPin().name <<" is " << resDelay << endl;
							cout << "Result AAT for EndNode is " << resAAT;
							cout << ". TT is " << tt << endl << endl;
							
							vector<Edge *> * forRecur = _Graph->getEdges(out);
		
							for (int i = 0; i < forRecur->size(); i++)
							{
								/*
								if next Node is not Out then Recursive Calc
								*/
								Edge * ed = (*forRecur)[i];
								if (ed->StartNode == out)
								{
									if (ed->EndNode->getType() == NULL)
									{
										ed->EndNode->setAAT(resAAT);
									}
									else
									{
										CalcDelaysRecursive(ed->EndNode, tt, out->getAAT() + ed->Delay);
									}
								}	
							}
						}
					}
				}
			}
		}
	}
	
	virtual void RecursiveCalcRAT(Node * out)
	{
		vector <Edge *> * edges = _Graph->getEdges(out);
		double rat = 0.0;
		for (unsigned int i = 0; i < edges->size(); i++)
		{
			Edge * ed = edges->at(i);
			if (ed->StartNode != out)
			{
				rat = out->getRAT() - ed->Delay;
				if (ed->StartNode->getRAT() > rat)
				{
					ed->StartNode->setRAT(rat);
				}
				if (ed->StartNode->getType() != NULL)
				{
					RecursiveCalcRAT(ed->StartNode);
				}
			}
		}
	}
};

