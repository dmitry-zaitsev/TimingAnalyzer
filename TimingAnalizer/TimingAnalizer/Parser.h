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
	virtual Clock * GetClock(SdcParser &sp)
	{
		string clockName;
		string clockPort;
		double period;
		bool valid = sp.read_clock (clockName, clockPort, period);
		assert(valid);
		Clock * clk = new Clock();
		clk->Name = clockName;
		clk->Port = clockPort;
		clk->Period = period;
		return clk;
	}

	virtual vector<Node *> * GetSdcIns(SdcParser &sp, circuit * cir)
	{

		bool valid;
		vector<Node *> * vec = new vector<Node *>();

		do {
			string portName;
			double delay;
			valid = sp.read_input_delay (portName, delay);
			
			if (valid)
			{
				Node  * n = new Node (portName, NULL);
				n->setAAT(delay);
				//(*cir)[n] = NULL;
				cir->insert(pair<Node *, vector<Edge *> *>(n, NULL));
				vec->push_back(n);
			}
		} while (valid) ;

		do 
		{
			string portName ;
			string driverSize ;
			string driverPin ;
			double inputTransitionFall ;
			double inputTransitionRise ;
    
			valid = sp.read_driver_info (portName, driverSize, driverPin,
										 inputTransitionFall, inputTransitionRise) ;

			if (valid) 
			{
				for (int i = 0; i < vec->size(); i++)
				{
					Node * nInV = (*vec)[i];
					if(nInV->getName() == portName)
					{
						inputTransitionFall >= inputTransitionRise ? 
							nInV->SetTT(inputTransitionFall): 
							nInV->SetTT(inputTransitionRise);
						break;
					}
				}
			}
		} while (valid) ;
		return vec;
	}

	virtual vector<Node *> * GetSdcOuts(SdcParser &sp, circuit * cir)
	{
		bool valid;
		vector<Node *> * vec = new vector<Node *>();

		do 
		{
			string portName ;
			double delay ;
			valid = sp.read_output_delay (portName, delay) ;
			if (valid)
			{
				Node  * n = new Node (portName, NULL);
				(*cir)[n] = NULL;
				vec->push_back(n);
			}
		} 
		while (valid) ;
		return vec;
	}

	virtual vector<Edge *> * GetEdges(string spefFileName, Clock * clk, vector<Node *> * ins, vector<Node *> * outs)
	{
		SpefParser sp (spefFileName) ;

		string net ;
		double cap ;
		vector<Edge *> * vec = new vector<Edge *>();
		bool valid = sp.read_net_cap (net, cap) ;

		while (valid) 
		{
			if (clk->Port != net)
			{
				Edge* ed = new Edge;
				ed->Name = net;
				ed->Cap = cap;
				ed->StartNode = NULL;
				ed->EndNode = NULL;

				for (int i = 0; i < ins->size(); i++)
				{
					Node * sn = (*ins)[i];
					if (ed->Name == sn->getName())
					{
						ed->StartNode = sn;
					}
				}

				for (int i = 0; i < outs->size(); i++)
				{
					Node * en = (*outs)[i];
					if (ed->Name == en->getName())
					{
						ed->EndNode = en;
					}
				}
				vec->push_back(ed);
			}
			valid = sp.read_net_cap (net, cap) ;
		}
		return vec;
	}

	virtual void GetCircuit(VerilogParser &vp, string spefFileName, 
		circuit * cir, Library * lib, Clock * clk, vector<Node *> * ins, vector<Node *> * outs) 
	{
		bool valid;
		vector<Edge *> * vec = GetEdges(spefFileName, clk, ins, outs);
		
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
								else if (edg->EndNode == NULL)
								{
									edg->EndNode = n;
								}
								if (edg->StartNode != NULL && edg->EndNode != NULL)
								{
									//add elements in result map
									Node * sn = edg->StartNode;
									if (!(*cir)[sn])
									{
										(*cir)[sn] = new vector<Edge *>();
									}
									(*cir)[sn]->push_back(edg);

									Node * en = edg->EndNode;
									if (!(*cir)[en])
									{
										(*cir)[en] = new vector<Edge *>();
									}
									(*cir)[en]->push_back(edg);

									//cout << edg->Name << " SP: " << edg->StartNode->getName() << " EP: " << edg->EndNode->getName() << endl;
								}
							}
						}


				}
			}
		} while (valid) ; 
	}

	virtual void GetInners(circuit * cir, instances * insts)
	{
		for (circuit::iterator i = cir->begin(); i != cir->end(); i++)
		{
			string cellInst = i->first->getName();
			if(!(*insts)[cellInst])
			{
				(*insts)[cellInst] = new vector<Node *>();
			}
			(*insts)[cellInst]->push_back(i->first);
		}
	}

	virtual void GoToRightPlace(VerilogParser &vp)
	{
	  string moduleName ;
	  bool valid = vp.read_module (moduleName) ;
	  assert (valid);

	  do 
	  {
		string primaryInput ;
		valid = vp.read_primary_input (primaryInput) ;
	  } while (valid) ;
	  do 
	  {
		string primaryOutput ;
		valid = vp.read_primary_output (primaryOutput) ;
	  } while (valid) ;
	  do 
	  {
		string net ;
		valid = vp.read_wire (net) ;
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
	  
