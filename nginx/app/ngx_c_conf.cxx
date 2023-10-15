﻿
//系统头文件放上边
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

//自定义头文件放下边,因为g++中用了-I参数，所以这里用<>也可以
#include "ngx_func.h"     //函数声明
#include "ngx_c_conf.h"   //和配置文件处理相关的类,名字带c_表示和类有关

// 静态成员赋值
CConfig *CConfig::m_instance = NULL;

// 构造函数
CConfig::CConfig(){		
}

// 析构函数
CConfig::~CConfig(){    
	std::vector<LPCConfItem>::iterator pos;	
	for(pos = m_ConfigItemList.begin(); pos != m_ConfigItemList.end(); ++pos)
	{		
		delete (*pos);
	}//end for
	m_ConfigItemList.clear(); 
}

//装载配置文件：将根目录下的nginx.conf读取出来
bool CConfig::Load(const char *pconfName) {   
    FILE *fp;
    fp = fopen(pconfName,"r");
    if(fp == NULL)
        return false;

    // 每一行配置文件读出来都放这里
    char  linebuf[501];   // 每行配置都不要太长，保持<500字符内，防止出现问题
    
    // 走到这里，文件打开成功 
    while(!feof(fp))     // 检查文件是否结束 ，没有结束则条件成立
    {    
        //大家要注意老师的写法，注意写法的严密性，商业代码，就是要首先确保代码的严密性
        if(fgets(linebuf,500,fp) == NULL) //从文件中读数据，每次读一行，一行最多不要超过500个字符 
            continue;
        //
        if(linebuf[0] == 0)
            continue;

        //处理注释行
        if(*linebuf==';' || *linebuf==' ' || *linebuf=='#' || *linebuf=='\t'|| *linebuf=='\n')
			continue;
        
    lblprocstring:
        // 屁股后边若有换行，回车，空格等都截取掉
		if(strlen(linebuf) > 0){
            // 如果每一行的末尾是换行，回车，空格，等都将对应位置给删除截取掉。
			if(linebuf[strlen(linebuf)-1] == 10 || linebuf[strlen(linebuf)-1] == 13 || linebuf[strlen(linebuf)-1] == 32) {
				linebuf[strlen(linebuf)-1] = 0;
				goto lblprocstring;
			}		
		}
        if(linebuf[0] == 0) // 单独的每一行出现若干个空格，也不读取配置文件
            continue;
        if(*linebuf=='[')   // [ 开头的也不处理，有些时候大型项目会有不同程序员写的配置，用[username]表示是谁写的配置文件，所以这一行也不做处理
			continue;

        // 这种 “ListenPort = 5678”走下来；
        // 处理完成所有的个人客观书写习惯，截取出来的配置文件就是 "ItemName = ItemContent"类型
        char *ptmp = strchr(linebuf,'=');
        if(ptmp != NULL){
            // new一个对应的结构体来读取配置文件的详细配置项
            // ngx_global.h中的结构定义
            // typedef struct
            // {
            //     char ItemName[50];
            //     char ItemContent[500];
            // }CConfItem,*LPCConfItem;

            // 注意第一行：指向结构体的指针
            LPCConfItem p_confitem = new CConfItem;                    // 注意前边类型带LP，后边new这里的类型不带

            // void *memset(void *str, int c, size_t n) 
            // 复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符
            // 作用：是在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
            memset(p_confitem,0,sizeof(CConfItem)); 

            strncpy(p_confitem->ItemName,linebuf,(int)(ptmp-linebuf)); // 等号左侧的拷贝到p_confitem->ItemName      ptmp就是=
            strcpy(p_confitem->ItemContent,ptmp+1);                    // 等号右侧的拷贝到p_confitem->ItemContent

            // 将对应的属性名和属性值的左右存在的空格全部删除掉
            Rtrim(p_confitem->ItemName);
			Ltrim(p_confitem->ItemName);
			Rtrim(p_confitem->ItemContent);
			Ltrim(p_confitem->ItemContent);

            //printf("itemname=%s | itemcontent=%s\n",p_confitem->ItemName,p_confitem->ItemContent); 
            // 注意的是：m_ConfigItemList里面储存的是指针：std::vector<LPCConfItem> m_ConfigItemList;  所以加的是p_confitem
            m_ConfigItemList.push_back(p_confitem);  //内存要释放，因为这里是new出来的 
        } //end if
    } //end while(!feof(fp)) 

    fclose(fp); // 这步不可忘记
    return true;
}

// 根据ItemName获取配置信息字符串，不修改不用互斥
// 作用：通过p_itemname获取配置项目的信息字符串
const char *CConfig::GetString(const char *p_itemname){
	std::vector<LPCConfItem>::iterator pos;	
    // 采用迭代器进行字符串的比较。
	for(pos = m_ConfigItemList.begin(); pos != m_ConfigItemList.end(); ++pos)
	{	
		if(strcasecmp( (*pos)->ItemName,p_itemname) == 0)
			return (*pos)->ItemContent;
	}//end for
	return NULL;
}

//根据ItemName获取数字类型配置信息，不修改不用互斥
int CConfig::GetIntDefault(const char *p_itemname,const int def){
	std::vector<LPCConfItem>::iterator pos;	
	for(pos = m_ConfigItemList.begin(); pos !=m_ConfigItemList.end(); ++pos)
	{	
		if(strcasecmp( (*pos)->ItemName,p_itemname) == 0)
            // int atoi(const char *str ); 把字符串转化为正整数
			return atoi((*pos)->ItemContent);
	}//end for
	return def;
}



