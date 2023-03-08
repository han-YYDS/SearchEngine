/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：SplitTool.h
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#ifndef _SPLITTOOL_H_
#define _SPLITTOOL_H_

#include <string>
#include <vector>

using std::string;
using std::vector;


class SplitTool
{
    // 构造函数
    SplitTool();
    
    // 虚析构函数
    virtual ~SplitTool();
    
    // 分词函数，纯虚函数，提供接口
    virtual vector<string> cut(const string & sentence) = 0;

};

#endif
