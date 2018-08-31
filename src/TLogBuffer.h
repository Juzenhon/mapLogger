//
// Created by juzenhon on 2018/7/9.
//

#ifndef _WIN32
//#include <zlib.h>
#include <string>
#include <stdint.h>
#include <mutex>

#include "TPtrBuffer.h"
#include "TAutoBuffer.h"


#ifndef PROJB_TLOGBUFFER_H
#define PROJB_TLOGBUFFER_H


class TLogCrypt;

class TLogBuffer {

public:
    TLogBuffer(void *_pbuffer, size_t _len, bool _is_compress = false);

    ~TLogBuffer();

public:
    TPtrBuffer &getData();

    void flush(TAutoBuffer &_buff);

    bool write(const void *_data, size_t _inputlen, TAutoBuffer &_out_buff);

    bool write(const void *_data, size_t _length);

private:

    bool reset();

    void clear();

private:
    std::mutex _logBufferMutex;
    TPtrBuffer _logBuff;
//    z_stream _cstream;
    TLogCrypt * _pLogCrypt;

    bool is_compress_;

};

#endif
#endif //PROJB_TLOGBUFFER_H
