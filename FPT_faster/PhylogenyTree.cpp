//
//  PhylogenyTree.cpp
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#include "PhylogenyTree.h"

#define FOREST_SORT 0
#define PRINT_DETAIL 0

using namespace std;

// implement TreeNode

TreeNode :: TreeNode() : id(-1), parent(NULL), label("-") {
}

TreeNode :: TreeNode(int x) : id(x), parent(NULL), label("-") {
}

TreeNode :: TreeNode(int x,string l) : id(x), parent(NULL), label(l) {
}

TreeNode :: ~TreeNode(){
    for(int i=0; i<children.size(); ++i){
        delete children[i];
    }
    children.clear();
}

void TreeNode :: AddChild(TreeNode *pChild){
    FPT_ASSERT_INFO( pChild!=NULL, "can't add a null child");
    children.push_back(pChild);
    pChild->parent = this;
}

bool TreeNode :: IsSibling(const TreeNode *p) const {  // if and only if their parents are the same, their subtrees don't have to be the same
    if (p==NULL) return false;
    if (p->parent==NULL) return false;
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
    if (label == "-") return "{ id = " + itoa(id) + " pid: " + itoa(parent?parent->id:-1) + " }";
    return "{ id = " + itoa(id) + ", label = " + label + " pid: " + itoa(parent?parent->id:-1) + " }";
#else
    return label;
#endif
    return "";
}

TreeNode * TreeNode :: Clone(){
    TreeNode * node = new TreeNode(this->id,this->label);
    TreeNode * child;
    for (int i=0;i<children.size();i++) {
        child = children[i]->Clone();
        child->parent = node;
        node->children.push_back(child);
    }
    return node;
}

// implement PhylogenyTree

#if FOREST_SORT==1
bool TreeComparer (PhylogenyTree * a,PhylogenyTree * b) {
    return a->GetRootNode()->GetId() < b->GetRootNode()->GetId();
}
#endif

bool NodeComparer (TreeNode * a,TreeNode * b) {
    return a->GetLablel() < b->GetLablel();
}

PhylogenyTree :: PhylogenyTree() : rootNode(NULL) {
}

PhylogenyTree :: PhylogenyTree(TreeNode * p) : rootNode(p) {
    BuildMaps();
}

PhylogenyTree :: ~PhylogenyTree() {
    if (rootNode != NULL) {
        delete rootNode;
        rootNode = NULL;
    }
}

void PhylogenyTree :: BuildByNewick(const string &newickStr) {
    if (rootNode != NULL) {
        delete rootNode;
        rootNode = NULL;
    }
    nodeNum = 0;
    rootNode = BuildSubtreeByNewick(newickStr);
    BuildMaps();
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
        return new TreeNode(nodeNum++,newickStr);
    } else {
        FPT_ASSERT_INFO(newickStr[0] == '(', "Wrong Newick format !" );
        FPT_ASSERT_INFO(newickStr[len-1] == ')', "Wrong Newick format !" );
        TreeNode * p = new TreeNode(nodeNum++);
        TreeNode * child1 = BuildSubtreeByNewick(newickStr.substr(1,pos-1));
        TreeNode * child2 = BuildSubtreeByNewick(newickStr.substr(pos+1,len-pos-2));
        child1->parent = child2->parent = p;
        p->children.push_back(child1);
        p->children.push_back(child2);
        return p;
    }
}

string PhylogenyTree :: ToString() {
    return SubTreeToString(rootNode);
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

void PhylogenyTree :: Contract() {
    rootNode = SubTreeContract(rootNode);
    BuildMaps();
}

TreeNode * PhylogenyTree :: SubTreeContract(TreeNode *p) {
    
    do {
        
        while (p->GetChildrenSize()==1) {
            TreeNode * tmp = p->children[0];
            p->children.clear();
            tmp->parent = p->parent;
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
    
    if (p->IsLeaf() && p->GetLablel() == "-") {
        delete p;
        p = NULL;
    }
    
    return p;
}

Forest PhylogenyTree :: DeleteEdge(TreeNode *p) {
    
    FPT_ASSERT_INFO(p->parent != NULL,"There is no edge above p");
    
    Forest forest;
    p->parent->removeChild(p);
    p->parent = NULL;
    forest.push_back(new PhylogenyTree(p));
    
    this->Contract();
    forest.push_back(this);
    
#if FOREST_SORT==1
    sort(forest.begin(),forest.end(),TreeComparer);
#endif
    return forest;
}

Forest PhylogenyTree :: DeleteEdges(vector<TreeNode *> nodes) {
    
    Forest forest;
    
    for (int i=(int)nodes.size()-1;i>=0;i--) {
        FPT_ASSERT_INFO(nodes[i]->parent != NULL,("There is no edge above p" + itoa(i)).c_str());
        nodes[i]->parent->removeChild(nodes[i]);
        nodes[i]->parent = NULL;
        forest.push_back(new PhylogenyTree(nodes[i]));
    }
    
    forest.push_back(this);
    
    for (int i=(int)forest.size()-1;i>=0;i--)
        forest[i]->Contract();
    
    int cnt = 0;
    int i = 0;
    while (i<forest.size()){
        if (forest[i]->rootNode != NULL)
            forest[cnt++] = forest[i];
        i++;
    }
    
    forest.resize(cnt);
#if FOREST_SORT==1
    sort(forest.begin(),forest.end(),TreeComparer);
#endif
    return forest;
}

void PhylogenyTree :: BuildMaps(){
    idMap.clear();
    labelMap.clear();
    if (rootNode==NULL) return;
    queue<TreeNode *> q;
    q.push(rootNode);
    TreeNode * p;
    while (!q.empty()){
        p = q.front(); q.pop();
        idMap[p->id] = p;
        if (p->label != "-") labelMap[p->label]=p;
        for (int i=(int) p->children.size()-1;i>=0;i--)
            q.push(p->children[i]);
    }
}

PhylogenyTree * PhylogenyTree :: Clone() {
    return new PhylogenyTree(rootNode->Clone());
}

vector<TreeNode *> PhylogenyTree :: GetAllLabeledNode(){
    vector<TreeNode *> res;
    unordered_map<string,TreeNode *> :: iterator i;
    for (i=labelMap.begin();i!=labelMap.end();i++)
        res.push_back(i->second);
    sort(res.begin(),res.end(),NodeComparer);
    return res;
}






















