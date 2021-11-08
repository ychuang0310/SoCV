#include <iostream>
#include <vector>
#include "seater.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2){
        cout << "Usage:\n\t" << argv[0] << " <inputfile>" << endl;
        return 0;
    }

    Seater s(argv[1]);
    s.InitCircuit();
    s.genProofModel();
    s.solve();
    s.reportResult();
    return 0;
}
