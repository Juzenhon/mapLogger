//
//  CLog.cpp
//  TNMP
//
//  Created by 兰怀玉 on 16/5/6.
//  Copyright © 2016年 兰怀玉. All rights reserved.
//

#include "CLog.h"
#include <iostream>
#include <ITNMPResponse.h>
#include <chrono>
#include <ctime>
#include "CTNUtils.h"
using namespace std::chrono;

#ifdef _WIN32
#include <Windows.h>
#elif ANDROID

#include <unistd.h>
#include <jni.h>

#undef printf

#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_DEBUG,"native-tnmp",__VA_ARGS__)

#else
#include <sys/types.h>
#endif

//#define LOG_FILENAME(path, tm) (path+"/imlog"+tm+".txt")
const int CLog::MAX_FILE_LOG_SIZE = 1024 * 1024 * 50;


CLog *CLog::instance() {
    static CLog sClog;
    return &sClog;
}

CLog::CLog() :_file(NULL){
}

CLog::~CLog() {
}

bool CLog::open() {

    time_t now_time = system_clock::to_time_t(system_clock::now());
    tm now_tm = *localtime(&now_time);
    if (_file != NULL) {
        if(_lastFileTime > 0){
            tm last_tm = *localtime(&_lastFileTime);
            if (last_tm.tm_year == now_tm.tm_year && last_tm.tm_mon == now_tm.tm_mon &&
                last_tm.tm_mday == now_tm.tm_mday) {
                return true;
            }
        }

        fclose(_file);
        _file = nullptr;
    }

    string sFileName = this->getFileName(now_time);// LOG_FILENAME(_appPath);

    _file = fopen(sFileName.c_str(), "a");

    if (nullptr == _file) {
        std::cout << "can't open the file:" << sFileName << std::endl;
        return false;
    }
    _lastFileTime = now_time;
    return true;
}


void CLog::closeClog() {
    stop();
    _logQueue.weakup();
    join();
}

void CLog::setPath(string path) {
    if (path.length() == 0) {
        return;
    }

    std::unique_lock<std::mutex> unilock(_appPathMtx);
    _appPath = path;

    if (!isRunning()) {
        start();
    }
}

void CLog::clearOldFile() {

    system_clock::time_point now = system_clock::now();
    for (int i = 0; i < 10; i++) {
        std::time_t tt = system_clock::to_time_t(now - std::chrono::hours(24 * (i + 7)));
        string sFileName = getFileName(tt);
        if (remove(sFileName.c_str()) == 0) {
            TNMPLOG("Succeded Remove log file:" << sFileName);
        } else {
            TNMPLOG("Fail to Remove log file:" << sFileName);
        }
    }
}

string CLog::getFileName(time_t timestamp) {
    if (timestamp == 0) {
        timestamp = system_clock::to_time_t(system_clock::now());
    }

    tm tLocalTime = *localtime(&timestamp);
    //"2011-07-18 23:03:01 ";
    char strDateTime[64] = {0};
    strftime(strDateTime, 30, "%Y-%m-%d", &tLocalTime);
//    std::string strDate(strDateTime);
    std::unique_lock<std::mutex> unilock(_appPathMtx);
    return (_appPath + "/imlog" + strDateTime + ".txt");
}


unsigned long CLog::getCurrentThreadID() {
#ifdef _WIN32
    return (unsigned long )GetCurrentThreadId();
#elif __APPLE__
    return (unsigned long )pthread_mach_thread_np(pthread_self());
//#elif defined(ANDROID)
//    return gettid();
#else
    return (unsigned long) pthread_self();
//    return (unsigned long )pthread_mach_thread_np(pthread_self());
#endif
}


void CLog::writeLog(stringstream &logstream) {
#ifndef _WIN32
	printf("%s\n", logstream.str().c_str());
#else
	printf("%s\r\n", logstream.str().c_str());
#endif
    //    logstream<<" thread:"<<getCurrentThreadID();
    string log = logstream.str();
    _logQueue.push(log);
}

//得到当前时间的字符串
string CLog::getTimeStr() {
    static std::chrono::steady_clock::time_point startup = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();

    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - startup);
    auto tt = std::chrono::system_clock::to_time_t
            (std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[128] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d.%lld  ",
            (int) ptm->tm_year + 1900, (int) ptm->tm_mon + 1, (int) ptm->tm_mday,
            (int) ptm->tm_hour, (int) ptm->tm_min, (int) ptm->tm_sec, d.count());
    return std::string(date);
}


void CLog::run() {
    printf("CLog::run()");
//    this->open();

    this->clearOldFile();

    int ret = 0;
    while (this->isRunning()) {
        string logStr;
        ret = _logQueue.pop(logStr, 60);
        if (ret != SFQUEUE_OK || logStr.length() == 0) {
            continue;
        }

        if (!this->open()) {
            continue;
        }

        fwrite(logStr.c_str(), logStr.length(), sizeof(char), _file);
        fwrite("\r\n", 1, 2, _file);
        fflush(_file);

        if(ftell(_file) > MAX_FILE_LOG_SIZE){
            fclose(_file);
            _file = nullptr;

            string logfileName = getFileName(_lastFileTime);
            string tmpFileName = logfileName + ".tmp";
            toonim::CTNPathUtils::removeFile(tmpFileName);
            toonim::CTNPathUtils::renameFile(logfileName,tmpFileName);
        }
    }

    if (_file != nullptr) {
        fclose(_file);
        _file = nullptr;
    }
}

