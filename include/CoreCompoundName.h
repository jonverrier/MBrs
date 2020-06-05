/////////////////////////////////////////
// CoreCompoundName.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef CORECOMPOUNDNAME_INCLUDED
#define CORECOMPOUNDNAME_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"

class CorePathParser;

/**
 * Represents paths for files, network resources, and other named objects
 */
class CORE_API CoreCompoundName
{
public:
	/** @group Construction, destruction, and assignment */
   CoreCompoundName ();
	CoreCompoundName (const CoreCompoundName& copyMe);
	CoreCompoundName (const std::vector<HString>& components, bool isFullPath);
						
	/**
	 * Use the specified parser to construct a compund name from the text
	 */
	CoreCompoundName (const HString& path,
					      const CorePathParser& parser);

	CoreCompoundName&			
   operator= (const CoreCompoundName& copyMe);

	virtual ~CoreCompoundName ();

/** @group Comparison and query methods */
	bool operator== (const CoreCompoundName& anotherPath) const;

	bool operator!= (const CoreCompoundName& anotherPath) const;

	/**
	 * Is it a fully qualified path? 
	 */
	bool isFullPath() const;

	/**
	 * Return the number of components in the path
	 */
	HUint numberOfComponents () const;

	/**
	 * Return the nth component of the path, starting at an initial index of 0
	 */
	HString componentAt (HUint index) const;	

	/**
	 * Does the compound mame have an n'th component ? 
	 */
	bool hasComponentAt(HUint index) const;

private:
	std::vector<HString> m_nameComponents;
	bool                 m_isFullPath;
};

/**
 * Converts an HString into a CoreCompoundName and vice-versa
 *
 * This is an abstract base class; use either Win32 or Linux implementations. 
 */
class CORE_API CorePathParser {
public:
/** @group Destructor */
	virtual ~CorePathParser ();

/** @group Parsing methods */

	/**
	 * Convert a text string into a CoreCompoundName 
	 */
	virtual CoreCompoundName parsePath  (const HString&) const = 0;
	
	/**
	 * Convert an CoreCompoundName into a text string
	 
	 */
	virtual HString formatPath (const CoreCompoundName&) const = 0;

protected:
/**  Construction for use by subclasses */
   CorePathParser	();
   CorePathParser	(const CorePathParser&);
};

/**
 * Parses and formats Unix-style file system path names
 */
class CORE_API CoreUnixPathParser : public CorePathParser {
public:
	/** @group Construction, destruction, and assignment */
	CoreUnixPathParser();
	virtual ~CoreUnixPathParser();

	/** @group Parsing methods */
	virtual CoreCompoundName parsePath (const HString& name) const;	
	virtual HString formatPath(const CoreCompoundName& name) const;

private:
	// Internal functions and data...
	enum { kUnixPathSeparator = '/' };
};

/**
 *	Parses and formats Windows-style file system path names
 */
class CORE_API CoreWindowsPathParser : public CorePathParser {
public:
	/** @group Construction, destruction, and assignment */
	CoreWindowsPathParser();
	virtual ~CoreWindowsPathParser();

	/** @group Parsing methods */
	virtual CoreCompoundName parsePath(const HString& name) const;
	virtual HString formatPath(const CoreCompoundName& name) const;

private:
};

/**
 *	Parses and formats UI-style file system path names "C: > Users > Jonathan"
 */
class CORE_API CoreUIPathFormatter : public CorePathParser {
public:
	/** @group Construction, destruction, and assignment */
	CoreUIPathFormatter();
	virtual ~CoreUIPathFormatter();

	/** @group Parsing methods */
	virtual CoreCompoundName parsePath(const HString& name) const;
	virtual HString formatPath(const CoreCompoundName& name) const;

private:
};
#endif // CORECOMPOUNDNAME_INCLUDED
