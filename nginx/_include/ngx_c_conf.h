﻿
#ifndef __NGX_CONF_H__
#define __NGX_CONF_H__

#include <vector>

#include "ngx_global.h"  //一些全局/通用定义

//类名可以遵照一定的命名规则规范，比如老师这里，第一个字母是C，后续的单词首字母大写
class CConfig{
//---------------------------------------------------
//这段代码老师在《c++从入门到精通》 多线程这章老师提过 单例设计模式，就是如下这些代码，大家即便没学过，也可以现在学
private:        // 为了防止外部去调用类的构造函数而创建实例，我们需要将构造函数的访问权限标记为protect或者private
	CConfig();
public:
	~CConfig();
private:
	static CConfig *m_instance;

public:	
// 第一次调用的时候，建议在主线程中使用，而不是在分支线程中使用
	static CConfig* GetInstance() 
	{	
		if(m_instance == NULL){     // 如果该值为空，表示其为第一次被调用，加上一把锁
			//加锁
			if(m_instance == NULL){					
				m_instance = new CConfig();
				static CGarhuishou cl; 
			}
			//放锁		
		}
		return m_instance;
	}	
	class CGarhuishou  //类中套类，用于释放对象
	{
	public:				
		~CGarhuishou()
		{
			if (CConfig::m_instance)
			{						
				delete CConfig::m_instance;
				CConfig::m_instance = NULL;				
			}
		}
	};
    
//------------------------------------------------------------------------------------------------------
// 重点是以下的成员函数，分时实现配置文件的装载和实现
public:
    //装载配置文件
    bool Load(const char *pconfName); 
    //根据ItemName获取配置信息字符串，不修改不用互斥
	const char *GetString(const char *p_itemname);
    //根据ItemName获取数字类型配置信息，不修改不用互斥
	int  GetIntDefault(const char *p_itemname,const int def);

public:
	std::vector<LPCConfItem> m_ConfigItemList; //存储配置信息的列表

};
//------------------------------------------------------------------------------------------------------

#endif





//--------------------------------------------------------------------------------------------------------------------------------------------------
// 单例设计模式：http://t.csdnimg.cn/4Uf2e
//  （1）单例模式是指在整个系统生命周期内，保证一个类只能产生一个实例，确保该类的唯一性。(服务器在main中只读取一次配置文件)
//  
//   (2) 为什么需要使用到哪里模式进行开发？
//       - 节省资源。一个类只有一个实例，不存在多份实例，节省资源。
//       - 方便控制。在一些操作公共资源的场景时，避免了多个对象引起的复杂操作。
//
//  （3）但是在实现单例模式时，需要考虑到线程安全的问题。
//       - 在拥有共享数据的多条线程并行执行的程序中，线程安全的代码会通过同步机制保证各个线程都可以正常且正确的执行，不会出现数据污染等意外情况。
//
//   (4) 如何保证线程安全？
//       - 给共享的资源加把锁，保证每个资源变量每时每刻至多被一个线程占用。
//       - 让线程也拥有资源，不用去共享进程中的资源。如：使用threadlocal可以为每个线程维护一个私有的本地变量。
//
//--------------------------------------------------------------------------------------------------------------------------------------------------