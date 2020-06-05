/////////////////////////////////////////
// HostException.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef HOSTEXCEPTION_INCLUDED
#define HOSTEXCEPTION_INCLUDED

#include "Common.h"
#include "Host.h"

/// <summary>
/// HostException - describes exceptions specific to the underlying operating system
/// </summary>
class HOST_API HostException
{
public :

   // Constructors 
   HostException (HUint errorCode, 
                  const HChar * fileName, HUint lineNumber);

   HostException (const HostException& copyMe);

   virtual  
   ~HostException (void);
   
   // Attributes
   HUint errorCode () const;

   HString sourceFilename () const;

   HUint sourceLineNumber () const;

   HString formattedAsString() const;

   // Comparison Operators
   bool operator== (const HostException& rhs) const;
      
   bool operator!= (const HostException& rhs) const;

   // Operations
   HostException& operator= (const HostException& copyMe);

protected :
   
private :
   void 
   logException (HUint errorCode, 
                 const HChar *fileName, HUint lineNumber, HString& formattedOutput);

   HUint    m_lineNumber;
   HUint    m_errorCode;
   HString  m_fileName;
   HString  m_formattedOutput;
};

/// <summary>
/// HostExceptionLogger - logs exceptions to the screen.
/// TODO: Extend to log to system event log
/// </summary>
class HOST_API HostExceptionLogger
{
public :

   HostExceptionLogger ();
   virtual ~HostExceptionLogger (void);
   
   // Operations
   static void initialise();

   static void terminate();

   void logException (HUint code,
                 const HString& file,
                 HUint line,
                 const HString& desc);

   static void logAssertionFailure (const HString& expr,
                        const HString& file,
                        HUint line);

protected :
   
private :

   // Constructors 
   HostExceptionLogger (const HostExceptionLogger& copyMe);
   HostExceptionLogger& operator= (const HostExceptionLogger& copyMe);

   static void writeAlways (const HString& event);
};

#define THROW(e, code) throw e(code, H_TEXT(__FILE__), __LINE__)

#endif // MHOSTEXCEPTION_INCLUDED
