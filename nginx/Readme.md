### 一、项目的目录结构设计
#### (1) 目录结构规划（make编译）
	特别注意：不固安是目录还是文件，文件名中一律不要带空格，一律不要用中文，最好的方式：字母，数字，下划线；
		     不要给自己找麻烦，远离各种坑
	主目录名nginx
	(a) _include目录：专门存放各种头文件； 如果分散:#include "sfaf/sdafas/safd.h"
	(b) app目录：放主应用程序.c(main()函数所在的文件)以及一些比较核心的文件；
		    (b.1) link_obj：临时目录：会存放临时的.o文件，这个目录不手工创建，后续用makefile脚本来创建
		    (b.2) dep：临时目录，会存放临时的.d开头的依赖文件，依赖文件能够告知系统哪些相关的文件发生变化，需要重新编译，后续用makefile脚本来创建
		    (b.3) nginx.c：主文件，main()入口函数就放到这里；
		    (b.4) ngx_conf.c  ，普通的源码文件，跟主文件关系密切，又不值得单独放在 一个目录；
	(c) misc目录：专门存放各种杂合性的不好归类的1到多个.c文件；暂时为空
	(d) net目录：专门存放和网络处理相关的1到多个.c文件，暂时为空
	(e) proc目录：专门存放和进程处理有关的1到多个.c文件，暂时为空
	(f) signal目录：专门用于存放和信号处理 有关的1到多个.c文件；
		ngx_signal.c

[linuxserver@localhost nginx]$ tree
├── app
│   ├── makefile
│   ├── nginx.c
│   └── ngx_conf.c
├── common.mk
├── config.mk
├── _include
│   ├── ngx_c_conf.h
│   ├── ngx_func.h
│   ├── ngx_signal2.h
│   └── ngx_signal.h
├── makefile
├── misc
├── net
├── proc
├── Readme.md
└── signal
    ├── makefile
    └── ngx_signal.c

### 二、makefile文件夹的设计
    (1) 传统的方案是使用gcc/g++命令完成对文件的编译，最终生成一个可执行文件，但是随着项目的复杂变大，我们需要为每一个.c/.cpp文件生成一个.o文件
         多个.c文件生成多个.o文件，最终这些.o文件会被链接到一起，生成一个可执行文件。使用gcc/g++是不行的。

    (2) 借助make命令来完成项目的编译，链接。最终生成可执行文件，一般大型项目都用make来完成
 	 
    (3) make工作原理：去当前目录当中的读取一个makefile的文件(文本文件)，将文本文件中的规则把源代码编译成可执行文件，程序员的目的就是编写makefile文件
                   makefile里面定义了如何去编译整个项目的编译，链接规则。
		
    (4) 总结为：[makefile就是编译工程中需要用到的各种源文件等等的一个依赖关系描述(各种.c/.cpp文件生成.o文件，.o文件的依赖关系)]

    (5) makefile文件为文本文件，utf-8编码方式，没有扩展名，一般而言放在项目根目录下[也会根据需要放在子目录]

    (6) 本项目的makefile文件的编写方式：
		① nginx的根目录放三个文件
			- makefile:  编译项目的入口脚本，编译项目从这里开始，起到总体控制的作用
			- config.mk: 一个配置脚本，被makefile文件包含，单独分离出来是为了应付一些可变的东西,所以，一般变动的东西都往这里搞；
			- common.mk: 是最重要最核心的编译脚本，定义makefile的编译规则，依赖规则等，通用性很强的一个脚本，
 				         并且各个子目录中都用到这个脚本来实现对应子目录的.c文件的编译；
		② 每一个子目录(app,signal)下都有一个叫做makefile的文件，每个这个makefile文件，都会包含根目录下的common.mk文件
		  从而实现自己这个子目录下的.c文件的编译
		③ 其他规划：
			- app/link_obj: 临时目录，用于存放.o目标文件
			- app/dep:		存放.d依赖关系文件
		
    (7) makefile脚本用法
			- make 编译生成可执行文件
			- make clean 如下代码所示，删除下列文件夹

### 三、基础设施之配置文件读取


### 四、