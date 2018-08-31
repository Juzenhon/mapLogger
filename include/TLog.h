//
// Created by juzenhon on 2018/7/3.
//

#ifndef PROJB_TLOG_H
#define PROJB_TLOG_H
#ifndef _WIN32

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

//typedef void (*consolePrint)(const char* tag,const char *format, ...);

namespace tlog {

    typedef enum {
        Unkonwn = 0,
        Default,
        Verbose,
        Debug,    // Detailed information on the flow through the system.
        Info,     // Interesting runtime events (startup/shutdown), should be conservative and keep to a minimum.
        Warn,     // Other runtime situations that are undesirable or unexpected, but not necessarily "wrong".
        Error,    // Other runtime errors or unexpected conditions.
        Fatal,    // Severe errors that cause premature termination.
        Silent,     // Special level used to disable all log messages.
    } TLogLevel;

    typedef struct XLoggerInfo_t {
        TLogLevel level;
        const char* tag = NULL;

        struct timeval timev;
        int64_t tid;
        int64_t maintid;
    } XLoggerInfo;

    enum TLogMode {
        TLogAsync,
        TLogSync,
    };

    class TLog {

    public:

        static TLog* get();

        virtual ~TLog(){}

        virtual void logOpen(const char*logDir,const char* cacheDir,const char*prefix,TLogMode mode = TLogAsync) = 0;

        virtual void setLogMode(TLogMode mode) = 0;

        virtual void logPrint(XLoggerInfo &info,const char* log) = 0;

        virtual void logPrintV(XLoggerInfo &info,const char* format ,...) = 0;

        virtual void setConsoleOpen(bool console) = 0;

        virtual void logFlush() = 0;

        virtual void logClose() = 0;

        virtual void setLogLevel(TLogLevel level) = 0;

        virtual TLogLevel getLogLevel() = 0;

        virtual bool logEnabled(TLogLevel level) = 0;

    };

}
#endif
#endif //PROJB_TLOG_H
