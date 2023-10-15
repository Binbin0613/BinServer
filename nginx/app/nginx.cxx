#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ngx_c_conf.h"  //和配置文件处理相关的类,名字带c_表示和类有关
#include "ngx_signal.h"
#include "ngx_func.h"    //各种函数声明


//和设置标题有关的全局量
char **g_os_argv;            // 原始命令行参数数组,在main中会被赋值
char *gp_envmem = NULL;      // 指向自己分配的env环境变量的内存
int  g_environlen = 0;       // 环境变量所占内存大小

int main(int argc, char *const *argv){             
    g_os_argv = (char **) argv; // 将main函数的参数argv赋值给创建的全局变量 g_os_argv
    // 由于是在将系统的环境变量搬家，在main函数中应该尽早的执行该方法
    ngx_init_setproctitle();    // 把环境变量搬家
    
    //1、首先读取配置文件，提供给后续使用-我们在main中，先把配置读出来，供后续使用 
    CConfig *p_config = CConfig::GetInstance();     // 单例类 ngx_c_conf.h中的单例设计模式
    if(p_config->Load("nginx.conf") == false){      // 把配置文件内容载入到内存
        printf("配置文件载入失败，退出!\n");          // 最为严重的错误，直接选择退出服务器程序 exit(1)
        exit(1);
    }

    //------------------------------------------------------------------begin 测试获取配置文件信息的用法------------------------------------------------------------------
    // int port = p_config->GetIntDefault("ListenPort",0); // 0是缺省值
    // printf("port=%d\n",port);
    // // 获取字符串类型的配置文件
    // const char *DBInfo = p_config->GetString("DBInfo");
    // if(DBInfo != NULL){
    //     printf("DBInfo=%s\n", DBInfo);
    // }
    // // 输出内容：
    // // port=5678
    // // DBInfo=127.0.0.1;1234;myr;123456;mxdb_g
    //------------------------------------------------------------------end 测试获取配置文件信息的用法------------------------------------------------------------------

    


    //------------------------------------------------------------------begin 测试修改服务器程序的名称------------------------------------------------------------------
    // for (int i = 0; environ[i]; i++)
    // {
    //     printf("evriron[%d]地址=%x    " ,i,(unsigned int)((unsigned long)environ[i]));
    //     printf("evriron[%d]内容=%s\n" ,i,environ[i]);
    // }
    // printf("--------------------------------------------------------");
    // ngx_init_setproctitle(); // 把环境变量给搬家
    
    /*for (int i = 0; environ[i]; i++){
        printf("evriron[%d]地址=%x    " ,i,(unsigned int)((unsigned long)environ[i]));
        printf("evriron[%d]内容=%s\n" ,i,environ[i]);
    }*/

    // 我要保证所有命令行参数我都不 用了
    ngx_setproctitle("nginx: Binbin Deng's nginx");  // 启动完成，查看进程名称[nginx: Binbin Deng's nginx]


    //printf("argc=%d,argv[0]=%s\n",argc,argv[0]);
    //strcpy(argv[0],"ce");
    //strcpy(argv[0],"c2212212121322324323e");
    //printf("非常高兴，大家和老师一起学习《linux c++通讯架构实战》\n");    
    
    // printf("environ[0]=%s\n" , environ[0]);
    // printf("environ[1]=%s\n" , environ[1]);
    // printf("environ[2]=%s\n" , environ[2]);
    // printf("environ[3]=%s\n" , environ[3]);
    // printf("environ[4]=%s\n" , environ[4]);

    // 注意的是： argv[]的内容与全局变量evriron数据在内容中是紧紧相连的。
    // printf("%d\n",argc);
    // for(int i = 0; i < argc; ++i){        
    //    printf("argv[%d]地址=%x    " ,i,(unsigned int)((unsigned long)argv[i]));
    //    printf("argv[%d]内容=%s\n",i,argv[i]);
    // }
    // 下边环境变量随便打两个
    // for(int i = 0; i < 2; ++i) {
    //    printf("environ[%d]地址=%x    " ,i,(unsigned int)((unsigned long)environ[i]));
    //    printf("environ[%d]内容=%s\n" ,i,environ[i]);
    // }
    // environ变量的内存紧跟在argv变量内存的后面。argv最后一个指针指向“def”------0x7fffb80f5613:def, 
    // environ第一个指针指向0x7fffb80f5617:ORBIT_SOCKETDIR=/tmp/orbit-lgh，它们相差了四个字节即“def\0”，\0是C语言字符串结束标记。
    //------------------------------------------------------------------end 测试修改服务器程序的名称------------------------------------------------------------------


    

    //myconf();
    //mysignal();
    // printf("非常高兴，大家和老师一起学习《linux c++通讯架构实战》\n");    
    // myconf();
    // mysignal();
    for(;;)
    {
        sleep(1); //休息1秒
        printf("休息1秒\n");
    }
    

    // 对于因为设置可执行程序标题导致的环境变量分配的内存，我们应该释放
    // 对应的分配内容的位置在ngx_setproctitle.cxx文件中，建议对于完成的每个文件夹个功能，都检查一下new的数量和使用valgrind工具检查
    if(gp_envmem){
        delete []gp_envmem;
        gp_envmem = NULL;
    }
    printf("程序退出，再见!\n");
    return 0;
}


