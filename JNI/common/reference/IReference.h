#ifndef __IREFERENCE__
#define __IREFERENCE__

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include "sync/interlock.h"
//#define _GLIBCXX_ATOMIC_BUILTINS 1
	#ifndef __stdcall
	#define __stdcall 
	#endif
	//#include <atomic.h>
#endif
/**
 * Abstract an Reference Counting object,\see http://en.wikipedia.org/wiki/Reference_counting
 */
class IReference
{
public:
	virtual ~IReference(){};
	/**
	 * Increments a reference count for every new copy of an interface pointer to a specified interface on a particular object. 
	 * \return Returns the value of the reference count.When an interface is fully released, the reference count is zero. This information should be used only for diagnostics and testing.
	 */
	virtual unsigned long	__stdcall AddRef() = 0;
	/**
	 * Decrements the reference count for the specified interface on a particular object. 
	 * \return Returns the value of the reference count.When an interface is fully released, the reference count is zero. This information should be used only for diagnostics and testing.
	 * If the object reference count goes to zero as a result of calling Release, the object is freed from memory.
	 */
	virtual unsigned long	__stdcall Release()= 0;
};
/**
 * Implement an Reference Counting object .
 */
class CReference
{
public:
	virtual ~CReference(){};
	CReference ()
		: m_lRef(0),m_lDel(1)	
	{
	}
	virtual unsigned long __stdcall AddRef()
	{
		return InterlockedIncrement(&m_lRef);
	}
	virtual unsigned long __stdcall Release()
	{
		if(InterlockedDecrement(&m_lRef) == 0  )
			if(InterlockedDecrement(&m_lDel) == 0)
		{
			delete this;
			return 0;
		}
		return m_lRef;
	}
public:
	volatile long m_lRef;
	volatile long m_lDel;
};
/**
 * Implement an Reference Counting template .
 */
template<class T>
class CReference_T 
	:private CReference
	,public T
	,virtual public IReference
{
public:
	typedef T _TYPE;
	typedef IReference _REF;
	virtual unsigned long __stdcall AddRef()
	{
		return CReference::AddRef();
	}
	virtual unsigned long __stdcall Release()
	{
		return CReference::Release();
	}
protected:
	virtual ~CReference_T(){}
};

/**
 * Used to auto manage an Reference Counting object .
 */
template<class T>
class CRefObj
{
public:
	typedef T _PtrClass;
	CRefObj()
	{
		p=0;
	}
	CRefObj(T* lp)
	{
		if ((p = lp) != 0)
			p->AddRef();
	}
	CRefObj(const CRefObj<T>& lp)
	{
		if ((p = lp.p) != 0)
			p->AddRef();
	}
	~CRefObj()
	{
		if (p)
			p->Release();
	}
	operator  T*() const
	{
		return  p;
	}
	T& operator*() const
	{
		return *p;
	}

	/*T** operator&()
	{
		return &p;
	}*/
	T* operator->() const
	{
		return p;
	}
	T* operator=(T* lp)
	{
		if (lp != 0)
			lp->AddRef();
		if (p)
			p->Release();
		p = lp;
		return p;
	}
	T* operator=(const CRefObj<T>& lp)
	{
		if (lp != 0)
			lp->AddRef();
		if (p)
			p->Release();
		p = lp;
		return p;
	}
	bool operator!() const
	{
		return (p == 0);
	}
	
// 	bool operator==(const T* pT) const
// 	{
// 		return p == pT;
// 	}
	T*GetObj(){return p;}

 	const T*GetObj()const{return p;}
	T*p;
};

#endif //__IREFERENCE__
