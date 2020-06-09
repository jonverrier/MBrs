//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "Common.h"
#include <collection.h>
#include <ppltasks.h>
#include "App.xaml.h"
#include "MainPage.xaml.h"

using namespace UI;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace concurrency;

MainPage::MainPage() 
//   : m_pModel (COMMON_NEW CoreImageListModel())
{
	InitializeComponent();
}

#if 0
void setPath(std::shared_ptr< CoreImageListModel> pModel, CoreCommandProcessor& processor, const HString& path)
{
   std::shared_ptr<CoreCommand> pCmd (COMMON_NEW CoreChangeDirectoryCommand(path, pModel->path(), pModel));
   processor.adoptAndDo(pCmd);
}


HString getPath(std::shared_ptr< CoreImageListModel> pModel, CoreCommandProcessor& processor)
{
   HString pathIn = pModel->path();

   if (pathIn.length() == 0)
   {
      //pathIn = CoreFileSystemEntity::loadImageDirectory();
      setPath(pModel, processor, pathIn);
   }

   return pathIn;
}
#endif
void UI::MainPage::onLoad(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
#if 0
   HString pathIn = getPath(m_pModel, m_processor);
   HChar* p = const_cast<HChar*> (pathIn.c_str());
   
   Platform::String^ newValue = ref new Platform::String (p);
#endif

   directoryPath->Text = L"Test";
}


void UI::MainPage::changeDirectory(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
   FolderPicker^ folderPicker = ref new FolderPicker();

   folderPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
   folderPicker->FileTypeFilter->Append(".jpg");

   auto _this = this;
#if 0
   std::shared_ptr< CoreImageListModel> _pModel = m_pModel;
   auto* _pProcessor = &m_processor;
#endif

   create_task(folderPicker->PickSingleFolderAsync()).then([_this](StorageFolder^ folder)
      {
         if (folder)
         {
#if 0
            setPath (_pModel, *_pProcessor, folder->Path->Begin());
#endif
            _this->directoryPath->Text = folder->Path;
         }
      });
}