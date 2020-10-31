#ifndef __THREAD_POOL_H__ 
#define __THREAD_POOL_H__ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
 
typedef struct _Node 
{   
    void *(*call_back)(void *arg); //call_back() func 
    void *arg;                     //�ص����� 
    struct _Node *next; 
 
} node_t; 
 
//�̳߳ؽṹ�� 
typedef struct _thread_pool 
{   
    //������������ 
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _Node
{
    void *(*call_back)(void *arg); //call_back() func
    void *arg;                     //�ص�����
    struct _Node *next;

} node_t;

//�̳߳ؽṹ��
typedef struct _thread_pool
{
    //������������
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    //����ָ��
    node_t *first;
    node_t *tail;

    size_t max_thread_count; //����߳���
    size_t counter;          //�Ѿ��������߳���
    size_t idle;             //�����߳���
    size_t quit;             //1:�˳��̳߳�

} threadpool_t;

///////////////////////////�����ӿ�//////////////////////
void threadpool_init(threadpool_t *thread, int num);//�̳߳س�ʼ��

void threadpool_add_task(threadpool_t *thread, void *(*fun)(void *), void *args);//���߳�

void threadpoll_destroy(threadpool_t *thread);//�����̳߳�

#endif /*__THREAD_POOL_H__*/
