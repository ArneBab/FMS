#include "../../include/db/sqlite3db.h"

#ifdef XMEM
	#include <xmem.h>
#endif

namespace SQLite3DB
{

DB::DB()
{
	Initialize();
}

DB::DB(const std::string &filename)
{
	Initialize();
	Open(filename);
}

DB::~DB()
{
	if(IsOpen())
	{
		Close();
	}
}

const bool DB::Close()
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		m_lastresult=sqlite3_close(m_db);
		if(m_lastresult==SQLITE_OK)
		{
			m_db=NULL;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

const bool DB::Execute(const std::string &sql)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		m_lastresult=sqlite3_exec(m_db,sql.c_str(),NULL,NULL,NULL);
		if(m_lastresult==SQLITE_OK)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

const bool DB::ExecuteInsert(const std::string &sql, long &insertid)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		m_lastresult=sqlite3_exec(m_db,sql.c_str(),NULL,NULL,NULL);
		if(m_lastresult==SQLITE_OK)
		{
			insertid=sqlite3_last_insert_rowid(m_db);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

const int DB::GetLastError(std::string &errormessage)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		int errcode=sqlite3_errcode(m_db);
		const char *errmsg=sqlite3_errmsg(m_db);
		if(errmsg)
		{
			errormessage=errmsg;
		}
		return errcode;
	}
	else
	{
		return SQLITE_OK;
	}
}

void DB::Initialize()
{
	m_db=NULL;
	m_lastresult=SQLITE_OK;
}

const bool DB::IsOpen()
{
	Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
	return m_db ? true : false;
}

const bool DB::Open(const std::string &filename)
{
	if(IsOpen()==true)
	{
		Close();	
	}
	if(IsOpen()==false)
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		m_lastresult=sqlite3_open(filename.c_str(),&m_db);
		if(m_lastresult==SQLITE_OK)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Statement DB::Prepare(const std::string &sql)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		sqlite3_stmt *statement=NULL;
		m_lastresult=sqlite3_prepare_v2(m_db,sql.c_str(),sql.size(),&statement,NULL);
		if(m_lastresult==SQLITE_OK)
		{
			return Statement(statement);
		}
		else
		{
			return Statement();
		}
	}
	else
	{
		return Statement();
	}
}

Recordset DB::Query(const std::string &sql)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		char **rs=NULL;
		int rows,cols;
		m_lastresult=sqlite3_get_table(m_db,sql.c_str(),&rs,&rows,&cols,NULL);
		if(m_lastresult==SQLITE_OK)
		{
			return Recordset(rs,rows,cols);
		}
		else
		{
			sqlite3_free_table(rs);
			return Recordset();
		}
	}
	else
	{
		return Recordset();
	}
}

const int DB::SetBusyTimeout(const int ms)
{
	if(IsOpen())
	{
		Poco::ScopedLock<Poco::FastMutex> g(m_mutex);
		m_lastresult=sqlite3_busy_timeout(m_db,ms);
		return m_lastresult;
	}
	else
	{
		return SQLITE_ERROR;
	}
}

}	// namespace
