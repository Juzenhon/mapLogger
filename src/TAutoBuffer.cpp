//
// Created by juzenhon on 2018/7/3.
//
#ifndef _WIN32
#include "TAutoBuffer.h"
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>


const TAutoBuffer KNullAtuoBuffer;

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

TAutoBuffer::TAutoBuffer(size_t _nSize)
    : parray_(NULL)
    , pos_(0)
    , length_(0)
    , capacity_(0)
    , malloc_unitsize_(_nSize)
{}


TAutoBuffer::TAutoBuffer(void* _pbuffer, size_t _len, size_t _nSize)
    : parray_(NULL)
    , pos_(0)
    , length_(0)
    , capacity_(0)
    , malloc_unitsize_(_nSize) {
    attach(_pbuffer, _len);
}

TAutoBuffer::TAutoBuffer(const void* _pbuffer, size_t _len, size_t _nSize)
    : parray_(NULL)
    , pos_(0)
    , length_(0)
    , capacity_(0)
    , malloc_unitsize_(_nSize) {
    write(0, _pbuffer, _len);
}


TAutoBuffer::~TAutoBuffer() {
    reset();
}

void TAutoBuffer::allocWrite(size_t _readytowrite, bool _changelength) {
    size_t nLen = pos() + _readytowrite;
    fitSize(nLen);

    if (_changelength) length_ = max(nLen, length_);
}

void TAutoBuffer::addCapacity(size_t _len) {
    fitSize(capacity() + _len);
}

void TAutoBuffer::write(const void *_pbuffer, size_t _len) {
    write(pos(), _pbuffer, _len);
    seek(_len, ESeekCur);
}

void TAutoBuffer::write(off_t &_pos, const void *_pbuffer, size_t _len) {
    write((const off_t &) _pos, _pbuffer, _len);
    _pos += _len;
}

void TAutoBuffer::write(const off_t &_pos, const void *_pbuffer, size_t _len) {
    size_t nLen = _pos + _len;
    fitSize(nLen);
    length_ = max(nLen, length_);
    memcpy((unsigned char*) ptr() + _pos, _pbuffer, _len);
}

void TAutoBuffer::write(TSeek _seek, const void *_pbuffer, size_t _len){
    off_t pos = 0;
    switch (_seek) {
        case ESeekStart:
            pos = 0;
            break;
        case ESeekCur:
            pos = pos_;
            break;
        case ESeekEnd:
            pos = length_;
            break;
        default:
            break;
    }

    write(pos, _pbuffer, _len);
}

size_t TAutoBuffer::read(void *_pbuffer, size_t _len) {
    size_t readlen = read(pos(), _pbuffer, _len);
    seek(readlen, ESeekCur);
    return readlen;
}

size_t TAutoBuffer::read(TAutoBuffer &_rhs, size_t _len) {
    size_t readlen = read(pos(), _rhs, _len);
    seek(readlen, ESeekCur);
    return readlen;
}

size_t TAutoBuffer::read(off_t &_pos, void *_pbuffer, size_t _len) const {
    size_t readlen = read((const off_t &) _pos, _pbuffer, _len);
    _pos += readlen;
    return readlen;
}

size_t TAutoBuffer::read(off_t &_pos, TAutoBuffer &_rhs, size_t _len) const {
    size_t readlen = read((const off_t &) _pos, _rhs, _len);
    _pos += readlen;
    return readlen;
}

size_t TAutoBuffer::read(const off_t &_pos, void *_pbuffer, size_t _len) const {

    size_t readlen = length() - _pos;
    readlen = min(readlen, _len);
    memcpy(_pbuffer, posPtr(), readlen);
    return readlen;
}

size_t TAutoBuffer::read(const off_t &_pos, TAutoBuffer &_rhs, size_t _len) const {
    size_t readlen = length() - _pos;
    readlen = min(readlen, _len);
    _rhs.write(posPtr(), readlen);
    return readlen;
}

off_t TAutoBuffer::move(off_t _move_len) {
    if (0 < _move_len) {
        fitSize(length() + _move_len);
        memmove(parray_ + _move_len, parray_, length());
        memset(parray_, 0, _move_len);
        length(pos() + _move_len, length() + _move_len);
    } else {
        size_t move_len = -_move_len;

        if (move_len > length()) move_len = length();

        memmove(parray_, parray_ + move_len, length() - move_len);
        length(move_len < (size_t) pos() ? pos() - move_len : 0, length() - move_len);
    }

    return length();
}

void  TAutoBuffer::seek(off_t _offset, TSeek _eorigin) {
    switch (_eorigin) {
    case ESeekStart:
        pos_ = _offset;
        break;

    case ESeekCur:
        pos_ += _offset;
        break;

    case ESeekEnd:
        pos_ = length_ + _offset;
        break;

    default:
        break;
    }

    if (pos_ < 0)
        pos_ = 0;

    if ((size_t)pos_ > length_)
        pos_ = length_;
}

void TAutoBuffer::length(off_t _pos, size_t _lenght) {
    length_ = _lenght;
    seek(_pos, ESeekStart);
}

void*  TAutoBuffer::ptr(off_t _offset) {
    return (char*)parray_ + _offset;
}

const void*  TAutoBuffer::ptr(off_t _offset) const {
    return (const char*)parray_ + _offset;
}

void* TAutoBuffer::posPtr() {
    return ((unsigned char*) ptr()) + pos();
}

const void* TAutoBuffer::posPtr() const {
    return ((unsigned char*) ptr()) + pos();
}

off_t TAutoBuffer::pos() const {
    return pos_;
}

size_t TAutoBuffer::posLength() const {
    return length_ - pos_;
}

size_t TAutoBuffer::length() const {
    return length_;
}

size_t TAutoBuffer::capacity() const {
    return capacity_;
}

void TAutoBuffer::attach(void *_pbuffer, size_t _len) {
    reset();
    parray_ = (unsigned char*)_pbuffer;
    length_ = _len;
    capacity_ = _len;
}

void TAutoBuffer::attach(TAutoBuffer &_rhs) {
    reset();
    parray_ = _rhs.parray_;
    pos_ = _rhs.pos_;
    length_ = _rhs.length_;
    capacity_ = _rhs.capacity_;

    _rhs.parray_ = NULL;
    _rhs.reset();
}

void* TAutoBuffer::detach(size_t *_plen) {
    unsigned char* ret = parray_;
    parray_ = NULL;
    size_t nLen = length();

    if (NULL != _plen)
        *_plen = nLen;

    reset();
    return ret;
}

void TAutoBuffer::reset() {
    if (NULL != parray_)
        free(parray_);

    parray_ = NULL;
    pos_ = 0;
    length_ = 0;
    capacity_ = 0;
}

void TAutoBuffer::fitSize(size_t _len) {
    if (_len > capacity_) {
        size_t mallocsize = ((_len + malloc_unitsize_ -1)/malloc_unitsize_)*malloc_unitsize_ ;

        void* p = realloc(parray_, mallocsize);

        if (NULL == p) {
//            ASSERT2(p, "_len=%" PRIu64 ", m_nMallocUnitSize=%" PRIu64 ", nMallocSize=%" PRIu64", m_nCapacity=%" PRIu64,
//                    (uint64_t)_len, (uint64_t)malloc_unitsize_, (uint64_t)mallocsize, (uint64_t)capacity_);
            free(parray_);
        }

        parray_ = (unsigned char*) p;

//        ASSERT2(_len <= 10 * 1024 * 1024, "%u", (uint32_t)_len);
//        ASSERT(parray_);
        
        memset(parray_+capacity_, 0, mallocsize-capacity_);
        capacity_ = mallocsize;
    }
}
#endif
