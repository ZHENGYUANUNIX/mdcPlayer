# **mdcPlayer**工具使用说明


## 提示
	1.在上电/休眠模式下执行一些指令和进行播包
	2.会按照rtfbag实际的时间顺序、以子文件夹为单位，休眠模切换为上电模式后，会切包播放
	3.台运行程序时，请执行"nohup mdcPlayer -c 脚本配置文件 -d 播放数据目录 >> log.txt &"


## 选项
	-h [ ] 帮助
	-c [*] 导入脚本配置文件，文件内的配置参数见说明尾部
	-d [*] 要读取播包数据的目录


## 配置参数脚本的内容说明

```
# 注释
# 每次进入上电状态都会切换播放下一组数据
# 以#开始代表这行是注释
# 以START开始，则代表这行是上电状态要执行的命令，命令从上往下执行
# 以SLEEP开始，则代码这行是休眠状态要执行的命令，命令从上往下执行
# 以SKIP开始，则代码这行是播包要忽略的Event名称
# START PLAYBAG，这条是播包的命令，不可以修改

START echo 开始指令1
START echo 开始指令2
START echo 开始指令3
START PLAYBAG
START echo 开始指令4

SLEEP echo 休眠指令1
SLEEP echo 休眠指令2
SLEEP echo 休眠指令3
SLEEP echo 休眠指令4

SKIP /Gac/Deploy/FusionMapService/FusionMap[/Gac/Deploy/DdsRequiredServiceInstance/FusionMapDdsProvidedServiceInstance1422]
SKIP /Gac/Deploy/FusionObstacleService/FusionObstacle[/Gac/Deploy/DdsRequiredServiceInstance/FusionObstacleDdsProvidedServiceInstance1315]
SKIP /Gac/Deploy/FusionTrafficSignService/FusionTrafficSign[/Gac/Deploy/DdsRequiredServiceInstance/FusionTrafficSignDdsProvidedServiceInstance1428]
SKIP /Gac/Deploy/InternalService/InternalPerceptionObjectService/InternalPerceptionObject[/Gac/Deploy/DdsRequiredServiceInstance/InternalPerceptionObjectDdsProvidedServiceInstance1311]
SKIP /Gac/Deploy/FusionTrafficSignService/FusionTrafficSign[/Gac/Deploy/DdsRequiredServiceInstance/FusionTrafficSignDdsProvidedServiceInstance1428]
```
