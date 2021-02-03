////////////////////////////////////////////////////////////////////////
//
// Simple implementation of Observer Design Pattern to implement thread
// class which can send notifications to attached observers when the 
// new thread is created status as 'true' with thread ID and status as
// 'false' when the thread execution ends
//
////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class IObserver;
////////////////////////////////////////////////////////////////////////
class ISubject
{
public:
	virtual void AttachObserver(IObserver * observerPtr)=0;
	virtual void DettachObserver(IObserver * observerPtr)=0;
	virtual void Notify(DWORD ThreadID, bool Status)=0;
};
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
class IObserver
{
public:
	virtual void AutoAttach(ISubject * subjectPtr)=0;
	virtual void AutoDettach(ISubject * subjectPtr)=0;
	virtual void Update(DWORD ThreadID, bool Status)=0;
};
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
class CSubject : public ISubject
{
public:

	CSubject(){}
	virtual ~CSubject(){}
	virtual void AttachObserver(IObserver * observerPtr){}
	virtual void DettachObserver(IObserver * observerPtr){}
	virtual void Notify(DWORD ThreadID, bool Status){}
};
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
class ThreadWraper : public CSubject
{
	DWORD m_threadID, m_threadWaitTime;
	HANDLE m_threadHandle;

protected:
	static vector<IObserver *> ObserverVect;

public:
	ThreadWraper(int);
	~ThreadWraper();
	void AttachObserver(IObserver * observerPtr);
	void DettachObserver(IObserver * observerPtr);
	void Notify(DWORD ThreadID, bool Status);

	//Thread proc
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
	HANDLE createThread();
	void WaitForThreadToFinish();

};
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
class CObserver : public IObserver
{

public:
	CObserver(){}
	virtual ~CObserver(){}
	virtual void AutoAttach(ISubject * subjectPtr){}
	virtual void AutoDettach(ISubject * subjectPtr){}

	//Call back function
	virtual void Update(DWORD ThreadID, bool Status){}

};
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
class CustomThreadObserver : public CObserver
{
	int m_observerID;
	static int NumberOfBoservers;
public:
	CustomThreadObserver()
	{
		m_observerID = ++NumberOfBoservers;
	}
	~CustomThreadObserver(){}

	void AutoAttach(ISubject * ptr);
	void AutoDettach(ISubject * ptr);
	void Update(DWORD ThreadID, bool Status);

};
////////////////////////////////////////////////////////////////////////

//////////////////////////static initializations////////////////////////
vector<IObserver *> ThreadWraper::ObserverVect;
int CustomThreadObserver::NumberOfBoservers=0;
////////////////////////////////////////////////////////////////////////

ThreadWraper::ThreadWraper(int WaitTime = 1000) : m_threadWaitTime(WaitTime)
{
	ThreadWraper::ObserverVect.reserve(10);
}

ThreadWraper::~ThreadWraper()
{
	if(!ThreadWraper::ObserverVect.empty())
	{
		ThreadWraper::ObserverVect.clear();
	}
}

void ThreadWraper::AttachObserver(IObserver * observerPtr)
{
	ThreadWraper::ObserverVect.push_back(observerPtr);
}

void ThreadWraper::DettachObserver(IObserver * observerPtr)
{
	vector<IObserver *>::iterator i = find(ThreadWraper::ObserverVect.begin(), ThreadWraper::ObserverVect.end(),observerPtr);

	if(i != ThreadWraper::ObserverVect.end())
	{
		ThreadWraper::ObserverVect.erase(i);
	}
}

void ThreadWraper::Notify(DWORD ThreadID, bool Status)
{
	for(unsigned i = 0; i < ThreadWraper::ObserverVect.size(); ++i)
	{
		ThreadWraper::ObserverVect[i]->Update(ThreadID, Status);
	}
}

DWORD WINAPI ThreadWraper::ThreadProc(LPVOID lpParam)
{
	ThreadWraper * ptr = static_cast<ThreadWraper *>(lpParam);

	ptr->Notify(ptr->m_threadID,true);

	//wait time for thread execution
	Sleep(ptr->m_threadWaitTime);

	ptr->Notify(ptr->m_threadID,false);

	return 0;
}

HANDLE ThreadWraper::createThread()
{
	m_threadHandle = CreateThread(NULL,0,ThreadProc,(LPVOID)this,0,&m_threadID);
	return m_threadHandle;
}

void ThreadWraper::WaitForThreadToFinish()
{
	if(m_threadHandle != NULL)
	{
		WaitForSingleObject(m_threadHandle,INFINITE);
	}
}

////////////////////////////////////////////////////////////////////////

void CustomThreadObserver::AutoAttach(ISubject * subjectPtr)
{
	subjectPtr->AttachObserver(this);
}

void CustomThreadObserver::AutoDettach(ISubject * subjectPtr)
{
	subjectPtr->DettachObserver(this);
}

void CustomThreadObserver::Update(DWORD ThreadID, bool Status)
{
	if(Status)
	{
		cout<<"Observer : "<<this->m_observerID<<" : The thread : "<<ThreadID<<" has started execution."<<endl;
	}
	else
	{
		cout<<"Observer : "<<this->m_observerID<<" : The thread : "<<ThreadID<<" has stoped execution !"<<endl;
	}
}
////////////////////////////////////////////////////////////////////////

int main()
{
/*
				======  Steps to use thread observers  ======

	 1] create thredwraper object with wait time
	 2] create customethreadobserver object then call attach observer on it with address of object created in step 1
	 3] Then call createThread method on threadWraper object
	 4] then update call back will come for all observers, attached to that thread
*/


	ThreadWraper Thread1(15000);

	CustomThreadObserver ThreadObserver1, ThreadObserver2;

	ThreadObserver1.AutoAttach(&Thread1);
	ThreadObserver2.AutoAttach(&Thread1);

	Thread1.createThread();

	Thread1.WaitForThreadToFinish();

	return 0;
}
