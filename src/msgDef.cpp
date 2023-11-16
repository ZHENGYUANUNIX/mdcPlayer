#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <fstream>
#include "msgDef.h"
#include <cstdio>
#include <algorithm>

HFile::HFile(const char *ptrName) : m_strFile(ptrName)
{
}

HFile::~HFile() = default;

bool HFile::endWith(const char *ptrSuffix)
{
    std::string strSuffix(ptrSuffix);
    if (strSuffix.size() > m_strFile.size()) {
        return false;
    }
    return m_strFile.find(strSuffix, m_strFile.size() - strSuffix.size()) != std::string::npos;
}

bool HFile::contains(const char *ptrName)
{
    if (m_strFile.empty()) {
        return false;
    }
    std::size_t nPos = m_strFile.find_last_not_of('/');
    if (std::string::npos != nPos) {
        if (m_strFile.length() > nPos + 1) {
            return std::string::npos != m_strFile.substr(nPos + 1).find(ptrName);
        }
    }
    return false;
}

bool HFile::exist()
{
    std::ifstream ifStream(m_strFile.c_str());
    return static_cast<bool>(ifStream);
}

bool HFile::remove()
{
    return exist() && std::remove(m_strFile.c_str());
}

bool HFile::isFile()
{
    if (exist()) {
        return false;
    }
    struct stat buffer;
    return (stat(m_strFile.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

const char *HFile::name()
{
    return m_strFile.c_str();
}


HDir::HDir(const char *ptrDir) : m_strDir(ptrDir)
{
}

HDir::~HDir() = default;

bool HDir::isDir()
{
    struct stat buffer;
    return (stat(m_strDir.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool HDir::isDeepest()
{
    if (m_strDir.empty()) {
        return false;
    }
    DIR *ptrDir = nullptr;
    struct dirent *ptrEntry = nullptr;
    struct stat info;

    if ((ptrDir = opendir(m_strDir.c_str())) == nullptr) {
        return false;
    }

    while ((ptrEntry = readdir(ptrDir)) != nullptr) {
        std::string strFullPath = m_strDir + "/" + ptrEntry->d_name;
        if (ptrEntry->d_name[0] == '.') {
            continue;
        }

        if (stat(strFullPath.c_str(), &info) != 0) {
            continue;
        }

        struct stat buffer;
        if (stat(strFullPath.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode)) {
            closedir(ptrDir);
            return false;
        }
    }
    closedir(ptrDir);
    return false;
}

bool HDir::exist()
{
    struct stat buffer;
    if (stat(m_strDir.c_str(), &buffer) != 0) {
        return false;
    }
    return (buffer.st_mode & S_IFDIR) != 0;
}

bool HDir::remove()
{
    return exist() && std::move(m_strDir.c_str());
}

const char *HDir::name()
{
    return m_strDir.c_str();
}

std::vector<std::string> &HDir::childDirAll()
{
    m_vecDir.clear();
    childDir(m_strDir, m_vecDir);
    return m_vecDir;
}

void HDir::childDir(const std::string &strPath, std::vector<std::string> &listDir)
{
    DIR *ptrDir = nullptr;
    struct dirent *ptrEntry = nullptr;
    struct stat info;

    if ((ptrDir = opendir(strPath.c_str())) == nullptr) {
        return;
    }

    while ((ptrEntry = readdir(ptrDir)) != nullptr) {
        if (ptrEntry->d_name[0] == '.') {
            continue;
        }
        std::string strName(strPath + "/" + ptrEntry->d_name);
        if (stat(strName.c_str(), &info) != 0) {
            continue;
        }
        HString strFullPath(strName.c_str());
        if (strFullPath.isDir()) {
            listDir.push_back(strName);
            childDir(strName, listDir);
        }
        else if (strFullPath.isFile()) {
            continue;
        }
    }
    closedir(ptrDir);
}


HString::HString(const char *ptrString) : m_string(ptrString)
{
}

HString::~HString() = default;

bool HString::isDir()
{
    struct stat buffer;
    return (stat(m_string.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool HString::isFile()
{
    if (exist()) {
        return false;
    }
    struct stat buffer;
    return (stat(m_string.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

const char *HString::name()
{
    return m_string.c_str();
}

bool HString::exist()
{
    std::ifstream ifStream(m_string.c_str());
    return static_cast<bool>(ifStream);
}

bool HString::startWith(const char *ptrStart)
{
    std::string strStart(ptrStart);
    if (m_string.size() < strStart.size()) {
        return false;
    }
    return m_string.substr(0, strStart.size()) == strStart;
}

std::string HString::simplified()
{
    std::string string = m_string;
    string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    return string;
}

void HString::removeStart(const char *ptrStart)
{
    if (startWith(ptrStart)) {
        std::string strStart(ptrStart);
        if (m_string.size() < strStart.size()) {
            return;
        }
        m_string = m_string.substr(strStart.size());
    }
}

std::vector<std::string> HString::listFiles(const char* ptrFileSuffix)
{
    std::string strFileSuffix(ptrFileSuffix);
    std::vector<std::string> listBag;
    DIR* ptrDir = opendir(m_string.c_str());
    struct dirent * ptrEnt;
    while ((ptrEnt = readdir(ptrDir)) != nullptr) {
        HFile strFile(ptrEnt->d_name);
        if (strFile.endWith(ptrFileSuffix)) {
            std::string strNameShort = strFile.name();
            if (strNameShort.size() >= strFileSuffix.size() && strNameShort.substr(strNameShort.size() - strFileSuffix.size()) == strFileSuffix) {
                std::string strFullName = m_string + "/" + strNameShort;
                HFile fileRtf(strFullName.c_str());
                if (fileRtf.exist()) {
                    std::ifstream streamRtf(strFullName.c_str());
                    if (streamRtf) {
                        std::string strHeader;
                        if (std::getline(streamRtf, strHeader)) {
                            HFile fileRtfOK(strHeader.c_str());
                            if (std::string::npos != strHeader.find("#RTFBAG")) {
                                listBag.push_back(m_string + "/" + strNameShort);
                            }
                        }
                        streamRtf.close();
                    }
                }
            }
        }
    }

    closedir(ptrDir);
    return listBag;
}

std::string HString::realName()
{
    std::size_t nPos = m_string.find_last_of("/");
    if (std::string::npos != nPos && nPos < m_string.size()) {
        return m_string.substr(nPos + 1);
    }
    return m_string;
}