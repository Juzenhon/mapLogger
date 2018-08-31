#ifndef _WIN32
#include "TLogCache.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>

#include <unistd.h>


TLogCache::TLogCache(bool bOwner)
        : _bOwner(bOwner), _pAddr(NULL), _iLength(0), _bCreate(false) {
}

TLogCache::~TLogCache() {
    if (_bOwner) {
        munmap();
    }
}

void TLogCache::mmap(size_t length, int prot, int flags, int fd, off_t offset) {
    if (_bOwner) {
        munmap();
    }
    _pAddr = ::mmap(NULL, length, prot, flags, fd, offset);
    if (_pAddr == (void *) -1) {
        _pAddr = NULL;
        return;
    }
    _iLength = length;
    _bCreate = false;
}

void TLogCache::mmap(const char *file, size_t length) {
    if(length <= 0){
        return;
    }

    if (_bOwner) {
        munmap();
    }
#ifdef _WIN32
    HANDLE dumpFileDescriptor = CreateFileA(file,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);

  HANDLE fileMappingObject = CreateFileMapping(dumpFileDescriptor,
                        NULL,PAGE_READWRITE, 0,0,NULL);
  _pAddr = MapViewOfFile(fileMappingObject,
                       FILE_MAP_ALL_ACCESS, 0, 0,length);
#else
    //注意_bCreate的赋值位置:保证多线程用一个对象的时候也不会有问题
    int fd = open(file, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd == -1) {
        if (errno != EEXIST) {
            return;
        } else {
            fd = open(file, O_CREAT | O_RDWR, 0666);
            if (fd == -1) {
                return;
            }
            _bCreate = false;
        }
    } else {
        _bCreate = true;
    }

    lseek(fd, length - 1, SEEK_SET);
    write(fd, "\0", 1);

    _pAddr = ::mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (_pAddr == (void *) -1) {
        _pAddr = NULL;
        close(fd);
        return;
    }
    _iLength = length;
    if (fd != -1) {
        close(fd);
    }
#endif
}

void TLogCache::munmap() {
    if (_pAddr == NULL) {
        return;
    }

#ifdef _WIN32
    UnmapViewOfFile(_pAddr);
#else
    int ret = ::munmap(_pAddr, _iLength);
    if (ret == -1) {
        return;
    }

    _pAddr = NULL;
    _iLength = 0;
    _bCreate = false;
#endif
}

void TLogCache::msync(bool bSync) {
    int ret = 0;
#ifdef _WIN32
    FlushViewOfFile(mappedFileAddress, _iLength);
#else
    if (bSync) {
        ret = ::msync(_pAddr, _iLength, MS_SYNC | MS_INVALIDATE);
    } else {
        ret = ::msync(_pAddr, _iLength, MS_ASYNC | MS_INVALIDATE);
    }
    if (ret != 0) {
        return;
    }
#endif
}

#endif

