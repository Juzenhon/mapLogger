//
// Created by juzenhon on 2018/7/3.
//
#ifndef _WIN32
#ifndef COMM_PTRBUFFER_H_
#define COMM_PTRBUFFER_H_

#include <sys/types.h>
#include <string.h>

class TPtrBuffer {
  public:
    enum TSeek {
        kSeekStart,
        kSeekCur,
        kSeekEnd,
    };
  public:
    TPtrBuffer(void* _ptr, size_t _len, size_t _maxlen);
    TPtrBuffer(void* _ptr, size_t _len);
    TPtrBuffer();
    ~TPtrBuffer();

    template<class T> void write(const T &_val)
    { write(&_val, sizeof(_val)); }

    template<class T> void write(int _nPos, const T &_val)
    { write(&_val, sizeof(_val), _nPos);}

    void write(const char *const _val)
    { write(_val, (unsigned int) strlen(_val));}

    void write(int _nPos, const char *const _val)
    { write(_val, (unsigned int) strlen(_val), _nPos);}

    void write(const void *_pBuffer, size_t _nLen);
    void write(const void *_pBuffer, size_t _nLen, off_t _nPos);

    template<class T> void read(T &_val)
    { read(&_val, sizeof(_val)); }

    template<class T> void read(int _nPos, const T &_val) const
    { read(&_val, sizeof(_val), _nPos); }

    size_t read(void *_pBuffer, size_t _nLen);
    size_t read(void *_pBuffer, size_t _nLen, off_t _nPos) const;

    void seek(off_t _nOffset, TSeek _eOrigin = kSeekCur);
    void length(off_t _nPos, size_t _nLenght);

    void* ptr();
    void* posPtr();
    const void* ptr() const;
    const void* posPtr() const;

    off_t pos() const;
    size_t posLength() const;
    size_t length() const;
    size_t maxLength() const;

    void attach(void *_pBuffer, size_t _nLen, size_t _maxlen);
    void attach(void *_pBuffer, size_t _nLen);
    void reset();

  private:
    TPtrBuffer(const TPtrBuffer& _rhs);
    TPtrBuffer& operator = (const TPtrBuffer& _rhs);

  private:
    unsigned char* parray_;
    off_t pos_;
    size_t length_;
    size_t max_length_;
};

extern const TPtrBuffer KNullPtrBuffer;

#endif
#endif	// COMM_PTRBUFFER_H_
