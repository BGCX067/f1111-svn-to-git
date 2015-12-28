// ProfileItem.h: interface for the CProfileItem class.
//

#pragma once

class CProfileItem
{
public:
	CProfileItem();
	virtual ~CProfileItem();

private:
	CString m_sValue;
	CString m_sKey;
	CString m_sSection;
	
public:
	void    SetSection (CString sVal) { m_sSection = sVal; };
	void    SetKeyName (CString sVal) { m_sKey     = sVal; };
	void    SetKeyValue(CString sVal) { m_sValue   = sVal; };
	CString GetSection ()             { return m_sSection; };
	CString GetKeyName ()             { return m_sKey;     };
	CString GetKeyValue()             { return m_sValue;   };
};

