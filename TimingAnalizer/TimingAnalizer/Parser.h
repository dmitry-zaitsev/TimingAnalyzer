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

	virtual void GetCircuit(string veriFileName, circuit * cir, Library lib) 
	{
		VerilogParser vp (veriFileName) ;
		bool valid;
		vector<Edge *> * vec = new vector<Edge *> ();
		do 
		{
			string net ;
			valid = vp.read_wire (net) ;
			if (valid)
			{
				Edge* ed;
				ed->Name = net;
				vec->push_back(ed);
			}
		} while (valid) ;
		
		do 
		{
			string cellType, cellInst ;
			vector<std::pair<string, string> > pinNetPairs ;
			valid = vp.read_cell_inst (cellType, cellInst, pinNetPairs) ;	
			
			if (valid) 
			{
				for (int i=0; i < pinNetPairs.size(); ++i) 
				{
					for (int j = 0; j < vec->size(); j++)
					{
						Edge * edg = (*vec)[j];
						//create node with name "cellInst outputName"
						vector<LibElement*> * l = lib.GetElements();
						LibElement * type = new LibElement();
						for (int i = 0; i < l->size(); i++)
						{
							if ((*l)[i]->GetName() == cellType)
							{
								type = (*l)[i];
								break;
							}
						}
						Node * n = new Node(cellInst + " " + pinNetPairs[i].first, type);
						if (pinNetPairs[i].second == edg->Name)
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
		  cout << endl ;
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
	  
