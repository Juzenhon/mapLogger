#ifndef _WIN32
#ifndef LOG_CRYPT_H_
#define LOG_CRYPT_H_

#include <stdint.h>
#include <string>

#include "TAutoBuffer.h"


class TLogCrypt {
public:
    TLogCrypt();
    virtual ~TLogCrypt() {}
    
private:
    TLogCrypt(const TLogCrypt&);
    TLogCrypt& operator=(const TLogCrypt&);

private:
    static uint32_t getHeaderLen();

    static uint32_t getTailerLen();

    static uint16_t getSeq(bool sync);

public:

    static uint16_t getLogLength(const char *const _data, int32_t _len);

    static uint32_t getValidLength(const char *const _data,int32_t _len);

    void cryptSyncLog(const char *const _log_data, int32_t _input_len, TAutoBuffer &_out_buff);
    void cryptAsyncLog(const char *const _log_data, int32_t _input_len, TAutoBuffer &_out_buff);
    
private:
    bool is_crypt_;

};



#endif /* LOG_CRYPT_H_ */
#endif
