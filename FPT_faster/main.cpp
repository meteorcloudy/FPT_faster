//
//  main.cpp
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#include <iostream>
#include "PhylogenyTree.h"
#include "FPTSolver.h"
#include "LCA.h"
using namespace std;


int main(int argc, const char * argv[]) {
    
    string newick1 = "((a,c),(b,d))";
    string newick2 = "((a,b),(c,d))";
   
    cout << "please input two phylogeny trees in Newick format: " <<endl;
    cout << "example : " <<endl;
    cout << "\t" << newick1 <<endl;
    cout << "\t" << newick2 <<endl;
    
//    freopen("/Users/meteorcloudy/Desktop/data/mytest.in", "r", stdin);
//    cin >> newick1 >> newick2;
    getline(cin,newick1);
    getline(cin,newick2);

//    newick1 = "((b,a),((c,5),4));";
//    newick2 = "(3,((2,(5,4)),1));";
    newick1 = newick1.substr(0,newick1.rfind(")")+1);
    newick2 = newick2.substr(0,newick2.rfind(")")+1);

//    cout << newick1 <<endl;
//    cout << newick2 <<endl;



    PhylogenyTree * t1 = new PhylogenyTree();
    t1->BuildByNewick(newick1);
    
    PhylogenyTree * t2 = new PhylogenyTree();
    t2->BuildByNewick(newick2);

//    cout << t1->Draw(0) <<endl;
//    cout << t2->Draw(0) <<endl;
    
    FPTSolver solver;
    int dist = solver.MAF_Calc(t1, t2);
    cout << "rSPR distance : " << dist <<endl;
    
    PhylogenyTree * forest = solver.GetResult();
    cout << "MAF:" <<endl;
    cout << forest->ToString() <<endl;
    
    delete t1;
    delete t2;
    
    return 0;
}
