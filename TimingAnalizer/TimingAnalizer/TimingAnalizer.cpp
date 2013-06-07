// TimingAnalizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "parser_helper.h"
#include "Node.h"
#include "Parser.h"
#include "Graph.h"
#include "Analyzer.h"
#include "Library.h"

/*
[1] path to folder with files 
(for example "C:\simple contains simple.v, simple.spef... and contest.lib 
or contest.lib can be placed in the project directory") 
*/

int main(int args, char** argv)
{
	string mainP = "C:\\Games\\TimingAnalyzer\\TimingAnalizer\\simple\\";
	string veriP = mainP + "simple.v";
	string spefP = mainP + "simple.spef";
	string sdcP = mainP + "simple.sdc";
	string libP = mainP + "contest.lib";

	Parser * p = new Parser();
	SdcParser sp(sdcP);
	VerilogParser vp(veriP);
	Clock * clk = p->GetClock(sp);
	Graph * gr = new Graph();
	Library * lib = new Library(p->GetLib(libP));
	circuit * cir = new circuit();
	instances * insts = new instances();
	
	//Get Inputs, Outputs, Clock from Parser
	vector<Node *> * ins = p->GetSdcIns(sp, cir);
	vector<Node *> * outs = p->GetSdcOuts(sp, cir);
	
	//Set Inputs, Outputs, Clock to Graph
	gr->setInputs(ins);
	gr->setOutputs(outs);
	gr->SetClock(clk);
	
	//Get Simple(without inner edges of LibElements) Circuit from Parser
	p->GoToRightPlace(vp);
	p->GetCircuit(vp, spefP, cir, lib, clk, ins, outs);
	
	//Set Simple Circuit to Graph
	gr->SetCircuit(cir);
	
	//Set Directions
	Analyzer * a = new Analyzer(gr);
	a->SetEdgesDirection();
	
	//Get and Set Instances
	p->GetInsts(cir, insts);
	gr->setInsts(insts);
	
	//Get and Set Complete Circuit
	p->GetInners(cir, insts);
	gr->SetCircuit(cir);

	//Some Tests
	//a->TestCheckInputEdges();
	//a->TestFromInToOut();

	a->CalcAAT();

	return 0;
}

