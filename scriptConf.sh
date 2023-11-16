# 注释
# 每次进入上电状态都会切换播放下一组数据
# 以#开始代表这行是注释
# 以START开始，则代表这行是上电状态要执行的命令，命令从上往下执行
# 以SLEEP开始，则代码这行是休眠状态要执行的命令，命令从上往下执行
# START PLAYBAG，这条是播包的命令，不可以修改

START echo >> 开始命令1
START echo >> 开始命令2
START echo >> 开始命令3
START PLAYBAG
START echo >> 开始命令4

SLEEP echo >> 休眠命令1
SLEEP echo >> 休眠命令2
SLEEP echo >> 休眠命令3
SLEEP echo >> 休眠命令4
