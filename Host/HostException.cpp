/////////////////////////////////////////
// HostException.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "HostException.h"
#include "HostInternal.h"

#include <atlbase.h> 
#include <OleAuto.h>
#include <D2DErr.h>
#include <mferror.h>

//#define CODEFROMHRESULT(hr)        (((hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) ? HRESULT_CODE(hr) : (hr == S_OK ? 0 : -1))
//#define TEXTFROMHRESULT(hr)        (_com_error(hr).ErrorMessage())

///////////////////////////////////////////////////////////////////////////////
// HostException
///////////////////////////////////////////////////////////////////////////////
  
HostException::HostException (HUint errorCode, 
                              const HChar *fileName, 
                              HUint lineNumber)
   : m_errorCode (errorCode),
     m_lineNumber (lineNumber),
     m_fileName (fileName),
     m_formattedOutput ()
{
   HString extraOut;

   logException (errorCode, fileName, lineNumber, extraOut);  
   m_formattedOutput = extraOut;
}

 HostException::HostException (const HostException& copyMe)
   : m_errorCode (copyMe.m_errorCode),
     m_lineNumber (copyMe.m_lineNumber),
     m_fileName (copyMe.m_fileName),
     m_formattedOutput (copyMe.m_formattedOutput)
{
}

HostException::~HostException ()
{
}

HUint  
HostException::errorCode () const
{
   return m_errorCode;
}

HString  
HostException::formattedAsString () const
{
   return m_formattedOutput;
}

HString  
HostException::sourceFilename () const
{
   return m_fileName;
}

HUint  
HostException::sourceLineNumber () const
{
   return m_lineNumber;
}

bool 
HostException::operator== (const HostException& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return m_errorCode   == rhs.m_errorCode
      && m_formattedOutput == rhs.m_formattedOutput
      && m_fileName   == rhs.m_fileName 
      && m_lineNumber  == rhs.m_lineNumber;
}
      
bool 
HostException::operator!= (const HostException& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return m_errorCode   != rhs.m_errorCode
      || m_formattedOutput != rhs.m_formattedOutput
      || m_fileName   != rhs.m_fileName 
      || m_lineNumber  != rhs.m_lineNumber;
}

HostException& 
HostException::operator= (const HostException& copyMe)
{
   m_errorCode   = copyMe.m_errorCode; 
   m_fileName   = copyMe.m_fileName; 
   m_lineNumber  = copyMe.m_lineNumber;
   m_formattedOutput  = copyMe.m_formattedOutput;

   return *this;
}
   
void  
HostException::logException (HUint errorCode, 
                             const HChar *pszFileName, HUint lineNumber, HString& out)
{  
   HostExceptionLogger logger;

   if (HRESULT_FACILITY(errorCode) == FACILITY_WIN32) {

      TCHAR sz[COMMON_STRING_BUFFER_SIZE];
      LONG lRet = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
         NULL, errorCode, 0,
         sz,
         COMMON_STRING_BUFFER_SIZE - 1,
         NULL);

      if (lRet == 0)
         throw HostException(MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError()), H_TEXT(__FILE__), __LINE__);

      out = sz;
   }
   else
   if ((HRESULT_FACILITY(errorCode) == FACILITY_MF
      || (HRESULT_FACILITY(errorCode) == FACILITY_D2D)))
   {
      CComPtr<IErrorInfo> pErrInfo;

      ::GetErrorInfo(0, &pErrInfo);
      if (pErrInfo != (NULL))
      {
         BSTR bstr;
         pErrInfo->GetDescription (& bstr);

         out = bstr;
         ::SysFreeString(bstr);
         ::SetErrorInfo(0, pErrInfo); // This propogates error info up the call chain
      }
   } 

   logger.logException (errorCode, pszFileName, lineNumber, out);
}

///////////////////////////////////////////////////////////////////////////////
// HostExceptionLogger
///////////////////////////////////////////////////////////////////////////////
HostExceptionLogger::HostExceptionLogger ()
{
}
 
HostExceptionLogger::~HostExceptionLogger ()
{
}

void HostExceptionLogger::logException (HUint code,
                                        const HString& file,
                                        HUint line,
                                        const HString& desc)
{
   std::wstringstream sscode; 
   sscode << code;

   std::wstringstream ssline; 
   ssline << line;

   HString exString =  H_TEXT(", Error code:") + sscode.str() 
                       + H_TEXT(", Filename:") + file 
                       + H_TEXT(", LineNumber:") + ssline.str() 
                       + H_TEXT(", Description:") + desc;

   std::wcout << exString << std::endl;
   HostExceptionLogger::writeAlways (exString);
}

void 
HostExceptionLogger::logAssertionFailure (const HString& expr,
                                          const HString& file,
                                          HUint line)
{
   std::wstringstream ssline; 
   ssline << line;

   HString exString =  H_TEXT(", File:") + file 
                       + H_TEXT(", LineNumber:") + ssline.str() 
                       + H_TEXT(", Expression:") + expr;

   std::wcout << exString << std::endl;
   HostExceptionLogger::writeAlways (exString);
}

// {479A15F0-2A75-45C2-9002-B47C1C18D6D1}
static const GUID hostEventGuid = 
{ 0x479a15f0, 0x2a75, 0x45c2, { 0x90, 0x2, 0xb4, 0x7c, 0x1c, 0x18, 0xd6, 0xd1 } };

static REGHANDLE hostRegHandle = NULL;

void 
HostExceptionLogger::initialise ()
{
   // Register the ETW provider.
   EventRegister(&hostEventGuid,     // ProviderId (GUID)
                 NULL,               // Optional Callback 
                 NULL,               // Optional Callback Context
                 &hostRegHandle);    // Registration Handle
}

void 
HostExceptionLogger::terminate ()
{
   // Register the ETW provider.
   EventUnregister (hostRegHandle);  // ProviderId (GUID)
   hostRegHandle = NULL;
}

void
HostExceptionLogger::writeAlways (const HString& event)
{
   // TODO: Change this to use a manifext
   // How to trace this:
   //    logman start mysession -p {479A15F0-2A75-45C2-9002-B47C1C18D6D1} -o mytest.etl -etc
   //    logman stop mysession -ets
   //    tracerpt mytest.etl
   //    output is in dumpfile.xml
   //
   ULONG ulRet = EventWriteString (hostRegHandle, 0, 0, event.c_str());
   if (ulRet == 0)
      return;
   else
      return;
}
