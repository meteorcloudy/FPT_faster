//
//  FPTSolver.cpp
//  FPT
//
//  Created by Yun Peng on 12/28/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#include "FPTSolver.h"

using namespace std;

Forest FPTSolver :: CloneForest(const Forest &forest) {  // 在内存中克隆一个森林
    Forest res;
    for (int i=0;i<forest.size();i++)
        res.push_back(forest[i]->Clone());
    return res;
}

void FPTSolver :: DeleteForest(Forest &forest) {    // 从内存中删除一个森林
    for (int i=0;i<forest.size();i++)
        delete forest[i];
    forest.clear();
}


TreeNode * FPTSolver :: LCA(TreeNode * p1,TreeNode * p2){ // 找到两个节点的最近公共祖先

    unordered_set<TreeNode *> ancestor;

    while (p1) {
        ancestor.insert(p1);
        p1 = p1->GetParent();
    }
    
    while (p2) {
        if (ancestor.find(p2)!=ancestor.end()) return p2;
        p2 = p2->GetParent();
    }
    
    return NULL;
}

void FPTSolver :: FindPendantNodes(TreeNode *p, TreeNode *r, vector<TreeNode *> &nodes){ // 找到一个节点与他的一个祖先节点之间的所以悬挂节点
    TreeNode * fp;
    
    while (p!=r){
        fp = p->GetParent();
        if (fp != r) {
            if (fp->GetChild(0) == p)
                nodes.push_back(fp->GetChild(1));
            else nodes.push_back(fp->GetChild(0));
        }
        p = fp;
    }
}

bool FPTSolver :: MAF(Forest F,PhylogenyTree * T1, Forest F2, int k, vector<pair<int, int> > Rt) {
    
    // step 1
    if (k<0) return false;
    
    TreeNode * qa = NULL , * qc = NULL; // qa , qc 保存(a,c)在F2中的指针
    
    while (true) {
        
        // step 2
        if (Rt.size()<=2) {
            for (int i=0;i<F2.size();i++)
                F.push_back(F2[i]);
            ans.clear();
            for (int i=0;i<F.size();i++)
                ans.push_back(F[i]->Clone());
            return true ;
        }
        
        //step 3
        unordered_map<int,bool> roots; // 保存F2中的所有根节点id，并标记他们是否出现在Rt中
        
        for (int i=(int) F2.size()-1;i>=0;i--)
            roots[F2[i]->GetRootNode()->GetId()] = false;
        
        int cnt = 0;
        for (int i=0;i<Rt.size();i++){
            if (roots.find(Rt[i].second)==roots.end()) { // 不是F2中的根节点则依然放在Rt中
                Rt[cnt++] = Rt[i];
            }
            else {
                roots[Rt[i].second] = true;             // 是F2中得根节点，则标记，并删除T1中的相应子树，并不再放入Rt中
                if (T1->GetNodeById(Rt[i].first)->IsRoot() == false) {
                    Forest forest = T1->DeleteEdge(T1->GetNodeById(Rt[i].first));
                    if (forest[0]->GetRootNode()->GetId() == Rt[i].first){
                        T1 = forest[1];
                        delete forest[0];
                    } else {
                        T1 = forest[0];
                        delete forest[1];
                    }
                }
            }
        }
        Rt.resize(cnt);  // 重置Rt大小
        
        bool isFChanged = false;
        
        cnt = 0;
        for (int i=0;i<F2.size();i++){  // 将F2中对应的子树移动到F中
            if (roots[F2[i]->GetRootNode()->GetId()]) {
                isFChanged = true;
                F.push_back(F2[i]);
            } else {
                F2[cnt++] = F2[i];
            }
        }
        F2.resize(cnt); // 重置F2大小
        
        if (isFChanged) continue; // 若F和Rt有改变则回到第二步
        
        //step 4
        
        unordered_map<int,int> RtMap; // 讲Rt中T1的所有节点id映射到它在Rt中的位置
        for (int i=0;i<Rt.size();i++)
            RtMap[Rt[i].first] = i;
        
        TreeNode * p = NULL;        // 在T1中BFS找到一队出现在Rt中的兄弟节点
        int pa,pc,p1,p2;
        p = qa = qc = NULL;
        pa = pc = -1;
        queue<TreeNode *> qNode;
        qNode.push(T1->GetRootNode());
        while (!qNode.empty()){
            p = qNode.front(); qNode.pop();
            if (p->IsLeaf()) continue;
            if (RtMap.find(p->GetChild(0)->GetId()) != RtMap.end() &&
                RtMap.find(p->GetChild(1)->GetId()) != RtMap.end() ) {
                pa = p->GetChild(0)->GetId();
                pc = p->GetChild(1)->GetId();
                break;
            } else {
                qNode.push(p->GetChild(0));   // 可改进，若出现在Rt中，则不必再入队
                qNode.push(p->GetChild(1));
            }
        }
        
        FPT_ASSERT_INFO(pa!=-1, "Can't find sibling node (a,c)");
        FPT_ASSERT_INFO(pc!=-1, "Can't find sibling node (a,c)");
        
        //step 5
        
        for (int i=0;i<F2.size();i++) {  // 找到(a,c)在F2中对应的点
            if (qa==NULL) qa = F2[i]->GetNodeById(Rt[RtMap[pa]].second);
            if (qc==NULL) qc = F2[i]->GetNodeById(Rt[RtMap[pc]].second);
            if (qa && qc) break;
        }

        FPT_ASSERT_INFO(qa!=NULL, ("Can't find corresponding node in F2 for a, label="+p->GetChild(0)->GetLablel()).c_str());
        FPT_ASSERT_INFO(qc!=NULL, ("Can't find corresponding node in F2 for c, label="+p->GetChild(1)->GetLablel()).c_str());
        
        if (!qa->IsSibling(qc)) break;  // 若（a,c）在F2中不是兄弟节点，则转到第6步
        
        p1 = p->GetId();
        p2 = qa->GetParent()->GetId();  // 若（a,c）在F2中是兄弟节点，则改变Rt，转到第2步
        
        int pos1,pos2;
        pos1 = RtMap[pa]; pos2= RtMap[pc];
        if (pos1<pos2) swap(pos1, pos2);
        
        Rt.erase(Rt.begin()+pos1);
        Rt.erase(Rt.begin()+pos2);
        Rt.push_back(make_pair(p1, p2));
    }
    
    //step 6.1
    
    TreeNode * ra, * rc; // 保存qa,qb所在树的根节点
    
    ra = qa;
    rc = qc;
    
    while (ra->GetParent()) ra = ra->GetParent();
    while (rc->GetParent()) rc = rc->GetParent();
    
    
    if (ra != rc) {  // 若(a,c)不在同一个树中，即不连通
        cnt1++;
        //删除节点a上面的边，递归执行MAF，注意这里需要创建F,F2,T1的副本，以保留现场，否则递归会出错
        if (DeleteAndSearch(F, T1, F2, k, Rt, ra, qa)) return true;
        
        //删除节点c上面的边，递归执行MAF
        if (DeleteAndSearch(F, T1, F2, k, Rt, rc, qc)) return true;
        
        return false;
    }
    
    //step 6.2 & 6.3    （a,c）在同一棵树里
    TreeNode * rac = LCA(qa,qc);  // rac保存qa,qc的最近公共祖先
    FPT_ASSERT_INFO(rac != NULL, "Can't find the common ancestor of a,c");
    
    vector<TreeNode *> b;  // 保存a,c路径上的所有悬挂节点
    
    FindPendantNodes(qa, rac, b);
    FindPendantNodes(qc, rac, b);
    
    FPT_ASSERT_INFO(b.size()>0, "No pendant nodes");
    
    if (b.size()==1) {
        cnt2++;
        if (DeleteAndSearch(F, T1, F2, k, Rt, ra, b[0])) return true;
    } else {
        cnt3++;
        //删除节点a上面的边，递归执行MAF
        if (DeleteAndSearch(F, T1, F2, k, Rt, ra, qa)) return true;
        
        //删除节点c上面的边，递归执行MAF
        if (DeleteAndSearch(F, T1, F2, k, Rt, rc, qc)) return true;
        
        // 删除所有悬挂节点上面的边
        if (b.size()>k) return false;  // 如果要删除的边大于k条，则直接返回false
        if (DeleteAndSearch(F, T1, F2, k, Rt, ra, b)) return true;
    }
    
    return false;
}

bool FPTSolver :: DeleteAndSearch(Forest &F, PhylogenyTree *T1, Forest &F2, int k, vector<pair<int, int> > &Rt, TreeNode* root,TreeNode *p) {
    Forest _F2,_F;
    PhylogenyTree * _T1;
    bool isOK;
    
    for (int i=0;i<F2.size();i++)
        if (F2[i]->GetRootNode() == root){
            PhylogenyTree * tree = F2[i]->Clone();
            Forest forest = tree->DeleteEdge(tree->GetNodeById(p->GetId()));
            for (int j=0;j<forest.size();j++)
                _F2.push_back(forest[j]);
        } else {
            _F2.push_back(F2[i]->Clone());
        }
    
    _F = CloneForest(F);
    _T1 = T1->Clone();
    isOK =  MAF(_F, _T1, _F2, k-1, Rt);
    DeleteForest(_F);
    DeleteForest(_F2);
    delete _T1;
    
    return isOK;
}


bool FPTSolver :: DeleteAndSearch(Forest &F, PhylogenyTree *T1, Forest &F2, int k, vector<pair<int, int> > &Rt, TreeNode* root,vector<TreeNode *> b) {
    Forest _F2,_F;
    PhylogenyTree * _T1;
    bool isOK;
    
    for (int i=0;i<F2.size();i++)
        if (F2[i]->GetRootNode() == root){
            PhylogenyTree * tree = F2[i]->Clone();
            
            for (int i=0;i<b.size();i++) b[i] = tree->GetNodeById(b[i]->GetId());
            
            Forest forest = tree->DeleteEdges(b);
            for (int j=0;j<forest.size();j++)
                _F2.push_back(forest[j]);
            
        } else {
            _F2.push_back(F2[i]->Clone());
        }
    
    _F = CloneForest(F);
    _T1 = T1->Clone();
    isOK =  MAF(_F, _T1, _F2, k - (int) b.size(), Rt);
    DeleteForest(_F);
    DeleteForest(_F2);
    delete _T1;
    
    return isOK;
}

bool FPTSolver :: MAF_K(PhylogenyTree *t1, PhylogenyTree *t2, int k){  // 判断t1和t2的rSPR距离是否小于等于k
    
    Forest F,F2;
    PhylogenyTree * T1;
    
    T1 = t1->Clone();
    F2.push_back(t2->Clone());
    
    vector<pair<int, int> > Rt;
    vector<TreeNode *> leaf1 = t1->GetAllLabeledNode();
    vector<TreeNode *> leaf2 = t2->GetAllLabeledNode();
    
    for (int i=0;i<leaf1.size();i++)
        Rt.push_back(make_pair(leaf1[i]->GetId(),leaf2[i]->GetId()));
    
    return MAF(F, T1, F2, k, Rt);
}

int FPTSolver :: MAF_Calc(PhylogenyTree *t1, PhylogenyTree *t2) { // 用二分查找或者递增法，找到最小的k
    int n = t1->GetNodeNum();
    FPT_ASSERT_INFO(n == t2->GetNodeNum(), "Two trees don't have the same number of leaves");
    vector<TreeNode *> leaf1 = t1->GetAllLabeledNode();
    for (int i=0;i<leaf1.size();i++)
        FPT_ASSERT_INFO(t2->GetNodeByLabel(leaf1[i]->GetLablel()), "Labels don't agree");

    cout << "There are " << n <<" leaves in the original tree" <<endl;
    
    
    cnt1=cnt2=cnt3=0;
    

    int i = 0;
    while (MAF_K(t1,t2,i)==false)
        i++;
//    cout << "cnt1=" << cnt1 << endl;
//    cout << "cnt2=" << cnt2 << endl;
//    cout << "cnt3=" << cnt3 << endl;
    return i;
}


























