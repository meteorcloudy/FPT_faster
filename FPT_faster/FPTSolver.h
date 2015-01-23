//
//  FPTSolver.h
//  FPT
//
//  Created by Yun Peng on 12/28/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#ifndef __FPT__FPTSolver__
#define __FPT__FPTSolver__

#include "PhylogenyTree.h"
#include "LCA.h"
using namespace std;

class FPTSolver{
    
private:
    LCA lca;
    PhylogenyTree * ans;
    void FindPendantNodes(TreeNode * p, TreeNode * r,vector<int> &nids);
    bool MAF(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k);
    
public:
    bool MAF_K(PhylogenyTree * t1, PhylogenyTree * t2,int k);
    int MAF_Calc(PhylogenyTree * t1, PhylogenyTree * t2);
    PhylogenyTree * GetResult() { return ans;};

};

#endif
