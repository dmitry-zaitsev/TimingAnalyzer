#pragma once
#include <vector>
#include "Node.h"
#include "parser_helper.h"
#include <cassert>
#include "Graph.h"
#include "LibElement.h"

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
		Node * start = new Node("start", "start node");
		do {
			string portName;
			double delay;
			valid = sp.read_input_delay (portName, delay);
			
			if (valid)
			{
				Node  * n = new Node (portName, "input");
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
		Node * endn = new Node("end", "end node");
		circuit * c = gr.getCircuit();
		do 
		{
			string portName ;
			double delay ;
			valid = sp.read_output_delay (portName, delay) ;
			if (valid)
			{
				Node  * n = new Node (portName, "output");
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

	virtual void GetCircuit(string veriFileName, circuit * cir) 
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
						Node * n = new Node(cellInst + " " + pinNetPairs[i].first, cellType);
						
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

		do 
		{
			LibParserCellInfo cell ;
			valid = lp.read_cell_info (cell) ;
			if (valid) 
			{
				LibElement * el = new LibElement(cell.name, cell.area, 
					cell.leakagePower, cell.isSequential, cell.dontTouch, cell.pins, cell.timingArcs);
			}
		} while (valid) ;
		return 0;
	}
};
	  
