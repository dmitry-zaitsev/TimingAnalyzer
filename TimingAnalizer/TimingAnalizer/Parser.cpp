#include "StdAfx.h"
#include "Parser.h"
#include <vector>

//constructors
Parser::Parser(void) { }
Parser::~Parser(void) { }

//methods
void Parser::GetInsideEdges(circuit * cir, vector<Edge *> * vec, Wire * w)	
{
	for (int iEdgeO = 0; iEdgeO < vec->size(); iEdgeO++)
	{
		Edge * edO = (*vec)[iEdgeO];
		if (edO->EndNode == w->Node)
		{
			for (int iEdgeI = 0; iEdgeI < vec->size(); iEdgeI++)
			{
				Edge * edI = (*vec)[iEdgeI];
				if (edI->StartNode == w->Node)
				{
					Edge * resEdge = new Edge();
					resEdge->Name = w->Node->getName();
					resEdge->StartNode = edO->StartNode;
					resEdge->EndNode = edI->EndNode;
					(*cir)[edO->StartNode]->push_back(resEdge);
					(*cir)[edI->EndNode]->push_back(resEdge);
				}
			}
		}
	}
}

void Parser::GetInputsEdges(circuit * cir, vector<Node *> * ins, vector<Edge *> * vec, Wire * w)
{
	for (int iInpNode = 0; iInpNode < ins->size(); iInpNode++)
	{
		if ((*ins)[iInpNode]->getName() == w->Node->getName())
		{
			for (int iWireEdge = 0; iWireEdge < vec->size(); iWireEdge++)
			{
				Edge * edg = new Edge();
				edg->Name = w->Node->getName();
				edg->StartNode = (*ins)[iInpNode];
				edg->EndNode = 	(*vec)[iWireEdge]->EndNode;
				Node * sN = edg->StartNode;
				Node * eN = edg->EndNode;

				if(!(*cir)[sN])
				{
					(*cir)[sN] = new vector<Edge *>();
				}
				(*cir)[sN]->push_back(edg);
				
				if(!(*cir)[eN])
				{
					(*cir)[eN] = new vector<Edge *>();
				}
				(*cir)[eN]->push_back(edg);
			}
		}
	}
}

void Parser::GetOutputsEdges(circuit * cir, vector<Node *> * outs, vector<Edge *> * vec, Wire * w)
{
	for (int iOutNode = 0; iOutNode < outs->size(); iOutNode++)
	{
		if ((*outs)[iOutNode]->getName() == w->Node->getName())
		{
			for (int iWireEdge = 0; iWireEdge < vec->size(); iWireEdge++)
			{
				Edge * edg = new Edge();
				edg->Name = w->Node->getName();

				if ((*vec)[iWireEdge]->StartNode == w->Node)
				{
					edg->StartNode = (*outs)[iOutNode];
					edg->EndNode = (*vec)[iWireEdge]->EndNode;
				}
				else
				{
					edg->StartNode = (*vec)[iWireEdge]->StartNode;
					edg->EndNode = (*outs)[iOutNode];
				}

				Node * sN = edg->StartNode;
				Node * eN = edg->EndNode;

				if(!(*cir)[sN])
				{
					(*cir)[sN] = new vector<Edge *>();
				}
				(*cir)[sN]->push_back(edg);
				
				if(!(*cir)[eN])
				{
					(*cir)[eN] = new vector<Edge *>();
				}
				(*cir)[eN]->push_back(edg);
			}
		}
	}
}

void Parser::GetWires(string spefFileName, Clock * clk, vector<Node *> * ins, vector<Node *> * outs)
{
	SpefParser sp (spefFileName) ;

	string net ;
	double cap ;
	bool valid = sp.read_net_cap (net, cap) ;

	while (valid) 
	{
		if (clk->Port != net)
		{
			Wire * w = new Wire();
			w->Node->setName(net);
			w->Cap = cap;

			for (int i = 0; i < ins->size(); i++)
			{
				Node * sn = (*ins)[i];
				if (w->Node->getName() == sn->getName())
				{
					w->IsInput = true;
				}
			}

			for (int i = 0; i < outs->size(); i++)
			{
				Node * en = (*outs)[i];
				if (w->Node->getName() == en->getName())
				{
					w->IsOutput = true;
				}
			}
			(*_Wires)[w] = new vector<Edge *>();
		}
		valid = sp.read_net_cap (net, cap) ;
	}
	cout << "Parsing completed: Wires" << endl;
}

void Parser::GetEdgesFromWires(circuit * cir, vector<Node *> * ins, vector<Node *> * outs)
{
	for (wires::iterator iWire = _Wires->begin(); iWire != _Wires->end(); iWire++)
	{
		if (!iWire->first->IsInput && !iWire->first->IsOutput)
		{
			GetInsideEdges(cir, iWire->second, iWire->first);
		} 
		else if (iWire->first->IsInput)
		{
			GetInputsEdges(cir, ins, iWire->second, iWire->first);
		}
		else if(iWire->first->IsOutput)
		{
			GetOutputsEdges(cir, outs, iWire->second, iWire->first);
		}
	}
}