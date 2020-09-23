
//shared_ptr的简单实现 
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

template <class T>
class mySharedPtr
{
public:
	mySharedPtr(T* Ptr = nullptr) : _Ptr(Ptr), _count(new int(1)), _mutex(new mutex) {}
	~mySharedPtr()
	{
		OldReleases();
	}

	mySharedPtr(const mySharedPtr<T>& sp) : _Ptr(sp._Ptr), _count(sp._count), _mutex(sp._mutex)
	{
		AddCount();
	}
	mySharedPtr<T>& operator=(const mySharedPtr<T>& sp)
	{
		if (this != &sp)
		{
			
			OldReleases();
			
			_Ptr = sp._Ptr;
			_count = sp._count;
			_mutex = sp._mutex;
			AddCount();
		}
		return *this;
	}
public:
	void AddCount()
	{ 
		_mutex->lock();
		(*_count)++;
		_mutex->unlock();
	}


public:
	int UsedCount()
	{
		return *_count;
	}

private:
   //旧资源的释放
    void OldReleases()
    {
        bool dflag = false;

        //引用计数count--
        _mutex->lock();
        if (--(*_count) == 0) //无引用 释放资源
        {
            delete _Ptr;
            delete _count;
            dflag = true;
        }
        _mutex->unlock();

        //注意这里要释放锁！！！（踩坑内存泄漏+1  今日1/n）
        if (dflag == true)
            delete _mutex;
    }

private:
    int     *_count;   //引用计数
    T       *_Ptr;       //实际资源指针
    mutex   *_mutex; //锁
};
int main()
{
	mySharedPtr<int> sp1(new int(10));
	mySharedPtr<int> sp2(sp1);

	cout <<"sp1.UsedCount()="<<sp1.UsedCount() << endl; //2
	cout <<"sp2.UsedCount()=" << sp2.UsedCount() << endl;//2

	mySharedPtr<int> sp3(new int(1));
	sp2 = sp3;
	cout << "sp3.UsedCount()=" << sp3.UsedCount() << endl;//2
	cout << "sp1.UsedCount()=" << sp1.UsedCount() << endl;//1
	cout << "sp2.UsedCount()=" << sp2.UsedCount() << endl;//2

	return 0;
}