#ifndef __THREAD_POOL_H__ 
#define __THREAD_POOL_H__ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
 
typedef struct _Node 
{   
    void *(*call_back)(void *arg); //call_back() func 
    void *arg;                     //回调参数 
    struct _Node *next; 
 
} node_t; 
 
//线程池结构体 
typedef struct _thread_pool 
{   
    //条件变量和锁 
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _Node
{
    void *(*call_back)(void *arg); //call_back() func
    void *arg;                     //回调参数
    struct _Node *next;

} node_t;

//线程池结构体
typedef struct _thread_pool
{
    //条件变量和锁
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    //队列指针
    node_t *first;
    node_t *tail;

    size_t max_thread_count; //最大线程数
    size_t counter;          //已经创建的线程数
    size_t idle;             //空闲线程数
    size_t quit;             //1:退出线程池

} threadpool_t;

///////////////////////////函数接口//////////////////////
void threadpool_init(threadpool_t *thread, int num);//线程池初始化

void threadpool_add_task(threadpool_t *thread, void *(*fun)(void *), void *args);//添线程

void threadpoll_destroy(threadpool_t *thread);//销毁线程池

#endif /*__THREAD_POOL_H__*/
