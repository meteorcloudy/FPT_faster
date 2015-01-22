//
//  Tree.h
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#ifndef FPT_Tree_h
#define FPT_Tree_h

#include "Utils.h"

using namespace std;

//TreeNode
class TreeNode{
    
    friend class PhylogenyTree;

private:
    TreeNode * parent;
    vector<TreeNode *> children;
    int id;
    int label;
    
public:
    TreeNode();
    TreeNode(int id);
    TreeNode(int id,int label);
    ~TreeNode();
    
    void AddChild(TreeNode *pChild);
    void SetId(int x) {id = x;}
    void SetLabel(const int str) {label = str;}
    bool IsLeaf() const { return children.size() == 0;}
    bool IsRoot() const { return parent == NULL;}
    bool IsSibling(const TreeNode * p) const ;
    void removeChild(TreeNode * p);
    int GetChildrenSize() { return (int)children.size();}
    TreeNode * GetChild(int i) { return children[i];}
    TreeNode * GetParent() { return parent;}
    int GetId() { return id;}
    int GetLablel() { return label;}
    string ToString();
    
    TreeNode * Clone(); // 克隆以当前的为根的子树
};


extern unordered_map<string, int> labelToNum;
extern unordered_map<int, string> numToLabel;
extern int totNum;

//PhylogenyTree
class PhylogenyTree{
    
private:
    vector<TreeNode *> roots;
    unordered_map<int,TreeNode *> idMap;
    unordered_map<int,TreeNode *> labelMap;
    int nodeNum;  // 仅建树时使用，不代表节点总数
    
    TreeNode * BuildSubtreeByNewick(const string & newickStr);
    string SubTreeToString(TreeNode * p);
    TreeNode * SubTreeContract(TreeNode * p);
    void BuildMaps();
    
public:
    PhylogenyTree();
    PhylogenyTree(vector<TreeNode *> p);
    ~PhylogenyTree();
    
    void BuildByNewick(const string & newickStr);
    TreeNode * GetRootNode(int k) { return roots[k];}
    TreeNode * GetNodeById(int id) { return idMap[id]; }
    TreeNode * GetNodeByLabel(int label) {return labelMap[label];}
    int GetNodeNum() { return (int) labelMap.size();}
    void Contract();
    vector<PhylogenyTree *> DeleteEdge(TreeNode * p); // delete the edge connecting node p and the parent of p , then return a forest
    vector<PhylogenyTree *> DeleteEdges(vector<TreeNode *> nodes); // delete serveral edges , then return a forest
    vector<TreeNode *> GetAllLabeledNode();
    string ToString();
    
    PhylogenyTree * Clone();
};


#endif




































