//
//  Utils.cpp
//  FPT
//
//  Created by Yun Peng on 12/27/14.
//  Copyright (c) 2014 Yun Peng. All rights reserved.
//

#include "Utils.h"
using namespace std;

void FPT_ASSERT(bool f) {
    if(f == false) {
        cout << "Assertion error" << endl;
        exit(1);
    }
}

void FPT_ASSERT_INFO( bool f, const char *info) {
    if(f == false) {
        cout << "Assertion Error: " << info << endl;;
        exit(1);
    }
}

bool CheckLabelFormat(const string &label) {
    if (label == "") return false;
    for (int i=0;i<label.length();i++) {
        if (!((label[i] >= 'a' && label[i] <= 'z' )
            ||(label[i] >= 'A' && label[i] <= 'Z' )
            ||(label[i] >= '0' && label[i] <= '9')
            || label[i] == '_'
            || label[i] == '.' ))
        return false;
    }
    return true;
}

string itoa(int x) {
    if (x==0) return "0";
    string ans = "";
    string sign = (x<0)?"-":"";
    x = abs(x);
    string tmp = " ";
    while (x) {
        tmp[0] = x % 10 + '0';
        ans = tmp + ans;
        x /= 10;
    }
    return sign + ans;
}

string itoa(void * _x) {
    long long x = (long long) _x;
    if (x==0) return "0";
    string ans = "";
    string sign = (x<0)?"-":"";
    x = abs(x);
    string tmp = " ";
    while (x) {
        tmp[0] = x % 10 + '0';
        ans = tmp + ans;
        x /= 10;
    }
    return sign + ans;
}


































