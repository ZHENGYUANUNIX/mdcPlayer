#include "player.h"
#include <cstring>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "msgDef.h"

std::atomic_bool Player::s_bRunnable;

Player::Player(const char * ptrReq) : m_strQuery(ptrReq)
{
    m_nCurrentIndex = 0;
    Player::s_bRunnable.store(true);
}

Player::~Player()
{
    Player::s_bRunnable.store(false);
    if (m_ptrThreadReq && m_ptrThreadReq->joinable()) {
        m_ptrThreadReq->join();
        m_ptrThreadReq.reset();
    }
}

bool Player::isWarkUp()
{
    FILE *ptrFile = nullptr;
    char buffer[256] = {0};
    ptrFile = popen(m_strQuery.c_str(), "r");
    bool bRet = false;
    if (ptrFile) {
        while (Player::s_bRunnable.load() && !feof(ptrFile)) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), ptrFile);
            std::string strMsg(buffer);
            if (std::string::npos != strMsg.find("MachineState")) {
                if (std::string::npos != strMsg.find("Startup")) {
                    bRet = true;
                    break;
                }
            }
        }
        pclose(ptrFile);
    }
    if (bRet) {
        std::cout << "当前处于上电状态..." << std::endl;
    }
    else {
        std::cout << "当前的处于休眠状态..." << std::endl;
    }
    return bRet;
}

bool Player::startUp()
{
    waiting();
    m_ptrThreadReq = std::make_shared<std::thread>(std::thread(std::mem_fn(&Player::listening), this));
    return true;
}

bool Player::readScript(const char *ptrFile)
{
    HFile file(ptrFile);
    if (!file.exist()) {
        std::cout << "文件" << ptrFile << "不存在，请检查" << std::endl;
        return false;
    }
    std::ifstream stream(file.name());

    std::string strRead;
    m_vecRunScript.clear();
    m_vecStopScript.clear();
    m_listSkipEvent.clear();
    if (stream) {
        if (stream.is_open()) {
            while (getline(stream, strRead)) {
                if (!strRead.empty()) {
                    HString stringRead(strRead.c_str());
                    if (stringRead.startWith("START")) {
                        stringRead.removeStart("START");
                        m_vecRunScript.push_back(stringRead.simplified());
                    }
                    else if (stringRead.startWith("SLEEP")) {
                        stringRead.removeStart("SLEEP");
                        m_vecStopScript.push_back(stringRead.simplified());
                    }
                    else if (stringRead.startWith("SKIP")) {
                        stringRead.removeStart("SKIP");
                        m_listSkipEvent.push_back(stringRead.simplified());
                    }
                }
                strRead.clear();
            }
            stream.close();
        }
    }
    else {
        std::cout << "文件" << ptrFile << "无法打开，请检查权限" << std::endl;
        return false;
    }
    if (m_vecRunScript.empty()) {
        std::cout << "文件" << ptrFile << "未读取到有效的数据" << std::endl;
        return false;
    }
    std::cout << "读取到的上电状态要执行的命令为: " << std::endl;
    for (const auto & strScript : m_vecRunScript) {
        std::cout << strScript << std::endl;
    }
    if (m_vecStopScript.empty()) {
        std::cout << "读取到的休眠状态要执行的命令为空" << std::endl;
    }
    else {
        std::cout << "读取到的休眠状态要执行的命令为: " << std::endl;
        for (const auto & strScript : m_vecRunScript) {
            std::cout << strScript << std::endl;
        }
    }

    return true;
}

void Player::waiting()
{
    while (Player::s_bRunnable.load() && !isWarkUp()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "等待上电中..." << std::endl;
    }
}

std::string Player::getCurrentTime()
{
    std::string strTime("当前系统时间");
    std::time_t time = std::time(nullptr);
    std::tm* timeLocal = std::localtime(&time);
    strTime.append(std::to_string(timeLocal->tm_year + 1900));
    strTime.append("-");
    strTime.append(std::to_string(timeLocal->tm_mon + 1));
    strTime.append("-");
    strTime.append(std::to_string(timeLocal->tm_mday));
    strTime.append(" ");
    strTime.append(std::to_string(timeLocal->tm_hour));
    strTime.append(":");
    strTime.append(std::to_string(timeLocal->tm_min));
    strTime.append(":");
    strTime.append(std::to_string(timeLocal->tm_sec));
    return strTime;
}

void Player::listening()
{
    m_nCurrentIndex = 0;
    bool bStatusHis = false;
    while (Player::s_bRunnable.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        bool bStatus = isWarkUp();
        if (bStatus && !bStatusHis) {
            bStatusHis = true;
            std::cout << "开始执行上电状态命令" << std::endl;
            for (const auto &strScript : m_vecRunScript) {
                HString strCmd(strScript.c_str());
                if (strCmd.startWith("PLAYBAG")) {
                    std::cout << "开始播包, 第" << m_nCurrentIndex + 1 << "组"  << std::endl;
                    std::string strBagAll;
                    if (m_listBagDirMsg.size() > m_nCurrentIndex) {
                        HString strBagDir(m_listBagDirMsg[m_nCurrentIndex].strDirName.c_str());
                        if (strBagDir.isDir()) {
                            std::vector<std::string> listBags = strBagDir.listFiles(".bag");
                            for (const auto& bagNameTest : listBags) {
                                std::cout << "读到的文件名称" << bagNameTest << std::endl;
                            }
                            if (listBags.empty()) {
                                std::cout << "目录" << strBagDir.name() << "未读取到rtfbag文件，本地播包跳过" << std::endl;
                            }
                            else {
                                std::string strBagsAll;
                                for (const auto& strBag : listBags) {
                                    strBagsAll.append(" ");
                                    strBagsAll.append(strBag);
                                }
                                std::string strScriptPlay = "rtfbag play -l " + strBagsAll;
                                if (!m_listSkipEvent.empty()) {
                                    strScriptPlay += " --skip-events";
                                    for (const auto& strSkip : m_listSkipEvent) {
                                        strScriptPlay.append(" ");
                                        strScriptPlay.append(strSkip);
                                    }
                                }

                                strScriptPlay += " --adjust-clock &";
                                std::cout << getCurrentTime() << ", rtfbag 要执行的播包指令为: " << strScriptPlay << std::endl;
                                if (system(strScriptPlay.c_str())) {
                                    std::cout << getCurrentTime() << ", " << strScriptPlay << "\t执行成功" << std::endl;
                                }
                            }
                        }
                    }
                }
                else {
                    if (system(strScript.c_str())) {
                        std::cout << strScript << "\t执行成功" << std::endl;
                    }
                }
            }
            if (m_nCurrentIndex++ >= m_listBagDirMsg.size()) {
                m_nCurrentIndex = 0;
            }
        }
        else if (!bStatus && bStatusHis) {
            bStatusHis = false;
            if (!m_vecStopScript.empty()) {
                std::cout << "开始执行休眠状态命令" << std::endl;
                for (const auto &strScript : m_vecRunScript) {
                    if (system(strScript.c_str())) {
                        std::cout << strScript << "\t执行成功" << std::endl;
                    }
                }
            }
        }
    }
}

bool Player::importBags(const char *ptrDir) {
    HDir dirRead(ptrDir);
    if (!dirRead.exist()) {
        std::cout << "导入数据的目录" << ptrDir << "不存在，请检查" << std::endl;
        return false;
    }
    std::vector<std::string> listDirs = dirRead.childDirAll();
    m_listBagDirMsg.clear();
    std::cout << "读到的listbag" << std::endl;
    for (const auto& dir : listDirs) {
        HString strDir(dir.c_str());
        BagDirMsg bagDir;
        if (strDir.isDir()) {
            std::vector<std::string> listBags = strDir.listFiles(".bag");
            if (!listBags.empty()) {
                bagDir.strDirName = dir;
                m_listBagDirMsg.push_back(bagDir);
            }
        }
    }

    std::cout << "读取到包含RtfBag文件的目录数量为" << m_listBagDirMsg.size() << std::endl;
    int nIndex = 1;
    for (const auto& bagDir : m_listBagDirMsg) {
        std::cout << "[" << nIndex << "]\t" << bagDir.strDirName << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
    doSort();
    return !m_listBagDirMsg.empty();
}


void Player::doSort()
{
    if (m_listBagDirMsg.empty()) {
        return;
    }
    for (auto &bagDir : m_listBagDirMsg) {
        HString strDir(bagDir.strDirName.c_str());
        if (strDir.isDir()) {
            std::vector<std::string> listBags = strDir.listFiles(".bag");
            if (!listBags.empty()) {
                std::cout << "读取文件的时间戳数据, 文件名是" << listBags[0].c_str() << std::endl;
                bagDir.nTimeStamp = getRtfTimeStamp(listBags[0].c_str());
            }
        }
    }
    std::cout << "排序前" << std::endl;
    int nIndex = 1;
    for (const auto & dir : m_listBagDirMsg) {
        std::cout << "[" << nIndex++ << "]\t" << dir.nTimeStamp << "\t" << dir.strDirName << std::endl;
    }

    std::sort(m_listBagDirMsg.begin(), m_listBagDirMsg.end(), [&](const BagDirMsg& obj1, const BagDirMsg& obj2) -> bool {
        return obj1.nTimeStamp < obj2.nTimeStamp;
    });
    std::cout << "排序后" << std::endl;
    nIndex = 1;
    for (const auto & dir : m_listBagDirMsg) {
        std::cout << "[" << nIndex++ << "]\t" << dir.nTimeStamp << "\t" << dir.strDirName << std::endl;
    }
}

long long Player::getRtfTimeStamp(const char *ptrBag)
{
    FILE *ptrFile = nullptr;
    char buffer[256] = {0};
    std::string strReadTimeScript("rtfbag info ");
    strReadTimeScript += ptrBag;
    ptrFile = popen(strReadTimeScript.c_str(), "r");
    bool bRet = false;
    if (ptrFile) {
        while (Player::s_bRunnable.load() && !feof(ptrFile)) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), ptrFile);
            std::string strMsg(buffer);
            if (std::string::npos != strMsg.find("start:")) {
                std::size_t nLeft = strMsg.find("(");
                std::size_t nRight = strMsg.find(")");
                if (nLeft != std::string::npos && nRight != std::string::npos && nLeft < nRight) {
                    std::string strTimeStamp = strMsg.substr(nLeft + 1, nRight - nLeft - 1);
                    return std::stod(strTimeStamp) * 1000;
                }
            }
        }
        pclose(ptrFile);
    }
    return 0;
}