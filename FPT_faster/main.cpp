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


void draw(){
    string newick1 = "((a,c),(b,d))";
    
    getline(cin,newick1);
    
    newick1 = newick1.substr(0,newick1.rfind(")")+1);
    
    PhylogenyTree * t1 = new PhylogenyTree();
    t1->BuildByNewick(newick1);
    
    cout << t1->Draw(0) <<endl;
}

void makedata(int n,int k){
    srand((unsigned int)time(0));
    string newick = "1";
    
    for (int i=2;i<=n;i++)
        newick = "("+newick+","+itoa(i)+")";
    
    PhylogenyTree * t = new PhylogenyTree();
    t->BuildByNewick(newick);
    
    t->randomSPR(n);
    
    cout << t->ToString() <<";"<<endl;
    
    t->randomSPR(k);
    cout << t->ToString() <<";"<<endl;
}


void run(){
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
    
    newick1 = newick1.substr(0,newick1.rfind(")")+1);
    newick2 = newick2.substr(0,newick2.rfind(")")+1);
    
    
    PhylogenyTree * t1 = new PhylogenyTree();
    t1->BuildByNewick(newick1);
    
    PhylogenyTree * t2 = new PhylogenyTree();
    t2->BuildByNewick(newick2);
    
    
    FPTSolver solver;
    int dist = solver.MAF_Calc(t1, t2);
    cout << "rSPR distance : " << dist <<endl;
    
    PhylogenyTree * forest = solver.GetResult();
    cout << "MAF:" <<endl;
    cout << forest->ToString() <<endl;
    
    delete t1;
    delete t2;

}

int main(int argc, const char * argv[]) {
    
//    draw();
//    return 0;
//    int n,k;
//    cin >> n >> k;
//    makedata(n,k);
    run();
    
    return 0;
}
