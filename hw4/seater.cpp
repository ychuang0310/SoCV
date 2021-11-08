#include "seater.h"

void Seater::parse(const string& filename)
{
	ifstream ifs(filename);
	string line;
	getline(ifs, line);
	_nSeats = stoul(line);
	size_t pos;
	string constraint;
	unsigned i ,j;
	while(getline(ifs, line)){
		line2constraint(line, constraint, i, j);
		if(constraint == "Assign"){
			_constraints[Assign].emplace_back(i ,j);
		}
		else if(constraint == "AssignNot"){
			_constraints[AssignNot].emplace_back(i ,j);
		}
		else if(constraint == "LessThan"){
			_constraints[LessThan].emplace_back(i ,j);
		}
		else if(constraint == "Adjacent"){
			_constraints[Adjacent].emplace_back(i ,j);
		}
		else if(constraint == "AdjacentNot"){
			_constraints[AdjacentNot].emplace_back(i ,j);
		}
	}
}

void Seater::InitCircuit()
{
	_gates.clear();
	_gates.resize(_nSeats);
	for(unsigned i=0; i<_nSeats; ++i){
		for(unsigned j=0; j<_nSeats; ++j){
			_gates[i].push_back(new Gate(i, j));
		}
	}
}

void Seater::genProofModel()
{
	_satsolver.initialize();
	_satsolver.assumeRelease();

	for(size_t i=0; i<_gates.size(); ++i) {
		for(size_t j=0; j<_gates[i].size(); ++j){
			_gates[i][j]->setVar(_satsolver.newVar());
		}
	}

	genProofModel_oneSeat();
	genProofModel_Assign();
	genProofModel_AssignNot();
	genProofModel_LessThan();
	genProofModel_Adjacent();
	genProofModel_AdjacentNot();
}

void Seater::genProofModel_oneSeat()
{
	vec<Lit> lits;
	// each men should at least sit in one seat
	for(unsigned i=0; i<_nSeats; ++i){
		lits.clear();
		for(unsigned j=0; j<_nSeats; ++j){
			lits.push( Lit(_gates[i][j]->getVar()) );
		}
		_satsolver.addClause(lits);
	}

	// each men should at most sit in one seat
	lits.clear();
	lits.growTo(2);
	for(unsigned i=0; i<_nSeats; ++i){
		for(unsigned j=0; j<_nSeats-1; ++j){
			lits[0] = ~Lit(_gates[i][j]->getVar());
			for(unsigned k=j+1; k<_nSeats; ++k){
				lits[1] = ~Lit(_gates[i][k]->getVar());
				_satsolver.addClause(lits);
			}
		}
	}

	// each sit should seat at least one men
	for(unsigned j=0; j<_nSeats; ++j){
		lits.clear();
		for(unsigned i=0; i<_nSeats; ++i){
			lits.push( Lit(_gates[i][j]->getVar()) );
		}
		_satsolver.addClause(lits);
	}

	// each sit should seat at most one men
	lits.clear();
	lits.growTo(2);
	for(unsigned j=0; j<_nSeats; ++j){
		for(unsigned i=0; i<_nSeats-1; ++i){
			lits[0] = ~Lit(_gates[i][j]->getVar());
			for(unsigned k=i+1; k<_nSeats; ++k){
				lits[1] = ~Lit(_gates[k][j]->getVar());
				_satsolver.addClause(lits);
			}
		}
	}
}

void Seater::genProofModel_Assign()
{
	for(size_t c=0; c<_constraints[Assign].size(); ++c){
		const unsigned &i = _constraints[Assign][c].first, &j = _constraints[Assign][c].second;
		_satsolver.assertProperty(_gates[i][j]->getVar(), true);
	}
}

void Seater::genProofModel_AssignNot()
{
	for(size_t c=0; c<_constraints[AssignNot].size(); ++c){
		const unsigned &i = _constraints[AssignNot][c].first, &j = _constraints[AssignNot][c].second;
		_satsolver.assertProperty(_gates[i][j]->getVar(), false);
	}
}

void Seater::genProofModel_LessThan()
{
	vec<Lit> lits;
	for(size_t c=0; c<_constraints[LessThan].size(); ++c){
		const unsigned &i = _constraints[LessThan][c].first, &j = _constraints[LessThan][c].second;
		_satsolver.assertProperty(_gates[j][0]->getVar(), false);
		for(unsigned k=1; k<_nSeats; ++k){
			lits.clear();
			lits.push( ~Lit(_gates[j][k]->getVar()) );
			for(unsigned m=0; m<k; ++m){
			    lits.push( Lit(_gates[i][m]->getVar()) );
			}
			_satsolver.addClause(lits);
		}
	}

	for(size_t c=0; c<_constraints[LessThan].size(); ++c){
		const unsigned &i = _constraints[LessThan][c].first, &j = _constraints[LessThan][c].second;
		_satsolver.assertProperty(_gates[i][_nSeats-1]->getVar(), false);
		for(unsigned k=0; k<(_nSeats-1); ++k){
			lits.clear();
			lits.push( ~Lit(_gates[i][k]->getVar()) );
			for(unsigned m=k+1; m<_nSeats; ++m){
			    lits.push( Lit(_gates[j][m]->getVar()) );
			}
			_satsolver.addClause(lits);
		}
	}
}

void Seater::genProofModel_Adjacent()
{
	vec<Lit> lits;
	for(size_t c=0; c<_constraints[Adjacent].size(); ++c){
		const unsigned &i = _constraints[Adjacent][c].first, &j = _constraints[Adjacent][c].second;
		lits.clear();
		lits.push( ~Lit(_gates[i][0]->getVar()) );
		lits.push( Lit(_gates[j][1]->getVar()) );
		_satsolver.addClause(lits);

		lits.clear();
		lits.push( ~Lit(_gates[i][_nSeats-1]->getVar()) );
		lits.push( Lit(_gates[j][_nSeats-2]->getVar()) );
		_satsolver.addClause(lits);

		for(unsigned k=1; k<(_nSeats-1); ++k){
			lits.clear();
			lits.push( ~Lit(_gates[i][k]->getVar()) );
			lits.push( Lit(_gates[j][k-1]->getVar()) );
			lits.push( Lit(_gates[j][k+1]->getVar()) );
			_satsolver.addClause(lits);
		}
	}

	for(size_t c=0; c<_constraints[Adjacent].size(); ++c){
		const unsigned &i = _constraints[Adjacent][c].first, &j = _constraints[Adjacent][c].second;
		lits.clear();
		lits.push( ~Lit(_gates[j][0]->getVar()) );
		lits.push( Lit(_gates[i][1]->getVar()) );
		_satsolver.addClause(lits);

		lits.clear();
		lits.push( ~Lit(_gates[j][_nSeats-1]->getVar()) );
		lits.push( Lit(_gates[i][_nSeats-2]->getVar()) );
		_satsolver.addClause(lits);

		for(unsigned k=1; k<(_nSeats-1); ++k){
			lits.clear();
			lits.push( ~Lit(_gates[j][k]->getVar()) );
			lits.push( Lit(_gates[i][k-1]->getVar()) );
			lits.push( Lit(_gates[i][k+1]->getVar()) );
			_satsolver.addClause(lits);
		}
	}
}

void Seater::genProofModel_AdjacentNot()
{
	vec<Lit> lits;
	for(size_t c=0; c<_constraints[AdjacentNot].size(); ++c){
		const unsigned &i = _constraints[AdjacentNot][c].first, &j = _constraints[AdjacentNot][c].second;
		lits.clear();
		lits.push( ~Lit(_gates[i][0]->getVar()) );
		lits.push( ~Lit(_gates[j][1]->getVar()) );
		_satsolver.addClause(lits);

		lits.clear();
		lits.push( ~Lit(_gates[i][_nSeats-1]->getVar()) );
		lits.push( ~Lit(_gates[j][_nSeats-2]->getVar()) );
		_satsolver.addClause(lits);

		for(unsigned k=1; k<(_nSeats-1); ++k){
			lits.clear();
			lits.push( ~Lit(_gates[i][k]->getVar()) );
			lits.push( ~Lit(_gates[j][k-1]->getVar()) );
			_satsolver.addClause(lits);

			lits[1] = ~Lit(_gates[j][k+1]->getVar());
			_satsolver.addClause(lits);
		}
	}

	for(size_t c=0; c<_constraints[AdjacentNot].size(); ++c){
		const unsigned &i = _constraints[AdjacentNot][c].first, &j = _constraints[AdjacentNot][c].second;
		lits.clear();
		lits.push( ~Lit(_gates[j][0]->getVar()) );
		lits.push( ~Lit(_gates[i][1]->getVar()) );
		_satsolver.addClause(lits);

		lits.clear();
		lits.push( ~Lit(_gates[j][_nSeats-1]->getVar()) );
		lits.push( ~Lit(_gates[i][_nSeats-2]->getVar()) );
		_satsolver.addClause(lits);

		for(unsigned k=1; k<(_nSeats-1); ++k){
			lits.clear();
			lits.push( ~Lit(_gates[j][k]->getVar()) );
			lits.push( ~Lit(_gates[i][k-1]->getVar()) );
			_satsolver.addClause(lits);

			lits[1] = ~Lit(_gates[i][k+1]->getVar());
			_satsolver.addClause(lits);
		}
	}
}

void Seater::solve()
{
	_SAT = _satsolver.assumpSolve();
}

void Seater::reportResult()
{
	if(!_SAT){
		cout << "No satisfiable assignment can be found." << endl;
		return;
	}
	cout << "Satisfiable assignment:" << endl;
	for(unsigned j=0; j<_nSeats; ++j){
		for(unsigned i=0; i<_nSeats; ++i){
			if( _satsolver.getValue(_gates[i][j]->getVar()) ){
				cout << j << '(' << i << ')';
				if(j!=_nSeats-1) cout << ", ";
				break;
			}
		}
	}
	// cout << endl;
}

void Seater::Debug()
{
    _satsolver.printStats();
    if(!_SAT){
	   cout << "UNSAT" << endl;
	   return;
    }
    cout << "SAT" << endl;
    cout << setw(3) << "i\\j";
    for(size_t i=0; i<_nSeats; ++i) cout << setw(3) << i;
    cout << endl;
    for(size_t i=0; i<_gates.size(); ++i){
		cout << setw(3) << i;
	   for(size_t j=0; j<_gates.size(); ++j){
			// cout << "a[" << i << "][" << j << "] " << _satsolver.getValue(_gates[i][j]->getVar()) << " ";
			cout << setw(3) << _satsolver.getValue(_gates[i][j]->getVar());
	   }
	   cout << endl;
    }
	cout << endl;
}

void Seater::line2constraint(const string& line, string& constraint, unsigned& i, unsigned& j)
{
	size_t beg, end;
	end = line.find('(');
	constraint = line.substr(0, end);
	beg = end+1;
	end = line.find(',', beg);
	i = stoul( line.substr(beg, end-beg) );
	beg = end+1;
	end = line.find(')', beg);
	j = stoul( line.substr(beg, end-beg) );
}
