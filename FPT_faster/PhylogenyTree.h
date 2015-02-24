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
    friend class LCA;

private:
    TreeNode * parent;
    vector<TreeNode *> children;
    int id;
    int label;
    int reflectId;
    
public:
    TreeNode();
    TreeNode(int id);
    TreeNode(int id,int label);
    TreeNode(int id,int label,int reflectId);
    TreeNode(TreeNode *p);
    ~TreeNode();
    
    void AddChild(TreeNode *pChild);
    void SetId(int x) {id = x;}
    void SetLabel(const int str) {label = str;}
    void SetReflectId(const int id) { reflectId = id;}
    bool IsLeaf() const { return children.size() == 0;}
    bool IsRtLeaf() const { return reflectId != -1;}
    bool Is2ndRtLeaf() const {
        if (children.size()!=2) return false;
        return children[0]->IsRtLeaf() && children[1]->IsRtLeaf();
    }
    bool IsRoot() const { return parent == NULL;}
    bool IsSibling(const TreeNode * p) const ;
    void removeChild(TreeNode * p);
    int GetChildrenSize() { return (int)children.size();}
    TreeNode * GetChild(int i) { return children[i];}
    TreeNode * GetParent() { return parent;}
    TreeNode * GetSiblingNode();
    TreeNode * GetRoot(){
        TreeNode * p = this;
        while (p->parent) p = p->parent;
        return p;
    }
    int GetId() { return id;}
    int GetLablel() { return label;}
    int GetReflectId() { return reflectId;}
    string ToString();
    
    TreeNode * Clone(); // 克隆以当前的为根的子树
};


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
    vector<string> SubTreeDraw(TreeNode *p,int &lx,int &lr);
    void BuildMaps();
    
public:
    PhylogenyTree();
    PhylogenyTree(PhylogenyTree * tree);
    ~PhylogenyTree();
    
    void BuildByNewick(const string & newickStr);
    TreeNode * GetRootNode(int k) { return roots[k];}
    TreeNode * GetNodeById(int id) { return idMap[id]; }
    TreeNode * GetNodeByLabel(int label) {return labelMap[label];}
    int GetLabeledNodeNum() { return (int) labelMap.size();}
    int GetNodeNum() { return (int) idMap.size();}
    int GetRootNum() { return (int) roots.size();}
    void AddRoot(TreeNode * p);
    void Contract();
    void DeleteEdge(int nid); // delete the edge connecting node p and the parent of p , then return a forest
    void DeleteEdges(vector<int> &nids); // delete serveral edges , then return a forest
    void EraseNode(TreeNode * p){
        idMap.erase(p->id);
        if (p->label != -1) labelMap.erase(p->label);
    }
    void RemoveRoot(TreeNode * p){
        for (int i=0;i<roots.size();i++)
            if (roots[i] == p){
                roots.erase(roots.begin()+i);
                break;
        }
        EraseNode(p);
    }
    vector<TreeNode *> GetAllNode();
    vector<TreeNode *> GetAllLabeledNode();
    vector<TreeNode *> GetReflectedNode();
    string ToString();
    string Draw(int i);
    
    void randomSPR(int k);
};


#endif




































