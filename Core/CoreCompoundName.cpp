/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "CorePrecompile.h"
#include "CoreCompoundName.h"

using namespace std;

static const HUint g_vectorSizeIncrement = 10;

static void manageSpace (vector<HString>& components, HUint index)
{
	if (components.capacity() <= index)
		components.reserve(components.capacity() + g_vectorSizeIncrement);
	components.resize(index + 1);
}

CoreCompoundName::CoreCompoundName ()
	: m_nameComponents ( ), m_isFullPath (false)
{
	m_nameComponents.reserve(g_vectorSizeIncrement);
}

CoreCompoundName::CoreCompoundName(const std::vector<HString>& components, bool isFullPath)
  : m_nameComponents (components), m_isFullPath (isFullPath)
{
}

CoreCompoundName::CoreCompoundName(const HString& fullPath,
					  						  const CorePathParser& aParser)
	: m_nameComponents( 0 )
{
	operator = (aParser.parsePath(fullPath));
}

CoreCompoundName::CoreCompoundName (const CoreCompoundName& copy)
	: m_nameComponents( copy.m_nameComponents), m_isFullPath(copy.m_isFullPath)
{
}

CoreCompoundName&
CoreCompoundName::operator= (const CoreCompoundName& copyMe)
{
	m_nameComponents = copyMe.m_nameComponents;
	m_isFullPath = copyMe.m_isFullPath;

	return *this;
}

CoreCompoundName::~CoreCompoundName ()
{
}


bool
CoreCompoundName::operator== (const CoreCompoundName& other) const
{
	return m_nameComponents == other.m_nameComponents && m_isFullPath == other.m_isFullPath;
}

bool
CoreCompoundName::operator!= (const CoreCompoundName& other) const
{
	return m_nameComponents != other.m_nameComponents || m_isFullPath != other.m_isFullPath;
}

bool
CoreCompoundName::isFullPath() const
{
	return m_isFullPath;
}

HUint
CoreCompoundName::numberOfComponents () const
{
	return static_cast<HUint>(m_nameComponents.size());
}

bool
CoreCompoundName::hasComponentAt(HUint index) const
{
	return m_nameComponents.size() >= index;
}

HString
CoreCompoundName::componentAt (HUint index) const
{
	return m_nameComponents[index];
}


//--------------------------------------------------------------------------
// CorePathParser implementation....
// 

CorePathParser::~CorePathParser ()
{
}

CorePathParser::CorePathParser ()
{
}

CorePathParser::CorePathParser (const CorePathParser &)
{
}

//--------------------------------------------------------------------------
// CoreUnixPathParser implementation....
// 
CoreUnixPathParser::~CoreUnixPathParser()
{
}

CoreUnixPathParser::CoreUnixPathParser()
{
}

CoreCompoundName
CoreUnixPathParser::parsePath(const HString& path) const
{
	std::vector<HString> pathComponents;
	HSize length = path.length(), pos = 0, next = 0;
	HUint componentIndex = 0;
	bool isFullPath = (length > 0) && (path[0] == '/');

	// Chop the path into components by searching for '/' separator characters
	pos = 0;
	while ((next = path.find('/', pos)) != length) {
		// Found one.
		HString component = path.substr(pos, next - pos);
		if (component.length() && component != HString(H_TEXT("."))) 
		{
			manageSpace(pathComponents, componentIndex);
			pathComponents[componentIndex] = component;
			componentIndex++;
		}
		pos = next + 1;
	}

	if (pos < length) 
	{
		// There's a component after the last '/' so append it too
		manageSpace(pathComponents, componentIndex);
		pathComponents[componentIndex] = path.substr(pos);
		componentIndex++;
	}
	return CoreCompoundName (pathComponents, isFullPath);
}

//--------------------------------------------------------------------------
// formatPath
//	Append each component, separated by '/' characters
// 
HString
CoreUnixPathParser::formatPath (const CoreCompoundName& aPath) const
{
	HString newPath;
	unsigned long componentCount = aPath.numberOfComponents(), index = 0;

	// Pre-pend a root 
	if (aPath.isFullPath()) {
		newPath[0] = '/';
	}
	for (unsigned long count = 0; count < componentCount; count++)
	{
		newPath.append(aPath.componentAt(count));
		if (count != componentCount - 1)
			newPath.append(H_TEXT("/"));
	}
	return newPath;
}


//--------------------------------------------------------------------------
// CoreWindowsPathParser implementation....
// 

//--------------------------------------------------------------------------
// Constructors, etc.....
// 
CoreWindowsPathParser::CoreWindowsPathParser()
{
}

CoreWindowsPathParser::~CoreWindowsPathParser()
{
}

//--------------------------------------------------------------------------
// parsePath
//	Convert a Windows-style path name string into an CoreCompoundName.  
//	The simple case is easy: just search for \ characters.
//	But we also have to handle UNC names like \\host\share\dir\file.txt
//	and paths containing drive letters, which complicate matters quite a bit
// 
CoreCompoundName
CoreWindowsPathParser::parsePath(const HString& path) const
{
	std::vector<HString> pathComponents;
	HSize length = path.length();
	HSize pos = 0, next = 0;
	HUint componentIndex = 0;
	bool isFullPath = false;

	if (path.find(H_TEXT(":\\"), 0) == 1) 
	{			
		// Does it start with a drive letter?
		isFullPath = true;
		manageSpace(pathComponents, componentIndex);
		pathComponents[componentIndex] = path.substr(0, 2);
		componentIndex++;
		pos += 3;
	}
	else 
	if (path.find(H_TEXT("\\\\"), 0) == 0) 
	{	
		// Is it a UNC-style path?
		isFullPath = true;
		// Skip the initial two slashes, then search for two more, so we
		// extract the \\host\share part of the path
		pos += 1;
		for (int i = 0; i < 2 && pos < length; i++) 
		{
			pos = path.find(H_TEXT('\\'), pos + 1);
		}

		if (pos < length) 
		{
			// Swallow just the \\host\share part of the path
			if (pathComponents.capacity() <= componentIndex)
				pathComponents.reserve(pathComponents.capacity() + g_vectorSizeIncrement);
			pathComponents[componentIndex] = path.substr(0, pos);
			componentIndex++;
			pos += 1;
		}
		else 
		{
			// Swallow the whole thing
			manageSpace(pathComponents, componentIndex);
			pathComponents[componentIndex] = path;
			componentIndex++;
			pos = length;
		}
	}
	else 
   if (path.find(H_TEXT("\\"), 0) == 0) 
	{		
		// "full" path without drive letter
		pos++;
		isFullPath = true;
	}

	// Now, finally, run through the rest of the path and chop it
	// into components by looking for \ characters.

	while ((next = path.find('\\', pos)) != HString::npos)
	{
		HString component = path.substr(pos, next - pos);
		if (component.length() && component != HString (H_TEXT("."))) 
		{
			manageSpace(pathComponents, componentIndex);
			pathComponents[componentIndex] = component;
			componentIndex++;
		}
		pos = next + 1;
	}
	if (pos < length) 
	{
		manageSpace(pathComponents, componentIndex);
		pathComponents[componentIndex] = path.substr(pos);
		componentIndex++;
	}

	return CoreCompoundName(pathComponents, isFullPath);
}

//--------------------------------------------------------------------------
// formatPath
//	Append each component, separated by '\' characters
// 
HString
CoreWindowsPathParser::formatPath(const CoreCompoundName& aPath) const
{
	HString newPath;
	HUint componentCount = aPath.numberOfComponents();

	// If this is a full path but the first component is not a drive or host,
	// we need to slap an extra slash on the host path

	int start = 0;
	if (aPath.isFullPath()) 
	{
		HString first = aPath.componentAt(start);
		if (!(first.length() > 0 && first.at(1) == H_TEXT(':') || first.substr(0, 2) == H_TEXT("\\\\"))) 
		{
			newPath += H_TEXT('\\');
		}
	}

	for (unsigned long count = start; count < componentCount; count++) 
	{
		newPath += aPath.componentAt(count);
		if (count < componentCount - 1)
			newPath += H_TEXT('\\');
	}

	// If it's a naked drive letter, tack on a \ to make it a legal path
	if (newPath.length() == 2 && newPath.at(1) == ':') 
	{
		newPath += H_TEXT("\\");
	}

	return newPath;
}

//--------------------------------------------------------------------------
// CoreUIPathFormatter implementation....
// 

//--------------------------------------------------------------------------
// Constructors, etc.....
// 
CoreUIPathFormatter::CoreUIPathFormatter()
{
}

CoreUIPathFormatter::~CoreUIPathFormatter()
{
}

//--------------------------------------------------------------------------
// parsePath
//	Null implementation - these paths are only used for display
// 
CoreCompoundName
CoreUIPathFormatter::parsePath(const HString& path) const
{
	return CoreCompoundName();
}

//--------------------------------------------------------------------------
// formatPath
//	Append each component, separated by " > " string
// 
HString
CoreUIPathFormatter::formatPath(const CoreCompoundName& aPath) const
{
	HString newPath;
	HUint componentCount = aPath.numberOfComponents();

	int start = 0;

	for (unsigned long count = start; count < componentCount; count++)
	{
		newPath += aPath.componentAt(count);
		if (count < componentCount - 1)
			newPath += H_TEXT(" > ");
	}

	return newPath;
}



