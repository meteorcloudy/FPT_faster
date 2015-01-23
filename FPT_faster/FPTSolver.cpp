//
//  FPTSolver.cpp
//  FPT_faster
//
//  Created by Yun Peng on 1/23/15.
//  Copyright (c) 2015 Yun Peng. All rights reserved.
//

#include "FPTSolver.h"
#include "LCA.h"
using namespace std;


void FPTSolver:: FindPendantNodes(TreeNode * p, TreeNode * r,vector<int> &nids){
    TreeNode * fp;
    
    while (p!=r){
        fp = p->GetParent();
        if (fp != r) {
            if (fp->GetChild(0) == p)
                nids.push_back(fp->GetChild(1)->GetId());
            else nids.push_back(fp->GetChild(0)->GetId());
        }
        p = fp;
    }
}

bool FPTSolver:: MAF(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k){
    
    if (k<0) return false;
    
    
    
    return false;
}

bool FPTSolver :: MAF_K(PhylogenyTree *t1, PhylogenyTree *t2, int k){  // 判断t1和t2的rSPR距离是否小于等于k

    PhylogenyTree * F,* T1,* F2;
    
    F = new PhylogenyTree();
    T1 = new PhylogenyTree(t1);
    F2 = new PhylogenyTree(t2);
    
    return MAF(F, T1, F2, k);
}

int FPTSolver:: MAF_Calc(PhylogenyTree * t1, PhylogenyTree * t2){
    int n = t1->GetLabeledNodeNum();
    FPT_ASSERT_INFO(n == t2->GetLabeledNodeNum(), "Two trees don't have the same number of leaves");
    vector<TreeNode *> leaf1 = t1->GetAllLabeledNode();
    for (int i=0;i<leaf1.size();i++)
        FPT_ASSERT_INFO(t2->GetNodeByLabel(leaf1[i]->GetLablel()), "Labels don't agree");
    
    cout << "There are " << n <<" leaves in the original tree" <<endl;
    
    int i = 0;
    
    vector<TreeNode *> leaf2 = t2->GetAllLabeledNode();
    
    for (int i=0;i<leaf1.size();i++) {
        leaf1[i]->SetReflectId(leaf2[i]->GetId());
        leaf2[i]->SetReflectId(leaf1[i]->GetId());
    }
    
    lca.Build(t2);
    
    while (MAF_K(t1,t2,i)==false)
        i++;
    return i;

}


















