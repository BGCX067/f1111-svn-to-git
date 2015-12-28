// Profile.cpp: implementation of the CProfile class.
//
// This class allows a program to support INI-style XML files. In other 
// words, the XML file being loaded MUST have sections (top-level nodes) 
// and each section supports unlimited children.  So, if you were to 
// create an INI file that contains the following:
//
// [GENERAL]
// setting1=0
// setting2=test
// [SPECIAL]
// setting1=spider
// setting2=0.12345
//
// ...the corresponding xml file would look like this:
//
// <xml>
//		<GENERAL>
//			<setting1>0</setting1>
//			<setting2>test</setting2>
//		</GENERAL>
//		<SPECIAL>
//			<setting1>spider</setting1>
//			<setting2>0.12345</setting2>
//		</SPECIAL>
// </xml>
//
// This class does NOT support child nodes for the 2nd-level nodes 
// because we're only trying to emulate INI files with it.
//
// We need to call CoInititialize (or CoInititializeEx) in 
// InitiInstance and CoUninitialize in ExitInstance of our app class 
// before using this class. 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileName.h"
#include "Profile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CProfile::CProfile()
{
	m_sFileName = "";
	m_bFromNormalize = false;
	m_bAutoSave = true;
	m_bAddOnFailedGet = true;
	m_bAddOnFailedSet = true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CProfile::~CProfile()
{
	if (m_bAutoSave)
	{
		Save();
	}
	DeleteItems();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CProfile::DeleteItems()
{
	int i = 0;
	while (i < m_tpaItems.GetSize())
	{
		delete m_tpaItems.GetAt(i++);
	}
	m_tpaItems.RemoveAll();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::Load(CString sFileName/*=""*/)
{
	m_sFileName = NormalizeFileName(sFileName);

	// Since we're loading new settings, we want to delete anything we've 
	// previously loaded 
	DeleteItems();

	//get a dom pointer
	MSXML::IXMLDOMDocumentPtr XMLDom;
	HRESULT hResult = XMLDom.CreateInstance("Microsoft.XMLDOM");
	if ( FAILED(hResult) )
	{
		AfxMessageBox("Failed to load DOM. Missing MSXML.DLL?");
		return false;
	}
	
	bstr_t bstrFile = m_sFileName;
	if (XMLDom->load(bstrFile) != VARIANT_TRUE)
	{
		CString strError = _T("");
		strError.Format(_T("Unable to load file:  %s\n\nPlease verify that the file exists."), m_sFileName);
		AfxMessageBox(strError);
		return false;
	}

	MSXML::IXMLDOMNodeListPtr XMLNodeList = XMLDom->GetdocumentElement()->selectNodes(_T("*"));

	m_nSectionCount = 0;
	int nLen = XMLNodeList->Getlength();
	if(nLen > 0)
	{
		//reset to the beginning of the document
		XMLNodeList->reset();

		// loop through all the top level nodes - all top-level nodes are 
		// assumed to be "sections". Section values are ignored.
		for(int i = 0; i < nLen; i++)
		{
			MSXML::IXMLDOMNodePtr XMLNode = XMLNodeList->Getitem(i);

			CString sSectionName = _T("");
			BSTR    bstr = XMLNode->GetnodeName();
			sSectionName = bstr;
			m_nSectionCount++;
			LoadSectionChildren(sSectionName, XMLNode);
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CProfile::LoadSectionChildren(CString sSectionName, MSXML::IXMLDOMNodePtr xmlSection)
{
	if (!xmlSection->hasChildNodes())
	{
		return;
	}
	// retrieve the list of child nodes
	MSXML::IXMLDOMNodeListPtr XMLNodeList = xmlSection->GetchildNodes();
	// see  how many we have
	int nLen = XMLNodeList->Getlength();
	// and add them to our list
	if(nLen > 0)
	{
		//reset to the beginning of the document
		XMLNodeList->reset();

		// loop through all the top level nodes - all nodes at this level are 
		// assumed to be actual values. Child nodes are ignore for these items.
		for(int i = 0; i < nLen; i++)
		{
			MSXML::IXMLDOMNodePtr XMLNode = XMLNodeList->Getitem(i);

			CString sKeyName     = _T("");
			CString sKeyValue    = _T("");

			BSTR bstr = XMLNode->GetnodeName();
			sKeyName  = bstr;
			bstr      = XMLNode->Gettext();
			sKeyValue = bstr;

			CProfileItem* pItem = new CProfileItem();
			pItem->SetSection (sSectionName);
			pItem->SetKeyName (sKeyName    );
			pItem->SetKeyValue(sKeyValue   );

			m_tpaItems.Add(pItem);
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::SaveAs(CString sFileName)
{
	if (!m_bFromNormalize)
	{
		m_sFileName = NormalizeFileName(sFileName);
	}
	return Save();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::Save()
{
	CProfileItem* pItem = NULL;
	int nKeyCount = (int)m_tpaItems.GetSize();

	// first,, we need to find our section names
	CStringArray saSections;
	CString      sLastSectionFound = _T("");
	for (int i = 0; i < nKeyCount; i++)
	{
		CProfileItem* pItem = m_tpaItems.GetAt(i);
		CString sItemSection = pItem->GetSection();
		if (sItemSection.CompareNoCase(sLastSectionFound) != 0)
		{
			saSections.Add(sItemSection);
			sLastSectionFound = sItemSection;
		}
	}

	int nSectionCount = (int)saSections.GetSize();
	CString sOutput = _T("<xml>\n");

	for (int i = 0; i < nSectionCount; i++)
	{
		CString sSectionName = saSections.GetAt(i);
		sOutput += _T("\t<") + sSectionName + _T(">\n");
		for (int j = 0; j < nKeyCount; j++)
		{
			pItem = m_tpaItems.GetAt(j);
			if (sSectionName.CompareNoCase(pItem->GetSection()) == 0)
			{
				CString sKeyName = pItem->GetKeyName();
				CString sValue = pItem->GetKeyValue();
				sOutput += (_T("\t\t<") + sKeyName + _T(">") +  sValue +  _T("</") + sKeyName + _T(">\n"));
			}
		}
		sOutput += _T("\t</") + sSectionName + _T(">\n");
	}
	sOutput += _T("</xml>\n");


	CStdioFile fDestination;
	if (fDestination.Open(m_sFileName, CFile::modeWrite | CFile::modeCreate) == 0)
	{
		TRACE("Could not create Destination File -- %s\n", m_sFileName);
		return false;
	}
	else
	{
		fDestination.WriteString(sOutput);
		fDestination.Close();
	}
	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CString CProfile::GetValue(CString sSection, CString sKey)
{
	CProfileItem *pItem = NULL;
	int nCount = (int)m_tpaItems.GetSize();
	for(int i = 0; i < nCount; i++)
	{
		pItem = m_tpaItems.GetAt(i);
		if (pItem != NULL)
		{
			if (pItem->GetSection().CompareNoCase(sSection) == 0 && pItem->GetKeyName().CompareNoCase(sKey) == 0)
			{
				return pItem->GetKeyValue();
			}
		}
	}
	
	return _T("");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CProfile::AddNewItem(CString sSection, CString sKey, CString sValue)
{
	CProfileItem* pItem = new CProfileItem();
	if (pItem)
	{
		pItem->SetSection (sSection);
		pItem->SetKeyName (sKey    );
		pItem->SetKeyValue(sValue  );
		m_tpaItems.Add(pItem);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CString CProfile::GetValue(CString sSection, CString sKey, CString sDefault)
{
	CString sVal = GetValue(sSection, sKey);
	if (sVal.IsEmpty())
	{
		sVal = sDefault;
	}
	else
	{
		if (m_bAddOnFailedGet)
		{
			AddNewItem(sSection, sKey, sDefault);
		}
	}
	return sVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
long CProfile::GetValue(CString sSection, CString sKey, long nDefault)
{
	CString sVal = GetValue(sSection, sKey);
	long    nVal = nDefault;
	if (!sVal.IsEmpty())
	{
		nVal = atol((LPCTSTR)sVal);
	}
	else
	{
		if (m_bAddOnFailedGet)
		{
			sVal.Format("%ld", nDefault);
			AddNewItem(sSection, sKey, sVal);
		}
	}
	return  nVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
double CProfile::GetValue(CString sSection, CString sKey, double nDefault)
{
	CString sVal = GetValue(sSection, sKey);
	double  nVal = nDefault;
	if (!sVal.IsEmpty())
	{
		 nVal = atof((LPCTSTR)sVal);
	}
	else
	{
		if (m_bAddOnFailedGet)
		{
			sVal.Format("%lf", nDefault);
			AddNewItem(sSection, sKey, sVal);
		}
	}
	return  nVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::SetValue(CString sSection, CString sKey, CString sValue)
{
	CProfileItem *pItem = NULL;
	int nCount = (int)m_tpaItems.GetSize();
	for(int i = 0; i < nCount; i++)
	{
		pItem = m_tpaItems.GetAt(i);
		if (pItem != NULL)
		{
			if (pItem->GetSection().CompareNoCase(sSection) == 0 && pItem->GetKeyName().CompareNoCase(sKey) == 0)
			{
				pItem->SetKeyValue(sValue);
				return true;
			}
		}
	}
	if (m_bAddOnFailedSet)
	{
		AddNewItem(sSection, sKey, sValue);
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::SetValue(CString sSection, CString sKey, long nValue)
{
	CString sVal;
	sVal.Format("%ld", nValue);
	return SetValue(sSection, sKey, sVal);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::SetValue(CString sSection, CString sKey, double nValue, int nPrecision/*=0*/)
{
	CString sVal;
	sVal.Format("%lf", nValue);
	return SetValue(sSection, sKey, sVal);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CProfile::GetValueExists(CString sSection, CString sKey)
{
	bool bExists = false;
	CProfileItem *pItem = NULL;
	int nCount = (int)m_tpaItems.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		pItem = m_tpaItems.GetAt(i);
		if (pItem != NULL)
		{
			if (pItem->GetSection().CompareNoCase(sSection) == 0 && pItem->GetKeyName().CompareNoCase(sKey) == 0)
			{
				bExists = true;
				break;
			}
		}
	}
	return bExists;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CProfile::RemoveKey(CString sSection, CString sKey)
{
	CProfileItem *pItem = NULL;
	int nCount = (int)m_tpaItems.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		pItem = m_tpaItems.GetAt(i);
		if (pItem != NULL)
		{
			if (pItem->GetSection().CompareNoCase(sSection) == 0 && pItem->GetKeyName().CompareNoCase(sKey) == 0)
			{
				delete pItem;
				m_tpaItems.RemoveAt(i);
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CProfile::RemoveSection(CString sSection)
{
	CProfileItem *pItem = NULL;
	int nCount = (int)m_tpaItems.GetSize();
	for (int i = nCount - 1; i >=0; i--)
	{
		pItem = m_tpaItems.GetAt(i);
		if (pItem != NULL)
		{
			if (pItem->GetSection().CompareNoCase(sSection) == 0)
			{
				delete pItem;
				m_tpaItems.RemoveAt(i);
			}
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CString CProfile::NormalizeFileName(CString sFileName)
{
	// let's get our default values before we start evaluating
	CString sResult          = _T("");
	CString sDefault         = forceExtention(GetProgramPath(TRUE),".xml");
	CString sDefaultFileName = justFileName(sDefault);
	CString sDefaultPath     = justPath(sDefault);
	// now we break up the specified filename into it's component parts
	CString sPath = justPath(sFileName);
	CString sName = justFileName(sFileName);

	AddBackSlash(sDefaultPath);

	// if a path wasn't specified, set the specified path to our default path
	if (sPath.IsEmpty())
	{
		sPath = sDefaultPath;
	}
	else
	{
		// if the path was specified but doesn't exist, set it to our default path
		if (!FileExists(sPath))
		{
			sPath = sDefaultPath;
		}
	}
	// add a backslash
	AddBackSlash(sPath);

	// if the name wasn't specified at all, so set it to our default name
	if 	(sName.IsEmpty())
	{
		sName = sDefaultFileName;
	}
	else
	{
		// we have a filename, but it only includes the extension (otherwise, we wouldn't have a filename)
		// if the name part is empty, set it to our default name part
		if (justName(sFileName).IsEmpty())
		{
			sName = justName(sDefaultFileName) + justExtention(sName);
		}
	}

	// if you assert here, something went horribly wrong with this process
	ASSERT(!sPath.IsEmpty() && !sName.IsEmpty());

	sResult = sPath + sName;

	if (!FileExists(sResult))
	{
		m_bFromNormalize = true;
		SaveAs(sResult);
		m_bFromNormalize = false;
	}

	return sResult;
}

//------------------------------------------------------------------------------
// This function should only be used for debugging to nsure that the object 
// contains what you think it should contain...
//------------------------------------------------------------------------------
void CProfile::ShowContentsByRange(int nStart/*=-1*/, int nStop/*=-1*/)
{
	// find our last index
	int nLastIndex = (int)m_tpaItems.GetSize() - 1;
	// if our starting index is < 0, set it to 0
	if (nStart < 0)
	{
		nStart = 0;
	}
	// if our starting point exceeds the last index, set it to the last index
	if (nStart > nLastIndex)
	{
		nStart = nLastIndex;
	}
	// if there's anything weird about the stop index, set it to the last item 
	// in the list.
	if (nStop < 0 || nStop > nLastIndex)
	{
		nStop = nLastIndex;
	}

	CProfileItem *pItem = NULL;
	CString sOutput = _T("");

	// now loop through the specified (and maybe massaged) range, and add each 
	// items physical and technological likeness to our own.
	for (int i = nStart; i <= nStop; i++)
	{
		CString sTemp = _T("");
		pItem = m_tpaItems.GetAt(i);
		sTemp.Format("%03d\t%s / %s = '%s'\t\n", 
		               i, 
					   pItem->GetSection(), 
					   pItem->GetKeyName(), 
					   pItem->GetKeyValue());
		sOutput += sTemp;
	}
	// show us what we have
	AfxMessageBox(sOutput, MB_ICONINFORMATION);
}

//------------------------------------------------------------------------------
// This function should only be used for debugging to nsure that the object 
// contains what you think it should contain...
//------------------------------------------------------------------------------
void CProfile::ShowContentsBySection(CString sSection)
{
	CProfileItem* pItem = NULL;
	int nKeyCount = (int)m_tpaItems.GetSize();
	CString sOutput = CString("Contents of section: ") + sSection + CString("\n-------------------------------------------------\t\n");

	for (int j = 0; j < nKeyCount; j++)
	{
		pItem = m_tpaItems.GetAt(j);
		if (sSection.CompareNoCase(pItem->GetSection()) == 0)
		{
			CString sTemp = _T("");
			sTemp.Format("%s = '%s'\t\n", 
			             pItem->GetKeyName(), 
			             pItem->GetKeyValue());
			sOutput += sTemp;
		}
	}
	AfxMessageBox(sOutput, MB_ICONINFORMATION);
}

