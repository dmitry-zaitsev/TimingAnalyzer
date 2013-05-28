#pragma once

#include <string>
#include "LibElement.h"

using namespace std;

class Node
{
public:
	Node(string name, LibElement * type);
	~Node(void);

	virtual string getName() {
		return _Name;
	}

	virtual void setName(string name) {
		_Name = name;
	}

	virtual void setImplementation(LibElement* element) {
	}

	virtual LibElement* getImplementation() {
		return 0; //TODO implement
	}
	
	virtual LibElement * getType() {
		return _Type;
	}

	virtual void setType(LibElement * type) {
		_Type = type;
	}

	virtual float getAAT() {
		return _AAT;
	}

	virtual void setAAT(float aat)
	{
		_AAT=aat;
	}

	virtual float getRAT() {
		return _RAT;
	}

	virtual void setRAT(float rat)
	{
		_RAT=rat;
	}

	virtual float getSlack() {
		return _Slack;
	}

	virtual void setSlack()
	{
		_Slack =_RAT - _AAT;
	}
private:
	string _Name;
	LibElement * _Type;
	float _AAT;
	float _RAT;
	float _Slack;
};

struct Compare
	{
		bool operator()(Node * const &a, Node * const &b) const
		{
				return a->getName().compare(b->getName()) > 0;
		}
	};