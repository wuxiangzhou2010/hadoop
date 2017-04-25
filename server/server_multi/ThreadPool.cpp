/*************************************************************************
 * Author: Wan XinShuo
 * Created Time: 2010年09月14日 星期二 11时58分17秒
 * File Name: ThreadPool.cpp
 * Description:
 ************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include "ThreadPool.h"
#include <iostream>
using namespace std;
CThreadPool CThreadPoolManager::m_ThreadPool;
CThreadPoolManager::CThreadPoolManager()
{
    cout << "CThreadPoolManager::CThreadPoolManager()" << endl;
}

CThreadPoolManager::~CThreadPoolManager()
{
    cout << "CThreadPoolManager::~CThreadPoolManager()" << endl;
}

void * CThreadPoolManager::ThreadPoolRoutine(void * Arg)
{
    cout << "Start thread 0x" << hex << pthread_self() << endl;
    while (1) {
        pthread_mutex_lock(&m_ThreadPool.m_QueueLock);
        // 这里用m_Shutdown变量告诉正在工作的消费者线程主程序即将退出，请不要再去等待，直接退出
        while (m_ThreadPool.m_CurQueueSize == 0 && !m_ThreadPool.m_Shutdown) {
            cout << "Thread 0x" << hex << pthread_self() << " is waiting ." << endl;
            pthread_cond_wait(&m_ThreadPool.m_QueueReady, &m_ThreadPool.m_QueueLock);
        }
        // 正在工作的消费者线程从这里退出
        if (m_ThreadPool.m_Shutdown) {
            cout << "Thread 0x" << hex << pthread_self() << " will exit" << endl;
            pthread_mutex_unlock(&m_ThreadPool.m_QueueLock);
            pthread_exit(NULL);
        }

        // cout<<"Thread 0x"<<hex<<pthread_self()<<" is starting to work"<<endl;
        assert(m_ThreadPool.m_CurQueueSize != 0);
        assert(m_ThreadPool.m_pQueueHead != NULL);
        // 这里加个条件判断
        if (m_ThreadPool.m_CurQueueSize == 0 && m_ThreadPool.m_pQueueHead == NULL)
            continue;
        CThreadWorker * pTempWorker = m_ThreadPool.m_pQueueHead;
        m_ThreadPool.m_pQueueHead = pTempWorker->m_Next;
        m_ThreadPool.m_CurQueueSize--;
        pthread_mutex_unlock(&m_ThreadPool.m_QueueLock);
        // 不能将业务逻辑代码放在锁内执行，否则会大大降低效率
        (*(pTempWorker->Process))(pTempWorker->Arg);
        delete pTempWorker;
        pTempWorker = NULL;
    }
    // 不会执行到这
    pthread_exit(NULL);
    return ((void *) 0);
} // CThreadPoolManager::ThreadPoolRoutine

void CThreadPoolManager::ThreadPoolManagerInit(int MaxThreadNum)
{
    m_ThreadPool.m_Shutdown = 0;
    pthread_mutex_init(&m_ThreadPool.m_QueueLock, NULL);
    pthread_cond_init(&m_ThreadPool.m_QueueReady, NULL);
    m_ThreadPool.m_pThreadID    = new pthread_t[MaxThreadNum];
    m_ThreadPool.m_MaxThreadNum = MaxThreadNum;
    m_ThreadPool.m_CurQueueSize = 0;
    m_ThreadPool.m_pQueueHead   = NULL;
    for (int i = 0; i < MaxThreadNum; i++) {
        pthread_create(&m_ThreadPool.m_pThreadID[i], NULL, ThreadPoolRoutine, NULL);
        usleep(10);
    }
}

void CThreadPoolManager::ThreadPoolManagerAddWorker(void *(*Process)(void * Arg), void * Arg)
{
    CThreadWorker * pNewWorker = new CThreadWorker;

    pNewWorker->Process = Process;
    pNewWorker->Arg     = Arg;
    pNewWorker->m_Next  = NULL;
    pthread_mutex_lock(&m_ThreadPool.m_QueueLock);
    CThreadWorker * pTempWorker = m_ThreadPool.m_pQueueHead;
    if (pTempWorker != NULL) {
        while (pTempWorker->m_Next != NULL) {
            pTempWorker = pTempWorker->m_Next;
        }
        pTempWorker->m_Next = pNewWorker;
    } else   {
        m_ThreadPool.m_pQueueHead = pNewWorker;
    }
    assert(m_ThreadPool.m_pQueueHead != NULL);
    m_ThreadPool.m_CurQueueSize++;
    pthread_mutex_unlock(&m_ThreadPool.m_QueueLock);
    pthread_cond_signal(&m_ThreadPool.m_QueueReady);
}

int CThreadPoolManager::ThreadPoolManagerDestroy()
{
    // while( m_ThreadPool.m_CurQueueSize != 0 )usleep(1000);
    // 虽然上面一行不加锁也可以正常执行，但不严紧
    while (1) {
        pthread_mutex_lock(&m_ThreadPool.m_QueueLock);
        if (m_ThreadPool.m_CurQueueSize == 0) {
            pthread_mutex_unlock(&m_ThreadPool.m_QueueLock);
            break;
        }
        pthread_mutex_unlock(&m_ThreadPool.m_QueueLock);
        usleep(10);
        continue;
    }

    if (m_ThreadPool.m_Shutdown > 0)
        return -1;

    m_ThreadPool.m_Shutdown = 1;
    pthread_cond_broadcast(&m_ThreadPool.m_QueueReady);
    for (int i = 0; i < m_ThreadPool.m_MaxThreadNum; i++) {
        pthread_join(m_ThreadPool.m_pThreadID[i], NULL);
    }
    delete [] m_ThreadPool.m_pThreadID;
    CThreadWorker * Head = NULL;
    while (m_ThreadPool.m_pQueueHead != NULL) {
        Head = m_ThreadPool.m_pQueueHead;
        m_ThreadPool.m_pQueueHead = m_ThreadPool.m_pQueueHead->m_Next;
        delete m_ThreadPool.m_pQueueHead;
    }
    pthread_mutex_destroy(&m_ThreadPool.m_QueueLock);
    pthread_cond_destroy(&m_ThreadPool.m_QueueReady);

    return 0;
} // CThreadPoolManager::ThreadPoolManagerDestroy
