#ifndef Seater_h
#define Seater_h

#include "sat.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

class Gate
{
public:
	Gate(unsigned menid, unsigned seatid): _menid(menid), _seatid(seatid) {}
	~Gate() {}

	Var getVar() const { return _var; }
	void setVar(const Var& v) { _var = v; }

private:
	unsigned   _menid, _seatid;
	Var        _var;
};

class Seater
{
public:
	Seater(){}
	Seater(const string& filename) {parse(filename);}
	~Seater(){
		for(size_t i=0; i<_gates.size(); ++i){
			for(size_t j=0; j<_gates[i].size(); ++j) delete _gates[i][j];
		}
	}

	void parse(const string& filename);
	void InitCircuit();
	void genProofModel();

	void solve();
	void reportResult();
	
	void Debug();
private:
	unsigned _nSeats;
	// _gates[i][j] -> mi in sj
	vector<vector<Gate *>> _gates;
	SatSolver _satsolver;

	bool _SAT;

	enum Constraint{
		Assign, AssignNot, LessThan, Adjacent, AdjacentNot, nConstraints
	};
	vector<pair<unsigned, unsigned>> _constraints[nConstraints];

	// for genProofModel
	void genProofModel_oneSeat();
	void genProofModel_Assign();
	void genProofModel_AssignNot();
	void genProofModel_LessThan();
	void genProofModel_Adjacent();
	void genProofModel_AdjacentNot();

	void line2constraint(const string& line, string& constraint, unsigned& i, unsigned& j);
};

#endif