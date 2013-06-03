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

	vector<Node *> * ins = p->GetSdcIns(sp, cir);
	gr->setInputs(ins);
	vector<Node *> * outs = p->GetSdcOuts(sp, cir);
	gr->setOutputs(outs);
	gr->SetClock(clk);

	p->GoToRightPlace(vp);
	p->GetCircuit(vp, spefP, cir, lib, clk, ins, outs);
	gr->SetCircuit(cir);

	Analyzer * a = new Analyzer(gr);
	a->SetEdgesDirection();
	a->TestCheckInputEdges();

	return 0;
}

