
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //env
#include <string.h>

#include "ngx_global.h"

//设置可执行程序标题相关函数：分配内存，并且把环境变量拷贝到新内存中来
// 该方法就是把系统的环境变量搬走，这样的话修改argv[]的内容就算处理到了environ也没关系
void ngx_init_setproctitle(){    
    int i;
    // 统计环境变量所占的内存。注意判断方法是environ[i]是否为空作为环境变量结束标记
    for (i = 0; environ[i]; i++) {
        g_environlen += strlen(environ[i]) + 1; //+1是因为末尾有\0,是占实际内存位置的，要算进来
    } //end for

    //这里无需判断penvmen == NULL,有些编译器new会返回NULL，有些会报异常，但不管怎样，如果在重要的地方new失败了，你无法收场，让程序失控崩溃，助你发现问题为好； 
    gp_envmem = new char[g_environlen]; // 在nginx.cxx文件中创建的全局变量
    memset(gp_envmem,0,g_environlen);   // 内存要清空防止出现问题

    char *ptmp = gp_envmem;

    // 把原来的内存内容搬到新地方来
    for (i = 0; environ[i]; i++) {
        size_t size = strlen(environ[i])+1 ; //不要拉下+1，否则内存全乱套了，因为strlen是不包括字符串末尾的\0的
        strcpy(ptmp,environ[i]);      // 把原环境变量内容拷贝到新地方【新内存】
        environ[i] = ptmp;            // 然后还要让新环境变量指向这段新内存
        ptmp += size;
        // 上面这两行的代表表示，指针往后移动的过程
        // environ[i] = ptmp 表示将environ[i]的值为 指针ptmp指向的内容，而内容就是strcpy(ptmp,environ[i]);
        // 然后ptmp +=size 表示ptmp指针往后移动了size个大小，指向下一个空位置，然后接收下一个环境变量
    }
    return;
}

//设置可执行程序标题
void ngx_setproctitle(const char *title){
    //我们假设，所有的命令 行参数我们都不需要用到了，可以被随意覆盖了；
    //注意：我们的标题长度，不会长到原始标题和原始环境变量都装不下，否则怕出问题，不处理
    
    //(1)计算新标题长度
    size_t ititlelen = strlen(title); 

    //(2)计算总的原始的argv那块内存的总长度【包括各种参数】
    size_t e_environlen = 0;     //e表示局部变量吧
    for (int i = 0; g_os_argv[i]; i++){
        e_environlen += strlen(g_os_argv[i]) + 1;
    }

    size_t esy = e_environlen + g_environlen; //argv和environ内存总和
    if( esy <= ititlelen){
        //你标题多长啊，我argv和environ总和都存不下？注意字符串末尾多了个 \0，所以这块判断是 <=【也就是=都算存不下】
        return; // 这样就直接不处理了
    }

    //空间够保存标题的，够长，存得下，继续走下来    

    //(3)设置后续的命令行参数为空，表示只有argv[]中只有一个元素了，这是好习惯；防止后续argv被滥用，因为很多判断是用argv[] == NULL来做结束标记判断的;
    g_os_argv[1] = NULL;  // 让程序只有argv[0]一个元素

    //(4)把标题弄进来，注意原来的命令行参数都会被覆盖掉，不要再使用这些命令行参数,而且g_os_argv[1]已经被设置为NULL了
    char *ptmp = g_os_argv[0]; //让ptmp指向g_os_argv所指向的内存
    strcpy(ptmp,title);
    ptmp += ititlelen; //跳过标题，表示ptmp指向了传进来的标题的长度之后

    //(5)把剩余的原argv以及environ所占的内存全部清0，否则会出现在ps的cmd列可能还会残余一些没有被覆盖的内容；
    size_t cha = esy - ititlelen;  //内存总和减去标题字符串长度(不含字符串末尾的\0)，剩余的大小，就是要memset的；
    memset(ptmp,0,cha);            // 将ptmp所指向的内容的往后cha个内容的值都设置为0
    return;
}