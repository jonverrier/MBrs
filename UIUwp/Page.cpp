#include "pch.h"
#include "Page.h"
#if __has_include("Page.g.cpp")
#include "Page.g.cpp"
#endif


using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;

namespace winrt::MbrsUI::implementation
{
    Page::Page()
       : m_pDesktop(nullptr), m_pModel (nullptr)
    {
       InitializeComponent();
    }

    void Page::setModel(uint64_t ptrAsUint64)
    {
       m_pModel = reinterpret_cast<CoreImageListModel *>(ptrAsUint64);
       // m_pCommandProcessor = COMMON_NEW CoreCommandProcessor (m_pModel);
    }

    void Page::setDesktopCallback(uint64_t ptrAsUint64)
    {
       m_pDesktop = reinterpret_cast<DesktopCallback *>(ptrAsUint64);
    }

    void Page::onLoad(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (m_pModel)
       {
          HString path = m_pModel->pathAsUserString();
          this->directoryPath().Text(path);
       }
    }

    void Page::onChangeDirectory(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (m_pModel)
       {
          /* This section does not work as Pickers cannot currently be called from Xaml islands
           auto lifetime = get_strong();

           HWND hwnd;

           // set up a file picker to pick a directory wit no name restrictions
           auto picker = winrt::Windows::Storage::Pickers::FolderPicker();
           //picker.as<IInitializeWithWindow>()->Initialize(hwnd);

           picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
           picker.FileTypeFilter().Append(H_TEXT("*"));

           // set up an asynch operations
           StorageFolder folder = co_await  picker.PickSingleFolderAsync();
           */

          HString path = m_pModel->path();
          if (m_pDesktop->chooseFolder(path))
          {
             m_pModel->setPath(path);

             path = m_pModel->pathAsUserString();
             this->directoryPath().Text(path);
          }
       }
    }
}





