#include "../include/option.h"
#include "../include/db/sqlite3db.h"

#ifdef XMEM
	#include <xmem.h>
#endif

const bool Option::Get(const std::string &option, std::string &value)
{
	SQLite3DB::Statement st=m_db->Prepare("SELECT OptionValue FROM tblOption WHERE Option=?;");
	st.Bind(0,option);
	st.Step();
	if(st.RowReturned())
	{
		st.ResultText(0,value);
		return true;
	}
	else
	{
		return false;
	}
}

const bool Option::GetInt(const std::string &option, int &value)
{
	std::string valstr="";
	if(Get(option,valstr))
	{
		std::istringstream istr(valstr);
		if(istr >> value)
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
