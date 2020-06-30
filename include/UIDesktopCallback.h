#ifndef UIDESKTOPCALLBACK_INCLUDED
#define UIDESKTOPCALLBACK_INCLUDED

#include "Common.h"
class DesktopCallback
{
public:
   virtual bool chooseFolder(HString& newPath) = 0;
};

#endif //UIDESKTOPCALLBACK_INCLUDED