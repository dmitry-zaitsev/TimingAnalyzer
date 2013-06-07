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
		Node * n = iIter->first;
		if (n->GetPin().isInput && n->getType() != NULL)
		{
			ups = 0;
			downs = 0;
			for (int iEdge = 0; iEdge  < iIter->second->size(); iEdge ++)
			{
				cout << n->getName() << "\t" 
					<< (*iIter->second)[iEdge]->StartNode->getName() << "." 
					<< (*iIter->second)[iEdge]->StartNode->GetPin().name 
					<<"\t" << (*iIter->second)[iEdge]->EndNode->getName() 
					<< "." << (*iIter->second)[iEdge]->EndNode->GetPin().name << endl;
				if ((*iIter->second)[iEdge]->EndNode == n)
				{
					ups++;
				}
				if ((*iIter->second)[iEdge]->StartNode == n)
				{
					downs++;
				}
			}
			cout << "Alltogether is: " << iIter->second->size() << endl;
			if (ups != 1)
			{
				cout << "Node with Name " << n->getName() << "_" << n->GetPin().name << " has UPS: " << ups <<endl;
			}
			if (downs != 1)
			{
				cout << "Node with Name " << n->getName() << "_" << n->GetPin().name << " has DOWNS: " << downs << endl;
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