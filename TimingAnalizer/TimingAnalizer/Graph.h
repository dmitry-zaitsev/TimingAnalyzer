#pragma once
#include "Node.h"
#include <map>
#include <vector>

using namespace std;

struct Edge
{
	Node *StartNode;
	Node *EndNode;
	float Delay;
	string Name;
};

typedef map<Node *, vector<Edge *> *, Compare> circuit;

class Graph
{
private:
	circuit* _Circuit;
	vector<Node *> * _Inputs;
	vector<Node *> * _Outputs;
	Node * _StartNode;
	Node * _EndNode;
public:
	//constructors
	Graph();
	~Graph(void);
	
	//fields
	static Graph* gGraph;

	//methods
	virtual Node * GetStartNode()
	{
		return _StartNode;
	}

	virtual void SetStartNode(Node * n)
	{
		_StartNode = n;
	}

	virtual Node * GetEndNode()
	{
		return _EndNode;
	}

	virtual void SetEndNode(Node * n)
	{
		_EndNode = n;
	}
	
	virtual vector<Edge *> * getEdges(Node * node)
	{
		return _Circuit->find(node)->second;
	}

	virtual circuit * getCircuit()
	{
		return _Circuit;
	}

	virtual void SetCircuit(circuit * c)
	{
		_Circuit = c;
	}

	virtual void CalcSlack()
	{
		circuit::iterator iter;
		for (iter = _Circuit->begin(); iter != _Circuit->end(); iter++)
		{
			iter->first->setSlack();
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

