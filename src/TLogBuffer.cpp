//
// Created by juzenhon on 2018/7/9.
//
#ifndef _WIN32
#include "TLogBuffer.h"

#include "TLogCrypt.h"

TLogBuffer::TLogBuffer(void *_pbuffer, size_t _len, bool _isCompress) : is_compress_(_isCompress) {
    _logBuff.attach(_pbuffer, _len);

    _pLogCrypt = new TLogCrypt();
//    if (is_compress_) {
//        memset(&cstream_, 0, sizeof(cstream_));
//    }
}

TLogBuffer::~TLogBuffer() {
//    if (is_compress_ && Z_NULL != cstream_.state) {
//        deflateEnd(&cstream_);
//    }
    delete _pLogCrypt;
}

TPtrBuffer &TLogBuffer::getData() {
    return _logBuff;
}


void TLogBuffer::flush(TAutoBuffer &_buff) {

    _logBufferMutex.lock();
    if (TLogCrypt::getLogLength((char *) _logBuff.ptr(), _logBuff.length()) != 0) {
        uint32_t valid_len = TLogCrypt::getValidLength((char*) _logBuff.ptr(), _logBuff.length());
        _buff.write(_logBuff.ptr(), valid_len);
    }
    clear();
    _logBufferMutex.unlock();
}

bool TLogBuffer::write(const void *_data, size_t _inputlen, TAutoBuffer &_out_buff) {
    if (NULL == _data || 0 == _inputlen) {
        return false;
    }

    if (is_compress_) {
        //TODO
//        compressLog(_data,_length,outBuffer);
    }
    _pLogCrypt->cryptSyncLog((char *) _data, _inputlen, _out_buff);

    return true;
}


bool TLogBuffer::write(const void *_data, size_t _length) {
    if (NULL == _data || 0 == _length) {
        return false;
    }

    if (is_compress_) {
        //TODO
//        compressLog(_data,_length,outBuffer);
    }

    TAutoBuffer out_buffer;
    _pLogCrypt->cryptAsyncLog((char *) _data, _length, out_buffer);

    _logBufferMutex.lock();
    if (_logBuff.length() == 0) {
        reset();
    }
    _logBuff.write(out_buffer.ptr(), out_buffer.length());
    _logBufferMutex.unlock();
    return true;
}

bool TLogBuffer::reset() {

    clear();

    if (is_compress_) {
//        cstream_.zalloc = Z_NULL;
//        cstream_.zfree = Z_NULL;
//        cstream_.opaque = Z_NULL;
//
//        if (Z_OK !=
//            deflateInit2(&cstream_, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
//                         Z_DEFAULT_STRATEGY)) {
//            return false;
//        }
//
    }

    return true;
}

void TLogBuffer::clear() {
    memset(_logBuff.ptr(), 0, _logBuff.maxLength());
    _logBuff.length(0, 0);
}

#endif
