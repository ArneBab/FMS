#include "../../include/unicode/unicodestring.h"
#include "../../include/unicode/utfconversion.h"
#include "../../include/unicode/utf8.h"

#include <iterator>

UnicodeString::UnicodeString():m_flags(FLAG_NARROW_OK|FLAG_WIDE_OK),m_widestring(L""),m_narrowstring("")
{

}

UnicodeString::UnicodeString(const std::string &utf8string):m_flags(FLAG_NARROW_OK),m_widestring(L""),m_narrowstring(utf8string)
{
	CheckAndReplaceInvalid();
}

UnicodeString::UnicodeString(const std::wstring &widestring):m_flags(FLAG_WIDE_OK),m_widestring(widestring),m_narrowstring("")
{

}

const size_t UnicodeString::CharacterCount() const
{
	if((m_flags & FLAG_NARROW_OK)!=FLAG_NARROW_OK)
	{
		std::string temp=m_narrowstring;
		if(UTFConversion::ToUTF8(m_widestring,temp)==true)
		{
			try
			{
				size_t count=utf8::distance(temp.begin(),temp.end());
				return count;
			}
			catch(...)
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	try
	{
		size_t count=utf8::distance(m_narrowstring.begin(),m_narrowstring.end());
		return count;
	}
	catch(...)
	{
		return 0;
	}
}

const size_t UnicodeString::CharacterCount()
{
	if((m_flags & FLAG_NARROW_OK)!=FLAG_NARROW_OK)
	{
		Narrowen();
	}
	try
	{
		size_t count=utf8::distance(m_narrowstring.begin(),m_narrowstring.end());
		return count;
	}
	catch(utf8::invalid_utf8 &ex)
	{
		std::string validstring;
		utf8::replace_invalid(m_narrowstring.begin(),m_narrowstring.end(),std::back_inserter(validstring));
		m_narrowstring=validstring;
		m_flags=FLAG_NARROW_OK;
		return CharacterCount();
	}
	catch(...)
	{
		return 0;
	}
}

void UnicodeString::CheckAndReplaceInvalid()
{
	if((m_flags & FLAG_NARROW_OK)==FLAG_NARROW_OK)
	{
		std::string::iterator i=m_narrowstring.begin();
		try
		{
			i=utf8::find_invalid(m_narrowstring.begin(),m_narrowstring.end());
		}
		catch(...)
		{
		}
		if(i!=m_narrowstring.end())
		{
			std::string validstring;
			utf8::replace_invalid(m_narrowstring.begin(),m_narrowstring.end(),std::back_inserter(validstring));
			m_narrowstring=validstring;
			m_flags=FLAG_NARROW_OK;
		}
	}
}

void UnicodeString::Narrowen()
{
	if(UTFConversion::ToUTF8(m_widestring,m_narrowstring)==true)
	{
		m_flags|=FLAG_NARROW_OK;	
	}
}

const std::string UnicodeString::NarrowString() const
{
	if((m_flags & FLAG_NARROW_OK)!=FLAG_NARROW_OK)
	{
		std::string temp("");
		UTFConversion::ToUTF8(m_widestring,temp);
		return temp;
	}
	else
	{
		return m_narrowstring;
	}
}

const std::string &UnicodeString::NarrowString()
{
	if((m_flags & FLAG_NARROW_OK)!=FLAG_NARROW_OK)
	{
		Narrowen();
	}
	return m_narrowstring;
}

UnicodeString &UnicodeString::operator=(const std::string &utf8string)
{
	m_narrowstring=utf8string;
	m_flags=FLAG_NARROW_OK;
	CheckAndReplaceInvalid();
	return *this;
}

UnicodeString &UnicodeString::operator=(const std::wstring &widestring)
{
	m_widestring=widestring;
	m_flags=FLAG_WIDE_OK;
	return *this;
}

UnicodeString &UnicodeString::operator+=(const std::string &utf8string)
{
	if((m_flags & FLAG_NARROW_OK)!=FLAG_NARROW_OK)
	{
		Narrowen();
	}
	m_narrowstring+=utf8string;
	m_flags=FLAG_NARROW_OK;
	return *this;
}

UnicodeString &UnicodeString::operator+=(const std::wstring &widestring)
{
	if((m_flags & FLAG_WIDE_OK)!=FLAG_WIDE_OK)
	{
		Widen();
	}
	m_widestring+=widestring;
	m_flags=FLAG_WIDE_OK;
	return *this;
}

void UnicodeString::Trim(const size_t charpos)
{
	if(CharacterCount()>charpos && charpos>0)
	{
		std::string::iterator it=m_narrowstring.begin();
		try
		{
			utf8::advance(it,charpos,m_narrowstring.end());
		}
		catch(...)
		{
		}

		m_narrowstring.erase(it,m_narrowstring.end());
		m_flags=FLAG_NARROW_OK;
	}
	else if(charpos==0)
	{
		m_narrowstring="";
		m_widestring=L"";
		m_flags=FLAG_NARROW_OK|FLAG_WIDE_OK;
	}
}

void UnicodeString::Widen()
{
	if(UTFConversion::FromUTF8(m_narrowstring,m_widestring)==true)
	{
		m_flags|=FLAG_WIDE_OK;
	}
}

const std::wstring UnicodeString::WideString() const
{
	if((m_flags & FLAG_WIDE_OK)!=FLAG_WIDE_OK)
	{
		std::wstring temp(L"");
		UTFConversion::FromUTF8(m_narrowstring,temp);
		return temp;
	}
	else
	{
		return m_widestring;	
	}
}

const std::wstring &UnicodeString::WideString()
{
	if((m_flags & FLAG_WIDE_OK)!=FLAG_WIDE_OK)
	{
		Widen();
	}
	return m_widestring;
}