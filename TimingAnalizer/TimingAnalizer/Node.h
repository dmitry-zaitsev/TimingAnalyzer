#pragma once
#include <string>

using namespace std;

class Node
{
public:
	Node(string name, string type);
	~Node(void);

	virtual string getName() {
		return _Name;
	}

	virtual void setName(string name) {
		_Name = name;
	}
	
	virtual string getType() {
		return _Type;
	}

	virtual void setType(string type) {
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
	string _Type;
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