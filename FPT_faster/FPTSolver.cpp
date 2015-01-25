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

bool FPTSolver:: MergePair(PhylogenyTree *T1, PhylogenyTree *F2,TreeNode * n1, TreeNode * n2){
    TreeNode * p1 = F2->GetNodeById(n1->GetReflectId());
    TreeNode * p2 = F2->GetNodeById(n2->GetReflectId());
    if (p1->IsSibling(p2)) {
        TreeNode * f1 = n1->GetParent();
        TreeNode * f2 = p1->GetParent();
        f1->SetReflectId(f2->GetId());
        f2->SetReflectId(f1->GetId());
        n1->SetReflectId(-1);
        n2->SetReflectId(-1);
        p1->SetReflectId(-1);
        p2->SetReflectId(-1);
        T1->EraseNode(n1); T1->EraseNode(n2);
        F2->EraseNode(p1); F2->EraseNode(p2);
        return true;
    }
    return false;
}

bool FPTSolver:: MergeSiblingNodes(PhylogenyTree *T1, PhylogenyTree *F2){
    vector<TreeNode *> rt = T1->GetReflectedNode();
    int i = 0;
    bool isMerged = false;
    while (i<rt.size()){
        if (rt[i]->GetReflectId() == -1) { i++; continue;}
        TreeNode * sib = rt[i]->GetSiblingNode();
        if (sib != NULL){
            if (MergePair(T1,F2,rt[i],sib)) {
                rt.push_back(rt[i]->GetParent());
                isMerged = true;
            }
            
        }
        i++;
    }
    return isMerged;
}

Group FPTSolver:: FindGroup(PhylogenyTree * tree){
    FPT_ASSERT_INFO(tree->GetRootNum()==1, "T1's rootNum != 1");
    queue<TreeNode *> q;
    q.push(tree->GetRootNode(0));
    TreeNode * p;
    Group ans;
    while (!q.empty()){
        p = q.front(); q.pop();
        if (p->IsRtLeaf() || p->IsLeaf()) continue;
        if (p->Is2ndRtLeaf()) continue;
        
        if (p->GetChild(0)->Is2ndRtLeaf() && p->GetChild(1)->IsRtLeaf()) {
            ans.type = 3;
            ans.a = p->GetChild(0)->GetChild(0);
            ans.b = p->GetChild(0)->GetChild(1);
            ans.c = p->GetChild(1);
            return ans;
        }
        if (p->GetChild(1)->Is2ndRtLeaf() && p->GetChild(0)->IsRtLeaf()) {
            ans.type = 3;
            ans.a = p->GetChild(1)->GetChild(0);
            ans.b = p->GetChild(1)->GetChild(1);
            ans.c = p->GetChild(0);
            return ans;
        }
        if (p->GetChild(0)->Is2ndRtLeaf() && p->GetChild(1)->Is2ndRtLeaf()){
            ans.type = 4;
            ans.a = p->GetChild(0)->GetChild(0);
            ans.b = p->GetChild(0)->GetChild(1);
            ans.c = p->GetChild(1)->GetChild(0);
            ans.d = p->GetChild(1)->GetChild(1);
            return ans;
        }
        for (int i=0;i<p->GetChildrenSize();i++)
            if (!p->GetChild(i)->IsRtLeaf()) {
                q.push(p->GetChild(i));
            }
    }
    FPT_ASSERT_INFO(ans.type!=-1, "Can't find group");
    return ans;
}

bool FPTSolver:: MoveTree(PhylogenyTree *F, PhylogenyTree * T1, PhylogenyTree *F2){
    bool isMoved = false;
    int pos = 0;
    while (pos<F2->GetRootNum()){
        TreeNode * root = F2->GetRootNode(pos);
        if (root->GetReflectId() != -1){
            TreeNode * p = T1->GetNodeById(root->GetReflectId());
            T1->DeleteEdge(p->GetId());
            T1->RemoveRoot(p);
            delete p;
            F->AddRoot(root);
            F2->RemoveRoot(root);
            isMoved = true;
            continue;
        }
        pos++;
    }
    return isMoved;
}

void FPTSolver:: clearParam(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2){
    delete F;
    delete T1;
    delete F2;
}

void FPTSolver:: buildParam(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2,PhylogenyTree * &_F,PhylogenyTree * &_T1,PhylogenyTree * &_F2){
    _F = new PhylogenyTree(F);
    _T1 = new PhylogenyTree(T1);
    _F2 = new PhylogenyTree(F2);
}

TreeNode * FPTSolver:: IsOneDistance(TreeNode *p1, TreeNode *p2){
    if (p1->GetParent()->GetParent() != NULL && ( p1->GetParent()->GetParent() == p2->GetParent() ) ) return p1->GetSiblingNode();
    if (p2->GetParent()->GetParent() != NULL && ( p1->GetParent() == p2->GetParent()->GetParent() ) ) return p2->GetSiblingNode();
//    if ( p1->GetParent()->GetParent() == p2->GetParent() ) return true;
//    if ( p1->GetParent() == p2->GetParent()->GetParent() ) return true;
    return NULL;
}

bool FPTSolver:: Case_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2){
    TreeNode * roota = grp2.a->GetRoot();
    TreeNode * rootb = grp2.b->GetRoot();
    bool res = false;
    
    if (roota != rootb) {
        res = Case_1_1(F,T1,F2,k,grp2);
    } else {
        
        TreeNode * Rab = F2->GetNodeById(lca.ask(grp2.a->GetId(), grp2.b->GetId()));
        vector<int> pedant_ab;
        FindPendantNodes(grp2.a, Rab, pedant_ab);
        FindPendantNodes(grp2.b, Rab, pedant_ab);
        FPT_ASSERT_INFO(pedant_ab.size()!=0, "pedant size = 0!");
        
        if (pedant_ab.size()==1){
            res = Case_1_2_1(F, T1, F2, k, pedant_ab[0]);
        } else {
            
            TreeNode * rootc = grp2.c->GetRoot();
            if (roota != rootc) {
                res = Case_1_2_2_1(F, T1, F2, k, grp2, pedant_ab);
            } else {
                res = Case_1_2_2_2(F, T1, F2, k, grp2, pedant_ab);
            }
        }
    }
    return res;
}

bool FPTSolver:: Case_1_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.a->GetId());
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.b->GetId());
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    return false;
}

bool FPTSolver:: Case_1_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, int nid){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(nid);
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    return false;
}

bool FPTSolver:: Case_1_2_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, vector<int> & pedant_ab){
    PhylogenyTree * _F , * _T1, * _F2;
    
    if (k >= pedant_ab.size()) {
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ab);
        if (MAF(_F, _T1, _F2, k-(int)pedant_ab.size())) return true;
    }
    
    if (k >= 2){
        vector<int> nids;
        nids.push_back(grp2.a->GetId());
        nids.push_back(grp2.b->GetId());
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(nids);
        if (MAF(_F, _T1, _F2, k-2)) return true;
    }
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.c->GetId());
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    return false;
}

bool FPTSolver:: Case_1_2_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, vector<int> & pedant_ab){
    
    if (grp2.a->IsSibling(grp2.b)) { return Case_1_2_2_2_1(F, T1, F2, k, grp2.c->GetId()); }
    if (grp2.a->IsSibling(grp2.c)) { return Case_1_2_2_2_1(F, T1, F2, k, grp2.b->GetId()); }
    if (grp2.b->IsSibling(grp2.c)) { return Case_1_2_2_2_1(F, T1, F2, k, grp2.a->GetId()); }
    
    if (IsOneDistance(grp2.a, grp2.c) && IsOneDistance(grp2.b, grp2.c)) { return Case_1_2_2_2_2(F, T1, F2, k, grp2, pedant_ab);}
    
    TreeNode * Rac = F2->GetNodeById(lca.ask(grp2.a->GetId(), grp2.c->GetId()));
    TreeNode * Rbc = F2->GetNodeById(lca.ask(grp2.b->GetId(), grp2.c->GetId()));
    
    vector<int> * pedant_ac = new vector<int>();
    vector<int> * pedant_bc = new vector<int>();
    FindPendantNodes(grp2.a, Rac, *pedant_ac);
    FindPendantNodes(grp2.c, Rac, *pedant_ac);
    FindPendantNodes(grp2.b, Rbc, *pedant_bc);
    FindPendantNodes(grp2.c, Rbc, *pedant_bc);
    
    if (pedant_ac->size() < pedant_bc->size()) {
        swap(grp2.a, grp2.b);
        swap(pedant_ac, pedant_bc);
    }
    
    bool res = Case_1_2_2_2_3(F, T1, F2, k, grp2, pedant_ab , * pedant_ac);
    
    delete pedant_ac;
    delete pedant_bc;
    
    return res;
}

bool FPTSolver:: Case_1_2_2_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, int nid){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(nid);
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    return false;
}

bool FPTSolver:: Case_1_2_2_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, vector<int> & pedant_ab){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.a->GetId());
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    if (k>=pedant_ab.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ab);
        if (MAF(_F, _T1, _F2, k-(int)pedant_ab.size())) return true;
    }
    
    if (k>=2){
        vector<int> nids;
        nids.push_back(grp2.b->GetId());
        nids.push_back(grp2.c->GetId());
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(nids);
        if (MAF(_F, _T1, _F2, k-2)) return true;
    }
    
    return false;
}

bool FPTSolver:: Case_1_2_2_2_3(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, vector<int> & pedant_ab, vector<int> & pedant_ac){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.a->GetId());
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    if (k>=pedant_ab.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ab);
        if (MAF(_F, _T1, _F2, k-(int)pedant_ab.size())) return true;
    }
    
    if (k>=2){
        vector<int> nids;
        nids.push_back(grp2.b->GetId());
        nids.push_back(grp2.c->GetId());
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(nids);
        if (MAF(_F, _T1, _F2, k-2)) return true;
    }

    bool flag = true;
    for (int i=0;i<pedant_ac.size()&&flag;i++)
        if (pedant_ac[i] == grp2.b->GetId()){
            flag = false;
        }
    if (flag) pedant_ac.push_back(grp2.b->GetId());
    
    if (k>=pedant_ac.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ac);
        if (MAF(_F, _T1, _F2, k-(int)pedant_ac.size())) return true;
    }
    
    return false;
}

bool FPTSolver:: Case_2_1(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, int nid1, int nid2){
    PhylogenyTree * _F , * _T1, * _F2;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(nid1);
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(nid2);
    if (MAF(_F, _T1, _F2, k-1)) return true;
    
    return false;
}

bool FPTSolver:: Case_2_2(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, vector<int> & pedant_one){
    PhylogenyTree * _F , * _T1, * _F2;
    
    if (k>=pedant_one.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_one);
        if (MAF(_F, _T1, _F2, k-(int)pedant_one.size())) return true;
    }
    
    return false;
}

bool FPTSolver:: Case_2_3(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2, TreeNode * Rab){
    PhylogenyTree * _F , * _T1, * _F2;
    
    vector<int> pedant_ab;
    FindPendantNodes(grp2.a, Rab, pedant_ab);
    FindPendantNodes(grp2.b, Rab, pedant_ab);
    
    if (k >= pedant_ab.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ab);
        if (Rab->IsRoot()){
            if (MAF(_F, _T1, _F2, k-(int)pedant_ab.size())) return true;
        } else {
            
            Prepare(_F, _T1, _F2);
            if (_F2->GetRootNum() == 0) {
                ans = new PhylogenyTree(_F); clearParam(_F, _T1, _F2); return true;
            }
            if (k-(int)pedant_ab.size()==0) return false;
            
            Group grp;
            grp.type = 3;
            grp.a = _F2->GetNodeById(grp2.c->GetId());
            grp.b = _F2->GetNodeById(grp2.d->GetId());
            grp.c = _F2->GetNodeById(Rab->GetId());
            
            vector<int> pedant_cd;
            TreeNode * Rcd = _F2->GetNodeById(lca.ask(grp2.c->GetId(), grp2.d->GetId()));
            FindPendantNodes(grp.a, Rcd, pedant_cd);
            FindPendantNodes(grp.b, Rcd, pedant_cd);
            
            if (Case_1_2_2_1(_F, _T1, _F2, k-(int)pedant_ab.size(), grp, pedant_cd)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
        }
    }

    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.a->GetId());
    MoveTree(_F, _T1, _F2);
    Group grp;
    grp.type = 3;
    grp.a = _F2->GetNodeById(grp2.c->GetId());
    grp.b = _F2->GetNodeById(grp2.d->GetId());
    grp.c = _F2->GetNodeById(grp2.b->GetId());
    vector<int> pedant_cd;
    TreeNode * Rcd = _F2->GetNodeById(lca.ask(grp2.c->GetId(), grp2.d->GetId()));
    FindPendantNodes(grp.a, Rcd, pedant_cd);
    FindPendantNodes(grp.b, Rcd, pedant_cd);
    
    if (Case_1_2_2_1(_F, _T1, _F2, k-1, grp, pedant_cd)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.b->GetId());
    MoveTree(_F, _T1, _F2);
    grp.a = _F2->GetNodeById(grp2.c->GetId());
    grp.b = _F2->GetNodeById(grp2.d->GetId());
    grp.c = _F2->GetNodeById(grp2.a->GetId());
    
    if (Case_1_2_2_1(_F, _T1, _F2, k-1, grp, pedant_cd)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    
    return false;
}

bool FPTSolver:: Case_2_4(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k, Group &grp2){
    
    PhylogenyTree * _F , * _T1, * _F2;
    
    vector<int> pedant_ab;
    TreeNode * Rab = F2->GetNodeById(lca.ask(grp2.a->GetId(), grp2.b->GetId()));
    FindPendantNodes(grp2.a, Rab, pedant_ab);
    FindPendantNodes(grp2.b, Rab, pedant_ab);
    
    if (k >= pedant_ab.size()){
        buildParam(F, T1, F2, _F, _T1, _F2);
        _F2->DeleteEdges(pedant_ab);
        
        Prepare(_F, _T1, _F2);
        if (_F2->GetRootNum() == 0) {
            ans = new PhylogenyTree(_F); clearParam(_F, _T1, _F2); return true;
        }
        if (k-(int)pedant_ab.size()>0) {
            Group grp;
            grp.type = 3;
            grp.a = _F2->GetNodeById(grp2.c->GetId());
            grp.b = _F2->GetNodeById(grp2.d->GetId());
            grp.c = _F2->GetNodeById(Rab->GetId());
            
            if (grp.a==NULL || grp.b==NULL || grp.c==NULL) {
                if (MAF(_F, _T1, _F2, k-(int)pedant_ab.size())) return true;
            } else {
                if (Case_1(_F, _T1, _F2, k-(int)pedant_ab.size(), grp)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
            }
        }
    }
    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.a->GetId());
    MoveTree(_F, _T1, _F2);
    Group grp;
    grp.type = 3;
    grp.a = _F2->GetNodeById(grp2.c->GetId());
    grp.b = _F2->GetNodeById(grp2.d->GetId());
    grp.c = _F2->GetNodeById(grp2.b->GetId());
    vector<int> pedant_cd;
    TreeNode * Rcd = _F2->GetNodeById(lca.ask(grp2.c->GetId(), grp2.d->GetId()));
    FindPendantNodes(grp.a, Rcd, pedant_cd);
    FindPendantNodes(grp.b, Rcd, pedant_cd);
    
    if (pedant_cd.size() == 1){
        if (Case_1_2_1(_F, _T1, _F2, k-1, pedant_cd[0])) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    } else {
        if (Case_1_2_2_2(_F, _T1, _F2, k-1, grp, pedant_cd)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    }

    
    buildParam(F, T1, F2, _F, _T1, _F2);
    _F2->DeleteEdge(grp2.b->GetId());
    MoveTree(_F, _T1, _F2);    
    grp.a = _F2->GetNodeById(grp2.c->GetId());
    grp.b = _F2->GetNodeById(grp2.d->GetId());
    grp.c = _F2->GetNodeById(grp2.a->GetId());
    
    Rcd = _F2->GetNodeById(lca.ask(grp2.c->GetId(), grp2.d->GetId()));
    pedant_cd.clear();
    FindPendantNodes(grp.a, Rcd, pedant_cd);
    FindPendantNodes(grp.b, Rcd, pedant_cd);
    
    if (pedant_cd.size() == 1){
        if (Case_1_2_1(_F, _T1, _F2, k-1, pedant_cd[0])) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    } else {
        if (Case_1_2_2_2(_F, _T1, _F2, k-1, grp, pedant_cd)) { clearParam(_F, _T1, _F2); return true;} else clearParam(_F, _T1, _F2);
    }
    
    return false;
}

void FPTSolver:: Prepare(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2){
    while (MergeSiblingNodes(T1, F2) | MoveTree(F, T1, F2));
}

bool FPTSolver:: MAF(PhylogenyTree * F,PhylogenyTree * T1,PhylogenyTree * F2, int k){
    
    if (k<0) return false;
    
    Prepare(F, T1, F2);
    if (F2->GetRootNum() == 0) {
        ans = new PhylogenyTree(F); clearParam(F, T1, F2); return true;
    }
    
    if (k==0) return false;
    
    //核心算法
    Group grp1 = FindGroup(T1);
    Group grp2;
    grp2.type = grp1.type;
    grp2.a = F2->GetNodeById(grp1.a->GetReflectId());
    grp2.b = F2->GetNodeById(grp1.b->GetReflectId());
    grp2.c = F2->GetNodeById(grp1.c->GetReflectId());
    if (grp1.type == 4) grp2.d = F2->GetNodeById(grp1.d->GetReflectId());
    
    bool res = false;
    
    if (grp1.type == 3){
        res = Case_1(F, T1, F2, k, grp2);
    } else {
        
        TreeNode * roota = grp2.a->GetRoot();
        TreeNode * rootb = grp2.b->GetRoot();
        
        if (roota != rootb) {
            res = Case_2_1(F,T1,F2,k,grp2.a->GetId(),grp2.b->GetId());
            goto finished;
        }
        
        TreeNode * rootc = grp2.c->GetRoot();
        TreeNode * rootd = grp2.d->GetRoot();
        
        if (rootc != rootd) {
            res = Case_2_1(F,T1,F2,k,grp2.c->GetId(),grp2.d->GetId());
            goto finished;
        }
        
        TreeNode * tmp;
        vector<int> pedant_one;
        if ((tmp=IsOneDistance(grp2.a, grp2.b))!= NULL) { pedant_one.push_back(tmp->GetId()); }
        if ((tmp=IsOneDistance(grp2.c, grp2.d))!= NULL) { pedant_one.push_back(tmp->GetId()); }
        
        if (pedant_one.size() != 0) {
            res = Case_2_2(F,T1,F2,k,pedant_one);
            goto finished;
        }
        
        if (roota != rootc){
            TreeNode * Rab = F2->GetNodeById(lca.ask(grp2.a->GetId(), grp2.b->GetId()));
            res = Case_2_3(F,T1,F2,k,grp2,Rab);
            goto finished;
        }
        
        res = Case_2_4(F,T1,F2,k,grp2);
    }
    
finished:
    clearParam(F,T1,F2);
    return res;
}

bool FPTSolver :: MAF_K(PhylogenyTree *t1, PhylogenyTree *t2, int k){  // 判断t1和t2的rSPR距离是否小于等于k

    PhylogenyTree * F,* T1,* F2;
    
    F = new PhylogenyTree();
    T1 = new PhylogenyTree(t1);
    F2 = new PhylogenyTree(t2);

//    cout << t1->ToString() <<endl;
//    cout << t2->ToString() <<endl;
//
//    cout << T1->ToString() <<endl;
//    cout << F2->ToString() <<endl;
    
    return MAF(F, T1, F2, k);
}

int FPTSolver:: MAF_Calc(PhylogenyTree * t1, PhylogenyTree * t2){
    int n = t1->GetLabeledNodeNum();
    FPT_ASSERT_INFO(n == t2->GetLabeledNodeNum(), "Two trees don't have the same number of leaves");
    vector<TreeNode *> leaf1 = t1->GetAllLabeledNode();
    for (int i=0;i<leaf1.size();i++)
        FPT_ASSERT_INFO(t2->GetNodeByLabel(leaf1[i]->GetLablel()), "Labels don't agree");
    
    cout << "There are " << n <<" leaves in the original tree" <<endl;
    
    vector<TreeNode *> leaf2 = t2->GetAllLabeledNode();
    
    for (int i=0;i<leaf1.size();i++) {
        leaf1[i]->SetReflectId(leaf2[i]->GetId());
        leaf2[i]->SetReflectId(leaf1[i]->GetId());
    }

//    cout << t1->ToString() <<endl;
//    cout << t2->ToString() <<endl;
    
    lca.Build(t2);
    
    int i = 0;
    while (MAF_K(t1,t2,i)==false){
        printf("%d failed!\n",i);
        i++;
    }
    
    return i;

}


















