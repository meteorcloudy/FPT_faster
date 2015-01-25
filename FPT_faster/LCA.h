//
//  LCA.h
//  FPT_faster
//
//  Created by Yun Peng on 1/23/15.
//  Copyright (c) 2015 Yun Peng. All rights reserved.
//

#ifndef FPT_faster_LCA_h
#define FPT_faster_LCA_h

#include "PhylogenyTree.h"
const int Log=20;
using namespace std;

class LCA{
    
private:
    vector<vector<int> > dp;
    vector<int> depth;
    
    void dfs(TreeNode * p)
    {
        int u = p->GetId(), v;
        
        for(int i=1;i<Log;i++)
            dp[u][i]=dp[dp[u][i-1]][i-1];
        
        for (int i=0;i<p->children.size();i++){
            v = p->children[i]->GetId();
            dp[v][0] = u;
            depth[v]=depth[u]+1;
            dfs(p->children[i]);
        }
    }
    
public:
    void Build(PhylogenyTree * tree){
        dp.clear(); depth.clear();
        dp.resize(tree->GetNodeNum(),vector<int>(20));
        depth.resize(tree->GetNodeNum(),0);
        dfs(tree->GetRootNode(0));
    }
    
    int ask(int u,int v){
        if(depth[u]<depth[v]) swap(u,v);
        
        for(int st=1<<(Log-1),i=Log-1;i>=0;i--,st>>=1){
            if(st<=depth[u]-depth[v]){
                u=dp[u][i];
            }
        }
        if(u==v) return u;
        
        for(int i=Log-1;i>=0;i--){
            if(dp[v][i]!=dp[u][i]){
                v=dp[v][i];
                u=dp[u][i];
            }
        }
        return dp[u][0];
    }

};

#endif






















