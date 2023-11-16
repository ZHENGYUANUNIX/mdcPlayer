#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include "player.h"

static int printHelp(const char *ptrName)
{
    std::cout << "[" << ptrName << "工具使用说明]\n"
              << "提示:\n"
              << "\t1. 支持在上电/休眠模式下执行一些指令和进行播包\n"
              << "\t2. 播包会按照rtfbag实际的时间顺序、以子文件夹为单位，休眠模切换为上电模式后，会切包播放\n"
              << "\t3. 在后台运行程序时，请执行\"nohup " << ptrName << " -c 脚本配置文件 -d 播放数据目录 >> log.txt &\"\n"
              << "\n"
              << "选项:\n"
              << "\t-h [ ] 帮助\n"
              << "\t-c [*] 导入脚本配置文件，文件内的配置参数见说明尾部\n"
              << "\t-d [*] 要读取播包数据的目录\n"
              << "\n"
              << "\n"
              << "---------------------------\n"
              << "以下是配置参数脚本的内容说明:\n"
              << "# 注释\n"
              << "# 每次进入上电状态都会切换播放下一组数据\n"
              << "# 以#开始代表这行是注释\n"
              << "# 以START开始，则代表这行是上电状态要执行的命令，命令从上往下执行\n"
              << "# 以SLEEP开始，则代码这行是休眠状态要执行的命令，命令从上往下执行\n"
              << "# 以SKIP开始，则代码这行是播包要忽略的Event名称\n"
              << "# START PLAYBAG，这条是播包的命令，不可以修改\n"
              << "\n"
              << "START echo 开始命令1\n"
              << "START echo 开始命令2\n"
              << "START echo 开始命令3\n"
              << "START PLAYBAG\n"
              << "START echo 开始命令4\n"
              << "\n"
              << "SLEEP echo 休眠命令1\n"
              << "SLEEP echo 休眠命令2\n"
              << "SLEEP echo 休眠命令3\n"
              << "SLEEP echo 休眠命令4\n"
              << "SLEEP echo 休眠命令4\n"
              << "\n"
              << "SKIP /Gac/Deploy/FusionMapService/FusionMap[/Gac/Deploy/DdsRequiredServiceInstance/FusionMapDdsProvidedServiceInstance1422]\n"
              << "SKIP /Gac/Deploy/FusionObstacleService/FusionObstacle[/Gac/Deploy/DdsRequiredServiceInstance/FusionObstacleDdsProvidedServiceInstance1315]\n"
              << "SKIP /Gac/Deploy/FusionTrafficSignService/FusionTrafficSign[/Gac/Deploy/DdsRequiredServiceInstance/FusionTrafficSignDdsProvidedServiceInstance1428]\n"
              << "SKIP /Gac/Deploy/InternalService/InternalPerceptionObjectService/InternalPerceptionObject[/Gac/Deploy/DdsRequiredServiceInstance/InternalPerceptionObjectDdsProvidedServiceInstance1311]\n"
              << "SKIP /Gac/Deploy/FusionTrafficSignService/FusionTrafficSign[/Gac/Deploy/DdsRequiredServiceInstance/FusionTrafficSignDdsProvidedServiceInstance1428]\n"
              << std::endl;
    return 0;
}

void signalHandle(int32_t)
{
    Player::s_bRunnable.store(false);
    std::cout << "收到退出信号, 程序准备退出" << std::endl;
    signal(SIGINT, SIG_DFL);
}


int main(int argc, char** argv)
{
    HString strAppName(argv[0]);
    if (argc < 3) {
        return printHelp(strAppName.realName().c_str());
    }
    std::string strDir, strConf;
    signal(SIGINT, signalHandle);
    int nOpt = -1;
    while ((nOpt = getopt(argc, argv, "hc:d:")) != -1) {
        switch (nOpt) {
            case 'h': {
                return printHelp(strAppName.realName().c_str());
            }
            case 'c': {
                strConf = optarg;
                break;
            }
            case 'd': {
                strDir = optarg;
                break;
            }
            default:
                break;
        }
    }
    if (strDir.empty() || strConf.empty()) {
        return printHelp(strAppName.realName().c_str());
    }
    Player player;
    if (player.importBags(strDir.c_str()) && player.readScript(strConf.c_str())) {
        player.startUp();
        while (Player::s_bRunnable.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    std::cout << "程序退出" << std::endl;
    return 0;
}
