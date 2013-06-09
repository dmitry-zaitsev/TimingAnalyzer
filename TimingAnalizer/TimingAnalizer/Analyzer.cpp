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
	for (circuit::iterator iIter = c->begin(); iIter != c->end(); iIter++)
	{
		int ups, downs;
		string s;
		Node * n = iIter->first;
		if ((n->GetPin().isInput) && (n->getType() != NULL) && (!n->getType()->IsSequential()))
		{
			ups = 0;
			downs = 0;
			for (int iEdge = 0; iEdge  < iIter->second->size(); iEdge ++)
			{
				if ((*iIter->second)[iEdge]->EndNode == n)
				{
					ups++;
				}
				if ((*iIter->second)[iEdge]->StartNode == n)
				{
					downs++;
				}
			}
			if (ups < 1)
			{
				cout << "Alltogether is: " << iIter->second->size() << endl;
				cout << "Node with Name " << n->getName() << "_" << n->GetPin().name << " has UPS: " << ups <<endl;
				
				cin >> s;
			}
			if (downs != 1)
			{
				cout << "Alltogether is: " << iIter->second->size() << endl;
				cout << "Node with Name " << n->getName() << "_" << n->GetPin().name << " has DOWNS: " << downs << endl;
				cin >> s;
			}
		}
	}
	cout << "Done 1 Up and Down" << endl;
}

void Analyzer::TestFromInToOut()
{
	for (int i = 0; i < _Graph->InsCount(); i++)
	{
		Node * n = _Graph->getInput(i);
		cout << "FITO Test for " << n->getName() << " input:" << endl;
		TestRecursiveForFITO(n);
	}
	cout << "Thats all with FITO." << endl;
}

void Analyzer::TestRecursiveForFITO(Node * n)
{
	vector<Edge *> * ed = _Graph->getEdges(n);

	for (int j = 0; j < ed->size(); j++)
	{
		if ((*ed)[j]->StartNode == n)
		{
			Node * next = (*ed)[j]->EndNode;
			if (next->getType() == NULL)
			{
				cout << "We probably came to out and its name is " << next->getName() << endl;
			}
			else
			{
				TestRecursiveForFITO(next);
			}
		}
	}
}