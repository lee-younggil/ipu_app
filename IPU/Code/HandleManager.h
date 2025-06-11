#pragma once

#include <list>

#include "Service.h"

class CHandleManager
{
private:
    //typedef std::list<CIFService *> HANDLE_LIST;
    typedef std::list<CService *> HANDLE_LIST;
	HANDLE_LIST m_listHandle;

public:
	CHandleManager(){}
	~CHandleManager(){ DeleteHandle(); }
    void RegisterHandle( CService *hHandle ){ m_listHandle.push_back( hHandle ); }
	void initialization()
	{
		for( HANDLE_LIST::iterator it = m_listHandle.begin() ; it != m_listHandle.end() ; it++ )
		{
			(*it)->initialization();
		}
	}
	void ServiceStart()
	{
		for( HANDLE_LIST::iterator it = m_listHandle.begin() ; it != m_listHandle.end() ; it++ )
		{
			(*it)->ServiceStart();
		}
	}
	void ServiceStop()
	{
		for( HANDLE_LIST::reverse_iterator it = m_listHandle.rbegin() ; it != m_listHandle.rend() ; it++ )
		{
			(*it)->ServiceStop();
		}
	}
	void DeleteHandle()
	{
		for( HANDLE_LIST::reverse_iterator it = m_listHandle.rbegin() ; it != m_listHandle.rend() ; it++ )
		{
			delete (*it);
		}
		m_listHandle.clear();
	}
};
