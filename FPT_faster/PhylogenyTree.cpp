//
//  PhylogenyTree.cpp
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#include "PhylogenyTree.h"

#define PRINT_DETAIL 0

using namespace std;


unordered_map<string, int> labelToNum; // 用于简化label为数字，输入输出时进行转换
unordered_map<int, string> numToLabel;
int totNum = 0; // 总的label节点个数

// implement TreeNode

TreeNode :: TreeNode() : id(-1), parent(NULL), label(-1), reflectId(-1) {
}

TreeNode :: TreeNode(int x) : id(x), parent(NULL), label(-1), reflectId(-1) {
}

TreeNode :: TreeNode(int x,int l) : id(x), parent(NULL), label(l), reflectId(-1) {
}

TreeNode :: TreeNode(int x,int l,int ref) : id(x), parent(NULL), label(l), reflectId(ref) {
}

TreeNode :: TreeNode(TreeNode * p){
    TreeNode(p->id,p->label,p->reflectId);
    for (int i=0;i<p->children.size();i++)
        this->AddChild(new TreeNode(p->children[i]));
}

TreeNode :: ~TreeNode(){
    for(int i=0; i<children.size(); ++i){
        delete children[i];
    }
    children.clear();
}

TreeNode * TreeNode:: GetSiblingNode(){
    TreeNode * p = this->parent;
    if (p==NULL) return NULL;
    if (p->children[0]==this) return p->children[1];
    return p->children[0];
}

void TreeNode :: AddChild(TreeNode *pChild){
    FPT_ASSERT_INFO( pChild!=NULL, "can't add a null child");
    children.push_back(pChild);
    pChild->parent = this;
}

bool TreeNode :: IsSibling(const TreeNode *p) const {  // if and only if their parents are the same, their subtrees don't have to be the same
    if (p==NULL || p->parent==NULL) return false;
    return this->parent == p->parent;
}

void TreeNode :: removeChild(TreeNode *p) {
    int i = 0;
    while (i < children.size()){
        if (children[i] == p) {
            children.erase(children.begin()+i);
            return;
        }
        i++;
    }
}

string TreeNode :: ToString() {
#if PRINT_DETAIL==1
    if (label == -1) return "{ id = " + itoa(id) + " pid: " + itoa(parent?parent->id:-1) + " }";
    return "{ id = " + itoa(id) + ", label = " + numToLabel[label] + " pid: " + itoa(parent?parent->id:-1) + " }";
#else
    return numToLabel[label];
#endif
    return "";
}

// implement PhylogenyTree

bool NodeComparer (TreeNode * a,TreeNode * b) {
    return a->GetLablel() < b->GetLablel();
}

PhylogenyTree :: PhylogenyTree() {
}

PhylogenyTree :: PhylogenyTree(PhylogenyTree * tree) {
    for (int i=0;i<tree->roots.size();i++)
        roots.push_back(new TreeNode(tree->roots[i]));
    BuildMaps();
}

PhylogenyTree :: ~PhylogenyTree() {
    for (int i=(int)roots.size()-1;i>=0;i--){
        delete roots[i];
    }
    roots.clear();
}

void PhylogenyTree :: BuildByNewick(const string &newickStr) {
    for (int i=(int)roots.size()-1;i>=0;i--){
        delete roots[i];
    }
    roots.clear();
    nodeNum = 0;
    roots.push_back(BuildSubtreeByNewick(newickStr));
}

TreeNode * PhylogenyTree :: BuildSubtreeByNewick(const string &newickStr) {
    int pos = 0;
    int cnt = 0;
    int len = (int) newickStr.length();
    
    while (pos < len) {
        if (cnt == 1 && newickStr[pos]==',') break;
        if (newickStr[pos] == '(') cnt++;
        if (newickStr[pos] == ')') cnt--;
        pos++;
    }
    
    if (pos == len) {
        FPT_ASSERT_INFO(cnt==0, "Wrong Newick format !" );
        FPT_ASSERT_INFO(CheckLabelFormat(newickStr), ("Wrong label format: "+newickStr).c_str());
        unordered_map<string,int> :: iterator it = labelToNum.find(newickStr);

        if (it==labelToNum.end()) {
            numToLabel[totNum] = newickStr;
            labelToNum[newickStr] = totNum;
            TreeNode * t = new TreeNode(nodeNum,totNum);
            labelMap[totNum++] = t;
            idMap[nodeNum++] = t;
            return t;
        } else {
            TreeNode * t = new TreeNode(nodeNum,it->second);
            labelMap[it->second] = t;
            idMap[nodeNum++] = t;
            return t;
        }
    } else {
        FPT_ASSERT_INFO(newickStr[0] == '(', "Wrong Newick format !" );
        FPT_ASSERT_INFO(newickStr[len-1] == ')', "Wrong Newick format !" );
        TreeNode * p = new TreeNode(nodeNum);
        idMap[nodeNum++] = p;
        TreeNode * child1 = BuildSubtreeByNewick(newickStr.substr(1,pos-1));
        TreeNode * child2 = BuildSubtreeByNewick(newickStr.substr(pos+1,len-pos-2));
        child1->parent = child2->parent = p;
        p->children.push_back(child1);
        p->children.push_back(child2);
        return p;
    }
}

string PhylogenyTree :: ToString() {
    string result = "";
    if (roots.size()==1)
        return SubTreeToString(roots[0]);
    else if (roots.size()>1) {
        result = "forest{\n";
        for (int i=0;i<roots.size();i++)
            result = "\t" + SubTreeToString(roots[i]) + "\n";
        result = "}";
    }
    return result;
}

string PhylogenyTree :: SubTreeToString(TreeNode * p) {
#if PRINT_DETAIL==1
    if (p->IsLeaf()) return p->ToString();
    return p->ToString() + " => ( " + SubTreeToString(p->children[0]) + " , " + SubTreeToString(p->children[1]) + " )";
#else
    if (p->IsLeaf()) return p->ToString();
    return "( " + SubTreeToString(p->children[0]) + " , " + SubTreeToString(p->children[1]) + " )";
#endif
    return "";
}

void PhylogenyTree :: AddRoot(TreeNode * p){
//    roots.push_back(new TreeNode(p));
    roots.push_back(p);
}

void PhylogenyTree :: Contract() {
    for (int i=(int)roots.size()-1;i>=0;i--)
        roots[i] = SubTreeContract(roots[i]);
    
    int cnt = 0;
    int i = 0;
    while (i<roots.size()){
        if (roots[i] != NULL)
            roots[cnt++] = roots[i];
        i++;
    }
    
    roots.resize(cnt);
}

TreeNode * PhylogenyTree :: SubTreeContract(TreeNode *p) {
    
    do {
        
        while (p->GetChildrenSize()==1) {
            TreeNode * tmp = p->children[0];
            p->children.clear();
            tmp->parent = p->parent;
            idMap.erase(p->id);
            delete p;
            p = tmp;
        }
        
        if (p->GetChildrenSize()==2) {
            TreeNode * child1 = SubTreeContract(p->children[0]);
            TreeNode * child2 = SubTreeContract(p->children[1]);
            p->children.clear();
            if (child1 != NULL) p->children.push_back(child1);
            if (child2 != NULL) p->children.push_back(child2);
        }
        
    } while (p->GetChildrenSize()!=0&&p->GetChildrenSize()!=2);
    
    if (p->IsLeaf() && p->GetLablel() == -1) {
        delete p;
        idMap.erase(p->id);
        p = NULL;
    }
    
    return p;
}

void PhylogenyTree :: DeleteEdge(int nid) {
    
    TreeNode * p = idMap[nid];
    
    FPT_ASSERT_INFO(p != NULL,("There is no node whose id = "+itoa(nid)).c_str());
    FPT_ASSERT_INFO(p->parent != NULL,"There is no edge above p");
    
    TreeNode * parent = p->parent;
    parent->removeChild(p);
    p->parent = NULL;
    roots.push_back(p);
    
    
    TreeNode * child = parent->children[0];
    
    idMap.erase(parent->id);
    parent->id = child->id;
    parent->label = child->label;
    parent->reflectId = child->reflectId;
    parent->children = child->children;
    idMap[parent->id] = parent;
    if (parent->label!=-1) labelMap[parent->label] = parent;
    child->children.clear();
    delete child;
    for (int i=0;i<parent->children.size();i++){
        parent->children[i]->parent = parent;
    }
    
    //    this->Contract();
}

void PhylogenyTree :: DeleteEdges(vector<int> &nids) {  //  TODO 用DeleteEdge优化
    
    for (int i=(int)nids.size()-1;i>=0;i--) {
        TreeNode * p = idMap[nids[i]];
        
        FPT_ASSERT_INFO(p != NULL,("_ There is no node whose id = "+itoa(nids[i])).c_str());
        FPT_ASSERT_INFO(p->parent != NULL,"_ There is no edge above p");
        
        p->parent->removeChild(p);
        p->parent = NULL;
        roots.push_back(p);
    }
    
    this->Contract();
}

void PhylogenyTree :: BuildMaps(){  // TODO , 优化，只bfs到siblingId!=-1的点
    idMap.clear();
    labelMap.clear();
    queue<TreeNode *> q;
    
    for (int i=(int)roots.size()-1;i>=0;i--)
        q.push(roots[i]);
    
    TreeNode * p;
    while (!q.empty()){
        p = q.front(); q.pop();
        idMap[p->id] = p;
        if (p->label != -1) labelMap[p->label]=p;
        if (p->reflectId != -1) continue;   // 只bfs到siblingId!=-1的点
        for (int i=(int) p->children.size()-1;i>=0;i--)    	
            q.push(p->children[i]);
    }
}


vector<TreeNode *> PhylogenyTree :: GetAllLabeledNode(){
    vector<TreeNode *> res;
    unordered_map<int,TreeNode *> :: iterator i;
    for (i=labelMap.begin();i!=labelMap.end();i++)
        res.push_back(i->second);
    sort(res.begin(),res.end(),NodeComparer);
    return res;
}

vector<TreeNode *> PhylogenyTree :: GetReflectedNode(){
    vector<TreeNode *> res;
    unordered_map<int,TreeNode *> :: iterator i;
    for (i=idMap.begin();i!=idMap.end();i++)
        if (i->second->reflectId != -1)
            res.push_back(i->second);
    return res;
}






















