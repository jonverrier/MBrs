#ifndef UIDESKTOPCALLBACK_INCLUDED
#define UIDESKTOPCALLBACK_INCLUDED

#include "Common.h"
class DesktopCallback
{
public:
   virtual bool chooseFolder(HString& newPath) = 0;

   enum class ESaveMode { kNone, kPending, kSaving, kSaved };

   virtual void setSaveFlag(ESaveMode mode) = 0;
};

#endif //UIDESKTOPCALLBACK_INCLUDED