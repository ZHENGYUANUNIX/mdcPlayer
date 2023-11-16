#pragma once
#include <string>
#include <vector>

class HString {
public:
    explicit HString(const char*);
    ~HString();

public:
    bool isDir();
    bool isFile();
    bool startWith(const char*);
    const char* name();
    std::string realName();
    std::string simplified();
    void removeStart(const char*);
    std::vector<std::string> listFiles(const char*);

private:
    bool exist();

private:
    std::string m_string;
};

class HFile {
public:
    explicit HFile(const char*);
    ~HFile();

public:
    bool endWith(const char*);
    bool contains(const char*);
    bool exist();
    bool remove();
    bool isFile();
    const char* name();

private:
    const std::string           m_strFile;
};


class HDir {
public:
    explicit HDir(const char*);
    ~HDir();

public:
    bool isDir();
    bool isDeepest();
    bool exist();
    bool remove();
    const char* name();
    std::vector<std::string> & childDirAll();

private:
    void childDir(const std::string&, std::vector<std::string>&);

private:
    const std::string           m_strDir;
    std::vector<std::string>    m_vecDir;
};
