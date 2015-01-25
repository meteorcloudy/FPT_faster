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

struct Group{
    TreeNode * a, * b, * c, *d;
    int type;
    Group(){ a=b=c=d=NULL; type=-1; }
};

class FPTSolver{
    
private:
    LCA lca;
    PhylogenyTree * ans;
    void FindPendantNodes(TreeNode * p, TreeNode * r,vector<int> &nids);
    bool MergePair(PhylogenyTree *T1, PhylogenyTree *F2,TreeNode * n1, TreeNode * n2);
    bool MergeSiblingNodes(PhylogenyTree * T1, PhylogenyTree * F2);
    bool MoveTree(PhylogenyTree * F, PhylogenyTree * T1, PhylogenyTree * F2);
    Group FindGroup(PhylogenyTree * tree);
    void clearParam(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2);
    void buildParam(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2,PhylogenyTree * & _F,PhylogenyTree * & _T1,PhylogenyTree * & _F2);
    TreeNode * IsOneDistance(TreeNode * p1, TreeNode * p2);
    bool Case_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2);
    bool Case_1_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,Group & grp2);
    bool Case_1_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,int nid);
    bool Case_1_2_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,Group & grp2,vector<int> & pedant);
    bool Case_1_2_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, vector<int> & pedant);
    bool Case_1_2_2_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,int nid);
    bool Case_1_2_2_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,Group & grp2,vector<int> & pedant);
    bool Case_1_2_2_2_3(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k,Group & grp2,vector<int> & pedant_ab,vector<int> & pedant_ac);
    bool Case_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, int nid1, int nid2);
    bool Case_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, vector<int> & pedant_one);
    bool Case_2_3(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, TreeNode * Rab);
    bool Case_2_4(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2);
    void Prepare(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2);
    bool MAF(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k);
    
public:
    bool MAF_K(PhylogenyTree * t1, PhylogenyTree * t2,int k);
    int MAF_Calc(PhylogenyTree * t1, PhylogenyTree * t2);
    PhylogenyTree * GetResult() { return ans;};

};

#endif
