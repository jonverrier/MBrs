//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}

void UI::MainPage::onLoad(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
   Platform::String^ newValue(L"C > Documents > Images");

   directoryPath->Text = newValue;
}


void UI::MainPage::changeDirectory(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
   FolderPicker^ folderPicker = ref new FolderPicker();

   folderPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
   folderPicker->FileTypeFilter->Append(".jpg");

   auto _this = this;

   create_task(folderPicker->PickSingleFolderAsync()).then([_this](StorageFolder^ folder)
      {
         if (folder)
         {
            _this->directoryPath->Text = L"Picked folder: " + folder->Path;
         }
      });
}