#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <memory>
#include <vector>
#include "msgDef.h"

struct BagDirMsg {
    std::string strDirName;
    long long nTimeStamp;
};

class Player
{
public:
    explicit Player(const char* = "mdc-dbg sm query MachineState");
    ~Player();

public:
    bool startUp();
    bool readScript(const char*);
    bool importBags(const char*);
private:

    void waiting();
    bool isWarkUp();
    void listening();
    void doSort();
    long long getRtfTimeStamp(const char*);

public:
    static std::atomic_bool         s_bRunnable;
private:
    int                             m_nCurrentIndex;
    std::string                     m_strQuery;
    std::shared_ptr<std::thread>    m_ptrThreadReq;
    std::vector<std::string>        m_vecRunScript;
    std::vector<std::string>        m_vecStopScript;
    std::vector<BagDirMsg>          m_listBagDirMsg;
    std::vector<std::string>        m_listSkipEvent;
};
