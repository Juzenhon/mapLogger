#ifndef _WIN32
#include "TPtrBuffer.h"

#include <string.h>


const TPtrBuffer KNullPtrBuffer(0, 0, 0);
//
// Created by juzenhon on 2018/7/3.
//
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

TPtrBuffer::TPtrBuffer(void* _ptr, size_t _len, size_t _maxlen)
    : parray_((unsigned char*)_ptr)
    , pos_(0)
    , length_(_len)
    , max_length_(_maxlen) {
}

TPtrBuffer::TPtrBuffer(void* _ptr, size_t _len)
    : parray_((unsigned char*)_ptr)
    , pos_(0)
    , length_(_len)
    , max_length_(_len) {
}

TPtrBuffer::TPtrBuffer() {
    reset();
}

TPtrBuffer::~TPtrBuffer() {
}

void TPtrBuffer::write(const void *_pBuffer, size_t _nLen) {
    write(_pBuffer, _nLen, pos());
    seek(_nLen, kSeekCur);
}

void TPtrBuffer::write(const void *_pBuffer, size_t _nLen, off_t _nPos) {
    size_t copylen = min(_nLen, max_length_ - _nPos);
    length_ = max(length_, copylen + _nPos);
    memcpy((unsigned char*) ptr() + _nPos, _pBuffer, copylen);
}

size_t TPtrBuffer::read(void *_pBuffer, size_t _nLen) {
    size_t nRead = read(_pBuffer, _nLen, pos());
    seek(nRead, kSeekCur);
    return nRead;
}

size_t TPtrBuffer::read(void *_pBuffer, size_t _nLen, off_t _nPos) const {

    size_t nRead = length() - _nPos;
    nRead = min(nRead, _nLen);
    memcpy(_pBuffer, posPtr(), nRead);
    return nRead;
}

void  TPtrBuffer::seek(off_t _nOffset, TSeek _eOrigin) {
    switch (_eOrigin) {
    case kSeekStart:
        pos_ = _nOffset;
        break;

    case kSeekCur:
        pos_ += _nOffset;
        break;

    case kSeekEnd:
        pos_ = length_ + _nOffset;
        break;

    default:
        break;
    }

    if (pos_ < 0)
        pos_ = 0;

    if ((unsigned int)pos_ > length_)
        pos_ = length_;
}

void TPtrBuffer::length(off_t _nPos, size_t _nLenght) {

    length_ = max_length_ < _nLenght ? max_length_ : _nLenght;
    seek(_nPos, kSeekStart);
}

void*  TPtrBuffer::ptr() {
    return parray_;
}

const void*  TPtrBuffer::ptr() const {
    return parray_;
}

void* TPtrBuffer::posPtr() {
    return ((unsigned char*) ptr()) + pos();
}

const void* TPtrBuffer::posPtr() const {
    return ((unsigned char*) ptr()) + pos();
}

off_t TPtrBuffer::pos() const {
    return pos_;
}

size_t TPtrBuffer::posLength() const {
    return length_ - pos_;
}

size_t TPtrBuffer::length() const {
    return length_;
}

size_t TPtrBuffer::maxLength() const {
    return max_length_;
}

void TPtrBuffer::attach(void *_pBuffer, size_t _nLen, size_t _maxlen) {
    reset();
    parray_ = (unsigned char*)_pBuffer;
    length_ = _nLen;
    max_length_ = _maxlen;
}

void TPtrBuffer::attach(void *_pBuffer, size_t _nLen) {
    attach(_pBuffer, _nLen, _nLen);
}

void TPtrBuffer::reset() {
    parray_ = NULL;
    pos_ = 0;
    length_ = 0;
    max_length_ = 0;
}

#endif
