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

using namespace std;

class FPTSolver{
    
private:
    Forest ans;
    int cnt1,cnt2,cnt3;
    Forest CloneForest(const Forest &forest);
    void DeleteForest(Forest &forest);
    TreeNode * LCA(TreeNode * p1,TreeNode * p2);
    void FindPendantNodes(TreeNode * p, TreeNode * r,vector<TreeNode *> &nodes);
    bool MAF(Forest F,PhylogenyTree * T1,Forest F2,int k, vector<pair<int, int> > Rt);
    bool DeleteAndSearch(Forest &F,PhylogenyTree * T1,Forest &F2,int k, vector<pair<int, int> > &Rt,TreeNode *root,TreeNode * p);
    bool DeleteAndSearch(Forest &F,PhylogenyTree * T1,Forest &F2,int k, vector<pair<int, int> > &Rt,TreeNode *root,vector<TreeNode *> p);
    
public:

    bool MAF_K(PhylogenyTree * t1, PhylogenyTree * t2,int k);
    int MAF_Calc(PhylogenyTree * t1, PhylogenyTree * t2);
    Forest GetResult() { return ans;};

};

#endif
