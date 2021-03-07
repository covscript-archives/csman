# 开发文档
<br></br>
***
## 1st, 事先约定:
<br></br>

* 我们认为, cs.runtime是一种特殊的包, 大多数情况以包的方法处理
* csman客户端不打算采用任何json解析库用于开发
* 包管理功能的描述: 用户通过csman客户端下载若干包到本地的pac_repo路径下, 但只有适合当前runtime的包才会被csman从pac_repo中挑选出放入CS_IMPORT_PATH中供CovScript使用. 换言之: **pac_repo是CS_IMPORT_PATH的多版本常备包仓库, 以备不时之需** 

* `.json`文件 : &emsp; 服务器上所有索引文件格式为`json`, 分别有:
    > ~~http://csman.info/ **csman.json** (总索引, 为了指引用户下载需要的文件)~~  
    http://csman.info/sources/ **Generic.json** (必需)  
    http://csman.info/sources/ **Linux_GCC_AMD64.json** (linux需要)  
    http://csman.info/sources/ **macOS_clang_AMD64.json** (macOS需要)  
    http://csman.info/sources/ **Win32_MinGW-w64_AMD64.json** (windows需要)  
    **sources.json** (由2个json文件**合并组成**, 用来存储服务器上**所有可下载包**信息)  


* `_idx`文件 : &emsp; 由于`.json`文件太复杂不好处理, 于是将其转换为`_idx`格式:
    > **sources_idx**:  &emsp; csman真正要处理的**可下载包**信息
***
## 2nd, 文件结构 :
<br></br>
* 项目的代码结构
  + **command.hpp** :&emsp; 命令行模块(主模块)
  + **dir.hpp**     :&emsp; 文件夹操作(小模块)
  + **global.hpp**  :&emsp; 全局信息的记录和交互(功能类)
  + **http.hpp**    :&emsp; 下载和其它网络功能模块(小模块)
  + **idx.hpp**     :&emsp;  `_idx`文件相关操作模块
  + **pac_repo.hpp**:&emsp; 本地包索引与管理模块

+ 客户端结构
  + csman客户端(位于csman家目录:~/.csman/)
  + csman配置文件(**强制**位于~/.csman_config)
  + csman位于包仓库中的已下载包(默认位于COVSCRIPT_HOME/pac_repo/)
  + sources_idx文件(位于csman家目录:~/.csman/)
***
## 3rd, 操作(过程)抽象描述
<br></br>
* csman客户端执行流程大致如下:
  

* 每个环节详细解释如下:


***
## 4th, 功能(函数)抽象描述
<br></br>
+ **dir.hpp**
    + `创建文件夹`


+ **global.hpp**
    + `识别字符串`: 将字符串遵循格式识别为: ABI号类, STD号类, 版本号类, 包名类
    + `contex类`: 可存储, 交换的**csman客户端全局信息**, 包括:
        + 当前runtime的信息
            + ABI, STD, 版本号
        + CovScript**环境变量**
            + COVSCRIPT_HOME, CS_IMPORT_PATH, CS_DEV_PATH
        + 部分csman路径
            + csman_path, pac_repo, config_path, idx_path

+ **http.hpp**
    + `下载`


+ **idx.hpp**
    + `读取sources_idx`: 读取内容并建立**依赖图**结构


+ **pac_repo.hpp**: