#pragma once

#include <string>
#include "LibElement.h"

using namespace std;

class Node
{
private:
	string _Name;
	LibElement * _Type;
	double _AAT;
	double _RAT;
	double _Slack;
	double _TT;
	LibParserPinInfo _Pin;
	bool _RATCalc;
public:
	Node(string name, LibElement * type);
	~Node(void);

	virtual string getName() {
		return _Name;
	}

	virtual void setName(string name) {
		_Name = name;
	}

	virtual bool IsRATCalculated()
	{
		return _RATCalc;
	}

	virtual void SetRATCalc(bool rat)
	{
		_RATCalc = rat;
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

	virtual double getAAT() {
		return _AAT;
	}

	virtual void setAAT(double aat)
	{
		_AAT=aat;
	}

	virtual double getRAT() {
		return _RAT;
	}

	virtual void setRAT(double rat)
	{
		_RAT=rat;
		UpdateSlack();
	}

	virtual double getSlack() {
		return _Slack;
	}

	virtual void UpdateSlack()
	{
		_Slack =_RAT - _AAT;
	}

	virtual LibParserPinInfo GetPin()
	{
		return _Pin;
	}

	virtual void SetPin(LibParserPinInfo p)
	{
		_Pin = p;
	}

	virtual double GetTT()
	{
		return _TT;
	}

	virtual void SetTT(double tt)
	{
		_TT = tt;
	}

	virtual void ResetParams()
	{
		_AAT = 0.0;
		_RAT = 0.0;
		//_TT = 0.0;
		_Slack = 0.0;
		_RATCalc = false;
	}
};

struct Compare
{
		bool operator()(Node * const &a, Node * const &b) const
		{
			return (a->getName()+a->GetPin().name).compare(b->getName() + b->GetPin().name) > 0;
		}
};