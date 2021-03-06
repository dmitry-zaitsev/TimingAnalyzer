#pragma once
#include "Node.h"
#include <map>
#include <vector>

using namespace std;

struct Edge
{
	Node *StartNode;
	Node *EndNode;
	double Delay;
	string Name;
	double Cap;
};

struct Clock
{
	string Name;
	string Port;
	double Period;
};

typedef map<Node *, vector<Edge *> *, Compare> circuit;
typedef map<string, vector<Node *> *> instances;

class Graph
{
private:
	instances * _Instances;
	circuit* _Circuit;
	vector<Node *> * _Inputs;
	vector<Node *> * _Outputs;
	Clock * _Clock;
public:
	//constructors
	Graph();
	~Graph(void);
	
	//fields
	static Graph* gGraph;

	virtual Clock * GetClock()
	{
		return _Clock;
	}

	virtual void SetClock(Clock * clk)
	{
		_Clock = clk;
	}

	virtual vector<Edge *> * getEdges(Node * node)
	{
		return _Circuit->find(node)->second;
	}

	virtual instances * getInsts()
	{
		return _Instances;
	}

	virtual void setInsts(instances * insts)
	{
		_Instances = insts;
	}

	virtual circuit * getCircuit()
	{
		return _Circuit;
	}

	virtual void SetCircuit(circuit * c)
	{
		_Circuit = c;
	}

	virtual void ResetNodes()
	{
		for (circuit::iterator i =_Circuit->begin(); i !=  _Circuit->end(); i++)
		{
			i->first->ResetParams();
			for (int j = 0; j < i->second->size(); j++)
			{
				(*i->second)[j]->Delay = 0.0;
			}
		}
	}

	virtual Node* getInput(int index)
	{
		return (*_Inputs)[index];
	}

	virtual void setInputs(vector <Node *> * v)
	{
		_Inputs = v;
	}

	virtual Node* getOutput(int index)
	{
		return (*_Outputs)[index];
	}

	virtual void setOutputs(vector <Node *> * v)
	{
		_Outputs = v;
	}

	virtual int OutsCount()
	{
		return _Outputs->size();
	}

	virtual int InsCount()
	{
		return _Inputs->size();
	}

};

