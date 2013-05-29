#pragma once
#include <vector>
#include "Node.h"
#include "parser_helper.h"
#include <cassert>
#include "Graph.h"
#include "LibElement.h"
#include "Library.h"

class Parser
{
public:
	//constructors
	Parser(void);
	~Parser(void);
	
	//methods

	virtual double GetClock(string sdcFileName)
	{
		SdcParser sp (sdcFileName);
		string clockName;
		string clockPort;
		double period;//check
		bool valid = sp.read_clock (clockName, clockPort, period);
		assert(valid);
		return period;
	}

	virtual vector<Node *> * GetSdcIns(string sdcFileName, Graph  gr)
	{
		SdcParser sp (sdcFileName) ;	
		bool valid;
		vector<Node *> * vec = new vector<Node *>();
		circuit * c = gr.getCircuit();
		Node * start = new Node("start", NULL);
		do {
			string portName;
			double delay;
			valid = sp.read_input_delay (portName, delay);
			
			if (valid)
			{
				Node  * n = new Node (portName, NULL);
				vec->push_back(n);
				Edge * ed;
				ed->EndNode = n;
				ed->StartNode = start;
				(*c)[n]->push_back(ed);
				(*c)[start]->push_back(ed);
			}
		} while (valid) ;
		return vec;
	}

	virtual vector<Node *> * GetSdcOuts(string sdcFileName, Graph gr)
	{
		bool valid;
		vector<Node *> * vec = new vector<Node *>();
		SdcParser sp (sdcFileName) ;
		Node * endn = new Node("end", NULL);
		circuit * c = gr.getCircuit();
		do 
		{
			string portName ;
			double delay ;
			valid = sp.read_output_delay (portName, delay) ;
			if (valid)
			{
				Node  * n = new Node (portName, NULL);
				vec->push_back(n);
				Edge * ed;
				ed->StartNode = n;
				ed->EndNode = endn;
				(*c)[n]->push_back(ed);
				(*c)[endn]->push_back(ed);
			}
		} 
		while (valid) ;
		return vec;
	}

	virtual vector<Edge *> * GetEdges(string spefFileName)
	{
		SpefParser sp (spefFileName) ;

		string net ;
		double cap ;
		vector<Edge *> * vec = new vector<Edge *>();
		bool valid = sp.read_net_cap (net, cap) ;

		while (valid) 
		{
			Edge* ed;
			ed->Name = net;
			ed->Cap = cap;
			vec->push_back(ed);
			valid = sp.read_net_cap (net, cap) ;
		}
		return vec;
	}

	virtual void GetCircuit(string veriFileName, string spefFileName, circuit * cir, Library * lib) 
	{
		VerilogParser vp (veriFileName) ;
		bool valid;
		vector<Edge *> * vec = GetEdges(spefFileName);
		
		do 
		{
			/*
			Get Nodes and Start, End Nodes of Edges
			*/
			string cellType, cellInst ;
			vector<std::pair<string, string> > pinNetPairs ;
			valid = vp.read_cell_inst (cellType, cellInst, pinNetPairs);	
			
			if (valid) 
			{
				vector<LibElement*> * l = lib->GetElements();

				for (int iPair = 0; iPair < pinNetPairs.size(); ++iPair)
				{
						LibElement * type = NULL;
						LibParserPinInfo pin;

						for (int iElem = 0; iElem < l->size(); iElem++)
						{
							/*
							Find proper element in a library and choose proper pin
							*/
							if ((*l)[iElem]->GetName() == cellType)
							{
								type = (*l)[iElem];
								vector<LibParserPinInfo> pins = type->GetPins();

								for (int iPin = 0; iPin < pins.size(); iPin++)
								{
									if(pins[iPin].name == pinNetPairs[iPair].first)
									{
										pin = pins[iPin];
										break;
									}
								}
								break;
							}
						}

						for (int iEdge = 0; iEdge < vec->size(); iEdge++)
						{
							/*
							Find Edge which in pinNetPair and filled End and Start Point
							*/
							Edge * edg = (*vec)[iEdge];
							
							//create node
							Node * n = new Node(cellInst, type);
							n->SetPin(pin);

							if (pinNetPairs[iPair].second == edg->Name)
							{
								if (edg->StartNode == NULL)
								{
									edg->StartNode = n;
								}
								else
								{
									edg->EndNode = n;
								}
								//add element in result map
								if (!(*cir)[n])
								{
									(*cir)[n] = new vector<Edge *>();
								}
								(*cir)[n]->push_back(edg);
							}
						}
				}
			}
		} while (valid) ; 
	}

	virtual vector<LibElement *> * GetLib(string libFileName)
	{
		LibParser lp (libFileName) ;
		bool valid;
		vector<LibElement *> * v = new vector<LibElement *>();
		do 
		{
			LibParserCellInfo cell ;
			valid = lp.read_cell_info (cell) ;
			if (valid) 
			{
				LibElement * el = new LibElement(cell.name, cell.area, 
					cell.leakagePower, cell.isSequential, cell.dontTouch, cell.pins, cell.timingArcs);
				v->push_back(el);
			}
		} while (valid) ;
		return v;
	}
};
	  
