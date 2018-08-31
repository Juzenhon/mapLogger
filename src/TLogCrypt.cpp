#ifndef _WIN32
#include "TLogCrypt.h"

#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static const char tMagicSyncStart = '\x06';
static const char tMagicSyncNoCryptStart ='\x08';
static const char tMagicAsyncStart ='\x07';
static const char tMagicAsyncNoCryptStart ='\x09';

static const char tMagicEnd  = '\0';

const static uint16_t MAX_LOG_LENGTH = UINT16_MAX;


TLogCrypt::TLogCrypt():is_crypt_(false) {
}


/*
 * |magic start(char)|seq(uint16_t)|length(uint16_t)|data|end(char)|
 */

uint32_t TLogCrypt::getHeaderLen() {
    return sizeof(char) + sizeof(uint16_t) + sizeof(uint16_t);
}

uint32_t TLogCrypt::getTailerLen() {
    return sizeof(tMagicEnd);
}


uint16_t TLogCrypt::getLogLength(const char *const _data, int32_t _len) {
    if (_len < getHeaderLen()){
        return 0;
    }
    
    char start = _data[0];
    if (tMagicAsyncStart != start && tMagicSyncStart != start
        && tMagicAsyncNoCryptStart != start && tMagicSyncNoCryptStart != start) {
        return 0;
    }
    
    uint16_t len = 0;
    memcpy(&len, _data + sizeof(char) + sizeof(uint16_t), sizeof(len));
    return len;
}

uint32_t TLogCrypt::getValidLength(const char *const _data,int32_t _len){
    if (_len < getHeaderLen()){
        return 0;
    }

    uint32_t ret_len = 0;

    while(ret_len < _len){
        char start = _data[ret_len];
        if (tMagicAsyncStart != start && tMagicSyncStart != start
            && tMagicAsyncNoCryptStart != start && tMagicSyncNoCryptStart != start) {
            break;
        }
        uint16_t len = 0;
        memcpy(&len, _data + ret_len + sizeof(char) + sizeof(uint16_t), sizeof(len));

        size_t tem_len = 0;
        tem_len += getHeaderLen();
        tem_len += len;
        tem_len += getTailerLen();
        if(ret_len + tem_len <= _len){
            ret_len += tem_len;
        } else{
            break;
        }

    }

    return ret_len;

}

uint16_t TLogCrypt::getSeq(bool _sync) {

    if (!_sync) {
        return 0;
    }

    static uint16_t s_seq = 0;

    s_seq ++;

    if (0 == s_seq) {
        ++s_seq;
    }

    return s_seq;
}

void TLogCrypt::cryptSyncLog(const char *const _log_data, int32_t _input_len,
                             TAutoBuffer &_out_buff) {

    if(_input_len < 1){
        return;
    }

    if (is_crypt_) {
//        memcpy(_out_buff.Ptr(), _log_data, _input_len);
//        crypt_data
    }


    _out_buff.allocWrite(getHeaderLen() + _input_len + getTailerLen());
    int32_t log_len = _input_len;
    int32_t start_pos = 0;

    do {
        uint16_t temp_len = log_len > MAX_LOG_LENGTH ? MAX_LOG_LENGTH : (uint16_t)log_len;

        _out_buff.write(is_crypt_ ? &tMagicSyncStart : &tMagicSyncNoCryptStart, sizeof(char));

        uint16_t seq = getSeq(true);
        _out_buff.write(&seq, sizeof(seq));

        _out_buff.write(&temp_len, sizeof(temp_len));

        _out_buff.write(_log_data + start_pos, temp_len);

        _out_buff.write(&tMagicEnd, sizeof(tMagicEnd));

        log_len -= temp_len;
        start_pos += temp_len;

    }while (log_len > 0);

}

void TLogCrypt::cryptAsyncLog(const char *const _log_data, int32_t _input_len,
                              TAutoBuffer &_out_buff) {
    if(_input_len < 1){
        return;
    }

    if (is_crypt_) {
//        memcpy(_out_buff.Ptr(), _log_data, _input_len);
//        crypt_data
    }


    _out_buff.allocWrite(getHeaderLen() + _input_len + getTailerLen());

    int32_t log_len = _input_len;
    int32_t start_pos = 0;

    do {
        uint16_t temp_len = log_len > MAX_LOG_LENGTH ? MAX_LOG_LENGTH : (uint16_t)log_len;

        _out_buff.write(&tMagicAsyncNoCryptStart, sizeof(char));

        uint16_t seq = getSeq(false);
        _out_buff.write(&seq, sizeof(uint16_t));

        _out_buff.write(&temp_len, sizeof(uint16_t));

        _out_buff.write(_log_data + start_pos, temp_len);

        _out_buff.write(&tMagicEnd, sizeof(char));

        log_len -= temp_len;
        start_pos += temp_len;

    }while (log_len > 0);

}

#endif