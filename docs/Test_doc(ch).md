# 测试文档

## 1 命令解释与预期

* help
> 打印帮助

* version
> 打印版本号

* config show
> 打印全部covscript及csman配置变量，多为路径

* config show \<key\>
> 打印一个键值为key的配置变量，多为路径

* config set \<key\> \<value\>
> 设置一个键值为key的配置变量，.csman_config文件受到相应改动

* list
> 打印所有包，以及依赖的runtime版本

* install \<package\>
> 安装一个包及依赖，pac_repo/和imports/下新增包文件，pac_repo受到改动

* install \<package\> [-f/--fix]
> 仅重新安装一个包，pac_repo/和imports/下新增包文件，pac_repo受到改动

* uninstall \<package\> 
> 删除一个包及支持，pac_repo/和imports/删除包文件，pac_repo受到改动

* uninstall \<package\> [-o/--only]
> 仅删除一个包，pac_repo/和imports/删除包文件，pac_repo受到改动

## 2 测试执行顺序

你他妈不会一个一个照着上面测吗？？？？脑子被李毅鸡巴捅了非要我一条条写？