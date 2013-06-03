#include "stdafx.h"
#include "Analyzer.h"
#include "Graph.h"
#include <vector>


Analyzer::Analyzer(Graph * gr)
{
	_Graph = gr;
}


Analyzer::~Analyzer(void)
{
}

/*List of test methods:
1)Must be one up-edge and one down-edge for input pin-node(if parser valid)
2)Can go through Graph from input to output
*/

void Analyzer::TestCheckInputEdges()
{
	circuit * c = _Graph->getCircuit();
	for (circuit::iterator i = c->begin(); i != c->end(); i++)
	{
		if (i->first->GetPin().isInput)
		{
			int ups = 0, downs = 0;
			for (int j = 0; j  < i->second->size(); j ++)
			{
				if ((*i->second)[j]->EndNode == i->first)
				{
					ups++;
				}
				if ((*i->second)[j]->StartNode == i->first)
				{
					downs++;
				}
			}
			if (ups != 1)
			{
				cout << "Node with Name " << i->first->getName() << "has more or less than one UPS.";
			}
			if (downs != 1)
			{
				cout << "Node with Name " << i->first->getName() << "has more than one DOWNS.";
			}
		}
	}
	cout << "Done 1 Up and Down" << endl;
}

void Analyzer::TestFromInToOut(Graph * gr)
{
	for (int i = 0; i < gr->InsCount(); i++)
	{
		Node * n = gr->getInput(i);
		cout << "FITO Test for" << n->getName() << "input:" << endl;
		TestRecursiveForFITO(n);
	}
	cout << "Thats all with FITO." << endl;
}

void Analyzer::TestRecursiveForFITO(Node * n)
{
	vector<Edge *> * ed = _Graph->getEdges(n);
	cout << "We probably came to out and its name is " << endl;

	for (int j = 0; j < ed->size(); j++)
	{
		if ((*ed)[j]->StartNode == n)
		{
			Node * next = (*ed)[j]->EndNode;
			if (next->getType() == NULL)
			{
				cout << next->getName() << endl;
			}
			else
			{
				TestRecursiveForFITO(next);
			}
		}
	}
}