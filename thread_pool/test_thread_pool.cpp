#include "thread_pool.h"

#define INIT_POOL_COUNT 3

////////////////////////////线程处理//////////////////////////
void* thread_fun(void *arg)
{
   // printf("执行线程回调函数:thread_fun()\n");
    threadpool_t *tp = (threadpool_t *)arg;
    while (1)
    {
        pthread_mutex_lock(&tp->mutex);
        tp->idle++;
        //当无产品消费时 消费者阻塞wait
        while (tp->first == tp->tail && tp->quit != 1)
        {
            pthread_cond_wait(&tp->cond, &tp->mutex);
        }
        //消费
        tp->idle--;
        //pool中由多个任务，可能被其它线程抢走，此处要判断是否为空
        if (tp->first == NULL)
        {
            //取任务处理
            node_t *tmp = tp->first;
            tp->first = tmp->next;

            pthread_mutex_unlock(&tp->mutex); //已经取到任务。所以解锁ok
            tmp->call_back(tmp->arg);         //执行任务

            free(tmp);
            tmp = NULL;
            pthread_mutex_lock(&tp->mutex);
        }
        if (tp->quit == 1 && tp->first == NULL)
        {
            tp->counter--;
            if(tp->counter==0){ //唤醒阻塞在1处的wait
                pthread_cond_signal(&tp->cond);
            }
            pthread_mutex_unlock(&tp->mutex);
            break;
        }
        pthread_mutex_lock(&tp->mutex);
    }
    printf("线程%p退出\n");
}
/////////////////////////////函数实现/////////////////////////
//线程池初始化
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
//添线程
void threadpool_add_task(threadpool_t *thread, void *(*fun)(void *), void *args)
{
    //printf("执行add\n");
    node_t *newtask = (node_t *)malloc(sizeof(node_t)); //创建任务
    newtask->call_back = NULL;
    newtask->arg = args;
    newtask->next = NULL;

    //加入队列中
    //作为共享资源，队列需要加锁
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
    //发现空闲线程 唤醒
    if (thread->idle > 0)
    {
        printf("唤醒空闲进程\n");
        pthread_cond_signal(&thread->cond);
    }
    {
        pthread_t tid;
        printf("创建进程=%d\n",tid);
        pthread_create(&tid, NULL, thread_fun, thread);
        thread->counter++;
    }
    pthread_mutex_unlock(&thread->mutex);
}
//销毁
void threadpool_destroy(threadpool_t *thread)
{
    printf("执行销毁\n");
    if (thread->quit == 1)
        return;
    pthread_mutex_lock(&thread->mutex);
    thread->quit = 1;
    //如果要有空闲线程
    if (thread->counter > 0)
    {
        if (thread->idle > 0)
        while (thread->counter > 0)                //有线程在执行任务，没有被唤醒
            pthread_cond_wait(&thread->cond, &thread->mutex); //1
    }
    pthread_mutex_unlock(&thread->mutex);
    return;
}

///////////////////////////////////////////////////////////////////
void *myfun(void *arg)
{
    //printf("执行任务处理函数···\n");
    int num = *(int *)arg;
    printf("%d task run in %p\n", num, pthread_self()); //打印进程id
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
                 
