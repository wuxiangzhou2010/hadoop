/*************************************************************************
 * Author: Wan Xinshuo
 * Created Time: 2010年09月10日 星期五 15时51分47秒
 * File Name: ThreadPool.h
 * Description:
 ************************************************************************/
#include <pthread.h>
typedef struct TagCThreadWorker {
    void * (*Process)(void * Arg);
    void *                    Arg;
    struct TagCThreadWorker * m_Next;
} CThreadWorker;
typedef struct TagCThreadPool {
    int             m_Shutdown;
    pthread_mutex_t m_QueueLock;
    pthread_cond_t  m_QueueReady;
    pthread_t *     m_pThreadID;

    int             m_MaxThreadNum;
    int             m_CurQueueSize;
    CThreadWorker * m_pQueueHead;
} CThreadPool;
class CThreadPoolManager
{
public:
    CThreadPoolManager();
    ~CThreadPoolManager();
public:
    static void * ThreadPoolRoutine(void * Arg);
public:
    void ThreadPoolManagerInit(int MaxThreadNum);
    void ThreadPoolManagerAddWorker(void *(*Process)(void * Arg), void * Arg);
    int  ThreadPoolManagerDestroy();
private:
    static CThreadPool m_ThreadPool;
};
