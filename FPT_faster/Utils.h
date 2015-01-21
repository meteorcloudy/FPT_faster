//
//  Utils.h
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#ifndef FPT_Utils_h
#define FPT_Utils_h

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <queue>
#include <vector>
#include <stack>
#include <map>
#include <unordered_set>
#include <unordered_map>

using namespace std;

void FPT_ASSERT(bool f);
void FPT_ASSERT_INFO( bool f, const char *info);
bool CheckLabelFormat(const string &label);
string itoa(int x);
string itoa(void * _x);

#endif
