#ifndef __TC_MMAP_H
#define __TC_MMAP_H
#ifndef _WIN32
#include <sys/mman.h>
#include <string>
using namespace std;


class TLogCache
{
public:

    /**
     * @brief 构造函数. 
     *  
     * @param bOwner, 如果拥有, 则析够的时候unmap
     */
    TLogCache(bool bOwner = true);

    /**
     * @brief 析够
     */
    ~TLogCache();

    /**
     * @brief 映射到进程空间. 
     *  
     * @param length, 映射到进程地址空间的字节数,从offset个字节算起
     * @param prot,指定共享内存的访问权限,可取值为(几个值的或):
     *              PROT_READ: 可读
     *  
     *              PROT_WRITE: 可写
     *  
     *              PROT_EXEC : 可执行
     *  
     *              PROT_NONE: 不可访问
     * @param flags, MAP_SHARED,MAP_PRIVATE,MAP_FIXED:一般取:MAP_SHARED
     * @param fd, 进程空间中的文件描述符
     * @param offset,文件的偏移地址, 一般为0
     * @throws TC_Mmap_Exception
     * @return
     */
    void mmap(size_t length, int prot, int flags, int fd, off_t offset = 0);

    /**
     * @brief 映射到进程空间，采用: 
     *  
     * PROT_READ|PROT_WRITE,MAP_SHARED方式 
     *  
     * 注意的文件大小会比length大一个字节(初始化时生成空洞文件的原因) 
     * @param file    文件名
     * @param length  映射文件的长度
     * @throws        TC_Mmap_Exception
     * @return
     */
    void mmap(const char *file, size_t length);

    /**
     * @brief 解除映射关系, 解除后不能在访问这段空间了. 
     *  
     * @throws TC_Mmap_Exception
     * @return
     */
    void munmap();

    /**
     * @brief 把共享内存中的改变写回磁盘中.
     *
     * @param bSync true:同步写回, false:异步写回
     * @throws      TC_Mmap_Exception
     * @return
     */
    void msync(bool bSync = false);

    /**
     * @brief 获取映射的指针地址.
     *
     * @return void* 映射的指针地址
     */
    void *getPointer() const    { return _pAddr; }

    /**
     * @brief 获取映射的空间大小.
     *
     * @return size_t 映射的空间大小
     */
    size_t getSize() const      { return _iLength; }

    /**
     * @brief 是否创建出来的，文件的存在可分为两种形式： 
     *  
     *        一种是创建的，一种是开始就存在的
     * @return 如果是创建出来的就返回true，否则返回false
     */
    bool iscreate() const       { return _bCreate; }

    /**
     * @brief 设置是否拥有. 
     *  
     * @param bOwner ture or fale
     */
    void setOwner(bool bOwner)  { _bOwner = bOwner; }

protected:

    /**
     * 是否拥有
     */
    bool    _bOwner;

    /**
     * 映射到进程空间的地址
     */
    void    *_pAddr;

    /**
     * 映射的空间大小
     */
    size_t  _iLength;

    /**
     * 是否创建出来的
     */
    bool    _bCreate;
};

#endif
#endif

