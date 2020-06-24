#include "pch.h"

#include "StringWrapper.h"
#if __has_include("StringWrapper.g.cpp")
#include "StringWrapper.g.cpp"
#endif

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
    StringWrapper::StringWrapper(winrt::hstring const& text)
       : m_text (text)
    {
    }

    winrt::hstring StringWrapper::text()
    {
       return m_text;
    }

   void StringWrapper::text(const winrt::hstring& in)
    {
       m_text = in;
    }
   winrt::event_token StringWrapper::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
   {
      return m_propertyChanged.add(handler);
   }

   void StringWrapper::PropertyChanged(winrt::event_token const& token)
   {
      m_propertyChanged.remove(token);
   }

    Page::Page()
       : m_pDesktop(nullptr), m_pModel (nullptr), m_pCommandProcessor (nullptr), 
         m_peopleTags (nullptr)
    {
       InitializeComponent();
       m_pModel.reset (COMMON_NEW CoreImageListModel());
       m_pCommandProcessor.reset (COMMON_NEW CoreCommandProcessor (m_pModel));

       m_peopleTags = winrt::single_threaded_observable_vector<MbrsUI::StringWrapper>();

       //m_peopleTags.Append((H_TEXT("Jonathan")));
       //m_peopleTags.Append((H_TEXT("Clarissa")));
       //m_peopleTags.Append((H_TEXT("Harold")));
    }

    void Page::setDesktopCallback(uint64_t p)
    {
       m_pDesktop = reinterpret_cast<DesktopCallback *>(p);
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
           * So instead we call back into the hosting application, which is Win32
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
             std::shared_ptr<CoreSelection> pSelection(COMMON_NEW CoreSelection());

             std::shared_ptr<CoreCommand> pCmd (COMMON_NEW CoreChangeDirectoryCommand (path, m_pModel->path(), m_pModel, pSelection));

             m_pCommandProcessor->adoptAndDo(pCmd);
             // m_pModel->setPath(path);

             path = m_pModel->pathAsUserString();
             this->directoryPath().Text(path);
          }
          
          /* Windows::UI::Xaml::Controls::CheckBox checkbox;
          auto in = checkbox.Content();
          Windows::UI::Xaml::Controls::ListBoxItem item;
          item.Content(checkbox);


          this->people().Items().Append(item); */
       }
    }

    void Page::onAddPerson(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onCancelPerson(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onAddPlace(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onCancelPlace(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onAddTime(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onCancelTime(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    Windows::Foundation::Collections::IObservableVector<MbrsUI::StringWrapper> Page::peopleTags()
    {
       return m_peopleTags;
    }
}





