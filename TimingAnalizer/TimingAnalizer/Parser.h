#pragma once
#include <vector>
#include <map>
#include "Node.h"
#include "parser_helper.h"
#include <cassert>
#include "Graph.h"
#include "LibElement.h"
#include "Library.h"

struct Wire
{
	bool IsOutput;
	bool IsInput;
	Node * Node;
	double Cap;
};

typedef map<Wire *, vector<Edge *> *> wires;

struct WCompare
{
	bool operator()(Wire * const &a, Wire * const &b) const
	{
		return (a->Node->getName()).compare(b->Node->getName()) > 0;
	}
};

class Parser
{
private:
	//fields
	wires * _Wires;

	//methods
	void GetInsideEdges(circuit * cir, vector<Edge *> * vec, Wire * w);

	void GetInputsEdges(circuit * cir, vector<Node *> * ins,vector<Edge *> * vec, Wire * w);

	void GetOutputsEdges(circuit * cir, vector<Node *> * outs,vector<Edge *> * vec, Wire * w);

	void GetWires(string spefFileName, Clock * clk, vector<Node *> * ins, vector<Node *> * outs);

	void GetEdgesFromWires(circuit * cir, vector<Node *> * ins, vector<Node *> * outs);

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
		cout << "Parsing completed: Clock" << endl;
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
		cout << "Parsing completed: SdcIns" << endl;
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
				//(*cir)[n] = NULL;
				vec->push_back(n);
			}
		} 
		while (valid) ;
		cout << "Parsing completed: SdcOuts" << endl;
		return vec;
	}

	virtual void GetCircuit(VerilogParser &vp, string spefFileName, 
		circuit * cir, Library * lib, Clock * clk, vector<Node *> * ins, vector<Node *> * outs) 
	{
		bool valid;
		GetWires(spefFileName, clk, ins, outs);
		
		do 
		{
			/*
			Get Nodes and Fill Wires
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
						//create node
						Node * n = new Node(cellInst, type);
						n->SetPin(pin);
						(*cir)[n] = new vector<Edge *> ();
						for (wires::iterator iWire = _Wires->begin(); iWire != _Wires->end(); iWire++)
						{
							/*
							Filled Wire Map pinNetPair and filled
							*/
							
							if (pinNetPairs[iPair].second == iWire->first->Node->getName())
							{
								Edge * edg = new Edge();

								if (iWire->first->IsInput 
									|| (iWire->first->IsOutput && n->GetPin().isInput)
									|| (!iWire->first->IsInput && !iWire->first->IsOutput && n->GetPin().isInput)
									)
								{
									edg->StartNode = iWire->first->Node;
									edg->EndNode = n;
								}
								else if ((iWire->first->IsOutput && !n->GetPin().isInput)
									|| (!iWire->first->IsInput && !iWire->first->IsOutput && !n->GetPin().isInput)
									)
								{
									edg->StartNode = n;
									edg->EndNode = iWire->first->Node;
								}

								if (!iWire->second)
								{
									iWire->second = new vector<Edge *> ();
								}

								iWire->second->push_back(edg);

								/*if(iWire->first->IsInput)
								{
									edg->StartNode = iWire->first->Node;
									edg->EndNode = n;
								}
								else if (iWire->first->IsOutput)
								{
									if (n->GetPin().isInput)
									{
										edg->StartNode = iWire->first->Node;
										edg->EndNode = n;
									}
									else
									{
										edg->StartNode = n;
										edg->EndNode = iWire->first->Node;
									}
								}
								else
								{
									if (n->GetPin().isInput)
									{
										edg->StartNode = iWire->first->Node;
										edg->EndNode = n;
									}
									else
									{
										edg->StartNode = n;
										edg->EndNode = iWire->first->Node;
									}
								}*/
							}
						}
				}
			}
		} while (valid) ;
		GetEdgesFromWires(cir, ins, outs);
		cout << "Parsing completed: Circuit" << endl;
	}

	virtual void GetInsts(circuit * cir, instances * insts)
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
		cout << "Parsing completed: Insts" << endl;
	}

	virtual void GetInners(circuit * cir, instances * insts)
	{
		for (circuit::iterator iIter = cir->begin(); iIter != cir->end(); iIter++)
		{
			Node * n = iIter->first;
			
			if ((n->GetPin().isInput) && (n->getType() != NULL) && !(n->getType()->IsSequential()))
			{	
				vector<LibParserTimingInfo> vecTI = n->getType()->GetArcs();
				
				for (int iTI = 0; iTI < vecTI.size(); iTI++)
				{
					/*
					Find TimingArc with fromPin equals our Pin in current Node
					*/
					string fP = vecTI[iTI].fromPin;
					string tP = vecTI[iTI].toPin;
					if (fP == n->GetPin().name)
					{
						Edge * edForIn = new Edge();
						edForIn->Name = n->getName()+ "_" + fP + "_" + tP;
						edForIn->StartNode = n;

						vector<Node *> * vecN = (*insts)[n->getName()];
						for (int iNode = 0; iNode < vecN->size(); iNode++)
						{
							/*
							Find in Instances Node with Pin equals toPin in current TimingArc
							*/
							Node * en = (*vecN)[iNode];
							if (en->GetPin().name == tP)
							{
								edForIn->EndNode = en;
								if (!iIter->second)
								{
									iIter->second = new vector<Edge *> ();
								}
								iIter->second->push_back(edForIn);
								(*cir)[en]->push_back(edForIn);
							}
						}
					}	
				}
			}
			else if (!(n->GetPin().isInput) && (n->getType() != NULL) && (n->getType()->IsSequential()))
			{
				vector<LibParserTimingInfo> vecTI = n->getType()->GetArcs();
				
				for (int iTI = 0; iTI < vecTI.size(); iTI++)
				{
					/*
					Find TimingArc with toPin equals our Pin in current Node
					*/
					string fP = vecTI[iTI].fromPin;
					string tP = vecTI[iTI].toPin;
					if (tP == n->GetPin().name)
					{
						Edge * edForIn = new Edge();
						edForIn->Name = n->getName()+ "_" + fP + "_" + tP;
						edForIn->EndNode = n;

						vector<Node *> * vecN = (*insts)[n->getName()];
						for (int iNode = 0; iNode < vecN->size(); iNode++)
						{
							/*
							Find in Instances Node with Pin equals toPin in current TimingArc
							*/
							Node * sn = (*vecN)[iNode];
							if (sn->GetPin().isInput)
							{
								edForIn->StartNode = sn;
								if (!iIter->second)
								{
									iIter->second = new vector<Edge *> ();
								}
								iIter->second->push_back(edForIn);
								(*cir)[sn]->push_back(edForIn);
							}
						}
					}	
				}
			}
		}
		cout << "Parsing completed: Inners" << endl;
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
			cout << "Parsing completed: Library" << endl;
			return v;
		}
};
	  
