#include "pch.h"
#include "Page.h"
#if __has_include("Page.g.cpp")
#include "Page.g.cpp"
#endif

#include <thread>
#include <future>
#include <ppltasks.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace concurrency;


namespace winrt::MbrsUI::implementation
{
    Page::Page()
       : m_pModel (nullptr)
    {
       InitializeComponent();
    }

    void Page::setModel(uint64_t ptrAsUint64)
    {
       m_pModel = reinterpret_cast<CoreImageListModel *>(ptrAsUint64);
    }

    void Page::onLoad(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       if (m_pModel)
       {
          HString path = m_pModel->pathAsUserString();
          this->directoryPath().Text(path);
       }
    }

    bool myfn(StorageFolder folder)
    {
       if (folder != nullptr)
       {
          /* HString path(folder.Path().c_str());
          pModel->setPath(path);

          path = pModel->pathAsUserString();
          _this->directoryPath().Text(path); */
          return true;
       }
       return false;
    }

    void Page::changeDirectory(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       if (m_pModel)
       {
           FolderPicker picker; 

           picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary); 
           picker.FileTypeFilter().Append (H_TEXT("*"));

           auto _this = this;
           auto pModel = m_pModel;

           using Task_Type = bool(StorageFolder folder);
           std::packaged_task<Task_Type> task (myfn);
           //create_task(picker.PickSingleFolderAsync()).then ([] { folder
           //   
           //   });
           //create_task(picker.PickSingleFolderAsync()).then([_this](StorageFolder folder)
           //   {
           //   });
       }
    }
}





