// Profile.h: interface for the CProfile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ProfileItem.h"
#import "msxml.dll" 
using namespace MSXML;

// typedef'd the array so we could make it easier to pass around if we needed 
// to do so.  As it turns out, we didn't need to pass it around right now.
typedef CTypedPtrArray<CPtrArray, CProfileItem*> CProfileItemArray;

class CProfile
{
protected:
	CProfileItemArray m_tpaItems;
	CString           m_sFileName;
	int               m_nSectionCount;
	bool              m_bFromNormalize;
	bool              m_bAutoSave;
	bool              m_bAddOnFailedGet;
	bool              m_bAddOnFailedSet;

	/// Normalizes the file name into somethng that shouldn't puke out an exception/error.
	CString NormalizeFileName  (CString sFileName);
	/// Loads the keys for the specified section
	void    LoadSectionChildren(CString sSectionName, MSXML::IXMLDOMNodePtr xmlSection);
	/// Base function for the other overloaded GetValue functions. It's private 
	/// so that some idiot programmer can't ruin my nefarious plan by doing 
	/// something unexpected.
	CString GetValue           (CString sSection, CString sKey);
	/// Adds a new item to the internal list.
	void    AddNewItem         (CString sSection, CString sKey, CString sValue);

public:
	CProfile();
	virtual ~CProfile();

	// Load/save functions
	/// Loads the specified file with the following conditions:
	///
	/// If the filename is NULL, the name defaults to the path and name of the 
	/// application with a "xml" extension.
	///
	/// If no filename is specified, the filename is set to the name of the 
	/// application with a "xml" extension.  To specifiy a filename, you MUST 
	/// include an extension. If no filename is specified, the last element of 
	/// the specified value will be interpreted as part of the path name. If 
	/// the path doesn't exist, the program's folder will be used.
	///
	/// If no path is specified, the path to the application is used.
	///
	bool Load  (CString sFileName=_T(""));
	/// Saves the file - if it doesn't exist, it is created, otherwise, the 
	/// existing file will be overwritten.
	bool Save  ();
	/// Saves the xml file under the specified name (name is normallized).
	bool SaveAs(CString sFileName);

	// Automatic behaviors
	/// Causes the class to either autosave or not when the object goes out of 
	/// scope. Default value is true.
	void    SetAutoSave(bool bVal)       { m_bAutoSave = bVal;       };
	/// Retrieves the object's current autosave status.
	bool    GetAutoSave()                { return m_bAutoSave;       };

	/// If you try to retrieve a value and it doesn't yet exist, setting this 
	/// toggle causes the class to either automatically add the non-existant 
	/// section/key, or not, depending on the state of the specified value. 
	/// Default value is true.
	void    SetAddOnFailedGet(bool bVal) { m_bAddOnFailedGet = bVal; };
	/// If you try to set a value and its key doesn't yet exist, setting this 
	/// toggle causes the class to either automatically add the non-existant 
	/// section/key, or not, depending on the state of the specified value. 
	/// Default value is true.
	void    SetAddOnFailedSet(bool bVal) { m_bAddOnFailedSet = bVal; };
	/// Retrieves the object's current "auto-add on failed gets" status.
	bool    GetAddOnFailedGet()          { return m_bAddOnFailedGet; };
	/// Retrieves the object's current "auto-add on failed sets" status.
	bool    SetAddOnFailedSet()          { return m_bAddOnFailedSet; };

	// general list manipualtions
	/// Deletes the entire list of keys.
	void DeleteItems  ();
	/// Removes the specified key from the specified section.
	void RemoveKey    (CString sSection, CString sKey);
	/// Removes all keys for the specified section, effectively removing the 
	/// section.
	void RemoveSection(CString sSection);

	// Gets
	/// Check to see if a key exists. You can use this function if you want to 
	/// check for a key value without implicity adding it to the objects key 
	/// list if it doesn't.
	bool    GetValueExists(CString sSection, CString strKey);
	/// Retrieves a string value.
	CString GetValue      (CString sSection, CString sKey, CString sDefault);
	/// Retrieves an integer (long) value.
	long    GetValue      (CString sSection, CString sKey, long nDefault   );
	/// Retrieves a floating point (double) value.
	double  GetValue      (CString sSection, CString sKey, double nDefault );

	// Sets
	/// Sets a string value. If the specified section and/or key do not exist, 
	/// they are automatically added to the list.
	bool    SetValue      (CString sSection, CString sKey, CString strValue);
	/// Sets an integer (long) value. If the specified section and/or key do 
	/// not exist, they are automatically added to the list.
	bool    SetValue      (CString sSection, CString sKey, long nValue     );
	/// Sets a floating point (double) value. If the specified section and/or 
	/// key do not exist, they are automatically added to the list.
	bool    SetValue      (CString sSection, CString sKey, double nValue, int nPrecision=0);

	// Debugging
	/// Retrieves the number of sections that were originally loaded. Generally 
	/// only used when debugging to make sure your xml file loaded as expected.
	int     GetOriginalSectionCount() { return m_nSectionCount;           };
	/// Retrieves the number of keys we currenty have. This value includes 
	/// all new keys that we've added while the object is in scope.  New keys 
	/// can be added by trying to retrieve a value for a key that doesn't yet 
	/// exist.
	int     GetKeyCount()     { return (int)m_tpaItems.GetSize(); };

	/// Used for debuugging - displays an AfxMessageBox with the contents of 
	/// the XML file. Default display is all items. If you have more than a 
	/// couple of dozen entries, consider limiting the range, or use the other 
	/// version of the function that shows contents according to section.
	void ShowContentsByRange(int nStart=-1, int nStop=-1);
	/// Used for debuugging - displays an AfxMessageBox with the contents of 
	/// the XML file according to the specified section. Of course, if you have 
	/// a crap-load of section items, you might have cause to create your own 
	/// debugging stuff.
	void ShowContentsBySection(CString sSection);

};

