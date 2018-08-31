//
// Created by juzenhon on 2018/7/3.
//
#ifndef _WIN32
#include "TILog.h"
#include <string>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "TLogBuffer.h"
#include "TLogCache.h"


namespace tlog {

    class TLogImpl : public TLog {

    public:
        TLogImpl() : _logClosed(false), _consoleOpen(false), _asyncThread(NULL), _level(Verbose),_logBufferMemory(NULL) {
        }

        virtual ~TLogImpl();

        virtual void
        logOpen(const char *logDir, const char *cacheDir, const char *prefix, TLogMode mode);

        virtual void setLogMode(TLogMode mode);

        virtual void logPrint(XLoggerInfo &info, const char *log);

        virtual void logPrintV(XLoggerInfo &info, const char *format, ...);

        virtual void setConsoleOpen(bool console);

        virtual void logFlush();

        virtual void logClose();

        virtual void setLogLevel(TLogLevel level);

        virtual TLogLevel getLogLevel();

        virtual bool logEnabled(TLogLevel level);

    protected:
        void asyncRun();

    private:
        /**
         *
         * @param _tips_format
         * @param ...
         */
        void writeTips2File(const char *_tips_format, ...);

        void deleteTimeoutFiles();

        bool openShareCache();

        void log2file(const void *_data, size_t _len);

        bool openLogFile();

        void closeLogFile();

        bool write2file(const void *_data, size_t _len,FILE* _file);

        void makeLogFileName(struct tm &_tm, char *filepath);

        void logFormat(const XLoggerInfo &_info, const char *_logbody, TPtrBuffer &_log);

    public:
        static const unsigned int LOG_MAP_LENGTH;
        static const unsigned int LOG_MAX_FILE_LENGTH;

        static const char *LOG_FILE_SUFFIX;
    private:

        std::string _logDir;

        std::string _cacheDir;

        std::string _prefix;

        FILE *_currentLogFile;

        time_t _openLogFileTime;

        TLogMode _mode;

        TLogLevel _level;

        bool _consoleOpen;

        bool _logClosed = false;

        bool _tLogOpened = false;

        TLogCache _mmapFile;

        TLogBuffer *_logBuffer = NULL;
        char *_logBufferMemory = NULL;

        std::condition_variable _conditionAsync;
        std::mutex _mutexAsync;

        std::mutex _logFileMutex;

        std::thread *_asyncThread;

    };

    const unsigned int TLogImpl::LOG_MAP_LENGTH = 150 * 1024;
    const unsigned int TLogImpl::LOG_MAX_FILE_LENGTH = 1024 * 1024 * 100;
    const char *TLogImpl::LOG_FILE_SUFFIX = ".tlog";

    TLog *TLog::get() {
        static TLogImpl impl;
        return &impl;
    }

}
#endif

