/////////////////////////////////////////
// HostInternal.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#define COMMON_STRING_BUFFER_SIZE 512

#define TESTEQUALTHROWLASTERROR(code, err) \
   if (code == err) throw HostException (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError()), H_TEXT(__FILE__), __LINE__)

#define TESTNOTEQUALTHROWLASTERROR(code, noerr) \
   if (code != noerr) throw HostException (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError()), H_TEXT(__FILE__), __LINE__)


#define TESTEXPRESSIONTHROWLASTERROR(fExprResult) \
   if (fExprResult) throw HostException (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ::GetLastError()), H_TEXT(__FILE__), __LINE__)

#define TESTEXPRESSIONTHROWERROR(fExprResult, uError) \
   if (fExprResult) throw HostException (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, uError), H_TEXT(__FILE__), __LINE__)


