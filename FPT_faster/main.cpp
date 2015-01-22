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
using namespace std;


int main(int argc, const char * argv[]) {
    
    string newick1 = "((a,c),(b,d))";
    string newick2 = "((a,b),(c,d))";
    //    string newick1 = "(a,(b,(d,(e,(f,(g,h))))))";
    //    string newick2 = "(b,(a,(g,(h,(d,(e,f))))))";
    
    cout << "please input two phylogeny trees in Newick format: " <<endl;
    cout << "example : " <<endl;
    cout << "\t" << newick1 <<endl;
    cout << "\t" << newick2 <<endl;
    
    cin >> newick1 >> newick2;
    if (newick1[newick1.length()-1]==';')
        newick1 = newick1.substr(0,newick1.length()-1);
    if (newick2[newick2.length()-1]==';')
        newick2 = newick2.substr(0,newick2.length()-1);
    //    freopen("output.txt", "w", stdout);
    cout << newick1 <<endl;
    cout << newick2 <<endl;
    //    return 0;


    PhylogenyTree * t1 = new PhylogenyTree();
    t1->BuildByNewick(newick1);
    
    PhylogenyTree * t2 = new PhylogenyTree();
    t2->BuildByNewick(newick2);
    
    
    return 0;
}
