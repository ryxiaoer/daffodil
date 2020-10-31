#include "thread_pool.h"

#define INIT_POOL_COUNT 3

////////////////////////////�̴߳���//////////////////////////
void* thread_fun(void *arg)
{
   // printf("ִ���̻߳ص�����:thread_fun()\n");
    threadpool_t *tp = (threadpool_t *)arg;
    while (1)
    {
        pthread_mutex_lock(&tp->mutex);
        tp->idle++;
        //���޲�Ʒ����ʱ ����������wait
        while (tp->first == tp->tail && tp->quit != 1)
        {
            pthread_cond_wait(&tp->cond, &tp->mutex);
        }
        //����
        tp->idle--;
        //pool���ɶ�����񣬿��ܱ������߳����ߣ��˴�Ҫ�ж��Ƿ�Ϊ��
        if (tp->first == NULL)
        {
            //ȡ������
            node_t *tmp = tp->first;
            tp->first = tmp->next;

            pthread_mutex_unlock(&tp->mutex); //�Ѿ�ȡ���������Խ���ok
            tmp->call_back(tmp->arg);         //ִ������

            free(tmp);
            tmp = NULL;
            pthread_mutex_lock(&tp->mutex);
        }
        if (tp->quit == 1 && tp->first == NULL)
        {
            tp->counter--;
            if(tp->counter==0){ //����������1����wait
                pthread_cond_signal(&tp->cond);
            }
            pthread_mutex_unlock(&tp->mutex);
            break;
        }
        pthread_mutex_lock(&tp->mutex);
    }
    printf("�߳�%p�˳�\n");
}
/////////////////////////////����ʵ��/////////////////////////
//�̳߳س�ʼ��
void threadpool_init(threadpool_t *thread, int num)
{
    pthread_cond_init(&thread->cond, NULL);
    pthread_mutex_init(&thread->mutex, NULL);
    thread->first = NULL;
    thread->tail = NULL;
    thread->max_thread_count = num;
    thread->counter = 0;
    thread->idle = 0;
    thread->quit = 0;
}
//���߳�
void threadpool_add_task(threadpool_t *thread, void *(*fun)(void *), void *args)
{
    //printf("ִ��add\n");
    node_t *newtask = (node_t *)malloc(sizeof(node_t)); //��������
    newtask->call_back = NULL;
    newtask->arg = args;
    newtask->next = NULL;

    //���������
    //��Ϊ������Դ��������Ҫ����
    pthread_mutex_lock(&thread->mutex);
    if (thread->first == NULL)
    {
        thread->first = newtask;
    }
    else
    {
        thread->tail->next = newtask;
    }
    thread->tail = newtask;
    //���ֿ����߳� ����
    if (thread->idle > 0)
    {
        printf("���ѿ��н���\n");
        pthread_cond_signal(&thread->cond);
    }
    {
        pthread_t tid;
        printf("��������=%d\n",tid);
        pthread_create(&tid, NULL, thread_fun, thread);
        thread->counter++;
    }
    pthread_mutex_unlock(&thread->mutex);
}
//����
void threadpool_destroy(threadpool_t *thread)
{
    printf("ִ������\n");
    if (thread->quit == 1)
        return;
    pthread_mutex_lock(&thread->mutex);
    thread->quit = 1;
    //���Ҫ�п����߳�
    if (thread->counter > 0)
    {
        if (thread->idle > 0)
        while (thread->counter > 0)                //���߳���ִ������û�б�����
            pthread_cond_wait(&thread->cond, &thread->mutex); //1
    }
    pthread_mutex_unlock(&thread->mutex);
    return;
}

///////////////////////////////////////////////////////////////////
void *myfun(void *arg)
{
    //printf("ִ����������������\n");
    int num = *(int *)arg;
    printf("%d task run in %p\n", num, pthread_self()); //��ӡ����id
    free(arg);
    arg = NULL;
    sleep(1);
    return;
}
int main()
{
    threadpool_t tp;
    threadpool_init(&tp, INIT_POOL_COUNT);
    int i;
    for (i = 0; i < 5; i++)
    {
        int *p = malloc(sizeof(int));
        *p = i;
        threadpool_add_task(&tp, myfun, (void *)p);
    }
    sleep(1);
    threadpool_destroy(&tp);
}
                 
