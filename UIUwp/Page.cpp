#include "pch.h"
#include "Page.h"
#if __has_include("Page.g.cpp")
#include "Page.g.cpp"
#endif

#include <thread>
#include <future>
#include <ppltasks.h>

typedef DWORD SHGDNF;
#include <shobjidl.h>

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


    fire_and_forget Page::changeDirectory(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       if (m_pModel)
       {
           auto lifetime = get_strong();

           HWND hwnd;

           // set up a file picker to pick a directory wit no name restrictions
           auto picker = winrt::Windows::Storage::Pickers::FolderPicker();
           //picker.as<IInitializeWithWindow>()->Initialize(hwnd);

           picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
           picker.FileTypeFilter().Append(H_TEXT("*"));

           // set up an asynch operations
           StorageFolder folder = co_await  picker.PickSingleFolderAsync();

           HString path(folder.Path().c_str());
           m_pModel->setPath(path);

           path = m_pModel->pathAsUserString();
           this->directoryPath().Text(path);
             
          /*
           FolderPicker picker;

           picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
           picker.FileTypeFilter().Append(H_TEXT("*"));

           auto folder { co_await picker.PickSingleFolderAsync() };

           HString path(folder.Path().c_str());
           m_pModel->setPath(path);

           path = m_pModel->pathAsUserString();
           this->directoryPath().Text(path);
           */
       }
    }
}





