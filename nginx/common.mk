
#.PHONY:all clean 

ifeq ($(DEBUG),true)
#-g是生成调试信息。GNU调试器可以利用该信息
# 如果DEBUG == true 则执行以下语句， 在config.mk文件中查看DEBUG的值
# 引用一个变量使用 $() 
# 如果使用的仅仅为c语言，则可以使用gcc 本项目也采用c++编写所以使用g++
CC = g++ -g
VERSION = debug
else
CC = g++
VERSION = release
endif

#CC = gcc

# $(wildcard *.c)表示扫描当前目录下所有.c文件, 注意是当前目录下的所有.c文件
# 当文件处于app目录下时，SRCS = nginx.c ngx_conf.c
SRCS = $(wildcard *.cxx)

# OBJS = nginx.o ngx_conf.o  这么一个一个增加.o太麻烦，下行换一种写法：把字符串中的.c替换为.o
# 将SRCS目录下的所有.cxx文件替换为.o文件
OBJS = $(SRCS:.cxx=.o)

# 把字符串中的.c替换为.d， 在.d文件中，存储的是对应的.c文件的依赖关系，详情请查看.d文件
# DEPS = nginx.d ngx_conf.d
DEPS = $(SRCS:.cxx=.d)

# 指定最终生成的可执行文件的位置
# 可以指定BIN文件的位置,addprefix是增加前缀函数
# 查看config.mk中的BUILD_ROOT的位置pwd  BIN = /mnt/hgfs/LinuxServerShare/nginx
BIN := $(addprefix $(BUILD_ROOT)/,$(BIN))

# 定义存放ojb文件的目录，目录统一到一个位置才方便后续链接，不然整到各个子目录去，不好链接
# 注意下边这个字符串，末尾不要有空格等否则会语法错误
# 统一的定义需要存放的文件夹的位置，定义文件夹
LINK_OBJ_DIR = $(BUILD_ROOT)/app/link_obj
DEP_DIR = $(BUILD_ROOT)/app/dep

# -p是递归创建目录，没有就创建，有就不需要创建了
# 执行shell命令 mkdir -p(循环递归创建) 
$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

# 我们要把目标文件生成到上述目标文件目录去，利用函数addprefix增加个前缀，在OBJS（OBJS = nginx.o ngx_conf.o）各个文件的前面添加路径LINK_OBJ_DIR
# /mnt/hgfs/linux/nginx/app/link_obj/ngx_signal2.o       /mnt/hgfs/linux/nginx/app/link_obj/ngx_signal.o
# := 在解析阶段直接赋值常量字符串【立即展开】，而 = 在运行阶段，实际使用变量时再进行求值【延迟展开】
# /mnt/hgfs/linux/nginx/app/link_obj/nginx.o             /mnt/hgfs/linux/nginx/app/link_obj/ngx_conf.o 
OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(OBJS))
DEPS := $(addprefix $(DEP_DIR)/,$(DEPS))

# 找到目录中的所有.o文件（编译出来的）
LINK_OBJ = $(wildcard $(LINK_OBJ_DIR)/*.o)
# 因为构建依赖关系时app目录下这个.o文件还没构建出来（程序的bug，所以需要手动加进来），所以LINK_OBJ是缺少这个.o的，我们 要把这个.o文件加进来
LINK_OBJ += $(OBJS)



# make找第一个目标开始执行[每个目标[就是我们要生成的东西]，其实都是定义一种依赖关系]，目标的格式为：
# 目标：目标依赖【可以省略】
#	要执行的命令【可以省略】
# 如下这行会是开始执行的入口，执行就找到依赖项$(BIN)去执行了，同时，这里也依赖了$(DEPS)，这样就会生成很多.d文件了
all:$(DEPS) $(OBJS) $(BIN)

#这里是诸多.d文件被包含进来，每个.d文件里都记录着一个.o文件所依赖哪些.c和.h文件。内容诸如 nginx.o: nginx.c ngx_func.h
#我们做这个的最终目的说白了是，即便.h被修改了，也要让make重新编译我们的工程，否则，你修改了.h，make不会重新编译，那不行的
#有必要先判断这些文件是否存在，不然make可能会报一些.d文件找不到
ifneq ("$(wildcard $(DEPS))","")   #如果不为空,$(wildcard)是函数【获取匹配模式文件名】，这里 用于比较是否为""
include $(DEPS)  
endif

#---------------------------------------------------------------------1-begin---------------------------------------------------------------------------------
#$(BIN):$(OBJS)
# 表示将LINK_OBJ文件中的所有的文件链接起来，并且生成一个目标可执行文件。
$(BIN):$(LINK_OBJ)
	@echo "------------------------build $(VERSION) mode--------------------------------!!!"

#一些变量：$@：目标，     $^：所有目标依赖
# gcc -o 是生成可执行文件
	$(CC) -o $@ $^
#---------------------------------------------------------------------1-end-----------------------------------------------------------------------------------



#---------------------------------------------------------------------2-begin---------------------------------------------------------------------------------
#%.o:%.c
$(LINK_OBJ_DIR)/%.o:%.cxx
# gcc -c是生成.o目标文件   -I可以指定头文件的路径
# 如下不排除有其他字符串，所以从其中专门把.c过滤出来 
# $(CC) -o $@ -c $^
# 下列执行的意思是： 存在多少个.cxx文件，均生成对应的.o文件，存放到 $@ 文件夹中去，即$(LINK_OBJ_DIR)/%.o
	$(CC) -I$(INCLUDE_PATH) -o $@ -c $(filter %.cxx,$^)
#---------------------------------------------------------------------2-end-----------------------------------------------------------------------------------



#---------------------------------------------------------------------3-begin---------------------------------------------------------------------------------
#我们现在希望当修改一个.h时，也能够让make自动重新编译我们的项目，所以，我们需要指明让.o依赖于.h文件
#那一个.o依赖于哪些.h文件，我们可以用“gcc -MM c程序文件名” 来获得这些依赖信息并重定向保存到.d文件中
#.d文件中的内容可能形如：nginx.o: nginx.c ngx_func.h
#%.d:%.c
$(DEP_DIR)/%.d:%.cxx
# gcc -MM $^ > $@
#.d文件中的内容形如：nginx.o: nginx.c ngx_func.h ../signal/ngx_signal.h，但现在的问题是我们的.o文件已经放到了专门的目录
# 所以我们要正确指明.o文件路径这样，对应的.h,.c修改后，make时才能发现，这里需要用到sed文本处理工具和一些正则表达式语法，不必深究
# gcc -MM $^ | sed 's,\(.*\)\.o[ :]*,$(LINK_OBJ_DIR)/\1.o:,g' > $@
# echo 中 -n表示后续追加不换行
	echo -n $(LINK_OBJ_DIR)/ > $@
#	gcc -MM $^ | sed 's/^/$(LINK_OBJ_DIR)&/g' > $@
#  >>表示追加
	gcc -I$(INCLUDE_PATH) -MM $^ >> $@

# 下列执行的命令的意思是：
# 将所有的.cxx文件，均生成对应的.d文件，并且存放到 $@ 文件夹中去，即 $(DEP_DIR)/%.d 

# 上述的功能为：输出相关的依赖关系,比如：输入命令： gcc -I/mnt/hgfs/LinuxServerShare/nginx/_include -MM nginx.c 表示在依赖目录(-I)下，查找出nginx.c的依赖关系
# 输出内容：
# nginx.o: nginx.cxx /mnt/hgfs/LinuxServerShare/nginx/_include/ngx_c_conf.h \
 /mnt/hgfs/LinuxServerShare/nginx/_include/ngx_global.h \
 /mnt/hgfs/LinuxServerShare/nginx/_include/ngx_signal.h \
 /mnt/hgfs/LinuxServerShare/nginx/_include/ngx_func.h

#上行处理后，.d文件中内容应该就如：/mnt/hgfs/linux/nginx/app/link_obj/nginx.o: nginx.c ngx_func.h ../signal/ngx_signal.h
#---------------------------------------------------------------------3-end-----------------------------------------------------------------------------------



#----------------------------------------------------------------nbegin-----------------
# clean:			
# rm 的-f参数是不提示强制删除
# 可能gcc会产生.gch这个优化编译速度文件
#	 rm -f $(BIN) $(OBJS) $(DEPS) *.gch
#----------------------------------------------------------------nend------------------







#--------------------------------------------------------------------------------------------------------------------------------------------------
# common.mk文件
#    （1）makefile文件就是common.mk文件在发挥作用
#
#	 （2）makefile文件具备较高的编写规范，一旦书写不规范就会出错
#		- makefile命令行前的缩进需要使用tab，而不是空格
#		- 在Windows环境下可能会导致相关（makefile,common.mk,config.mk）文件的编码方式编程utf-8，在使用make命令的时候
#		  会出现[makefile:1: *** missing separator. Stop]的错误，需要改变文件的编码方式。
#
#	  (3) config.mk文件中的BUILD_DIR表示需要编译的目录,所有包含.c文件夹的目录都需要添加进去，同理在对应的文件夹下也要有makefile文件
#
#
#--------------------------------------------------------------------------------------------------------------------------------------------------



