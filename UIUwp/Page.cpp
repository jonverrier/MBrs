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
         m_peopleTags (nullptr), m_placeTags (nullptr), m_timeTags (nullptr)
    {
       InitializeComponent();
       m_pModel.reset (COMMON_NEW CoreImageListModel());
       m_pCommandProcessor.reset (COMMON_NEW CoreCommandProcessor (m_pModel));

       m_peopleTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_placeTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_timeTags = winrt::single_threaded_observable_vector<winrt::hstring>();

       m_peopleTags.Append((H_TEXT("Jonathan")));
       m_peopleTags.Append((H_TEXT("Clarissa")));
       m_peopleTags.Append((H_TEXT("Harold")));

       m_placeTags.Append((H_TEXT("London")));
       m_placeTags.Append((H_TEXT("New York")));
       m_placeTags.Append((H_TEXT("Paris")));

       m_timeTags.Append((H_TEXT("Skiing")));
       m_timeTags.Append((H_TEXT("CrossFit")));
       m_timeTags.Append((H_TEXT("Summer Holiday")));
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

          this->people().ItemsSource(m_peopleTags);
          this->places().ItemsSource(m_placeTags);
          this->times().ItemsSource(m_timeTags);

          // All the 'addXXX' buttons are initially disabled because there are no changes in the text box
          this->addPersonButton().IsEnabled(false);
          this->addPlaceButton().IsEnabled(false);
          this->addTimeButton().IsEnabled(false);

          // Select no images, and then the tag lists are disabled as there is no selection to apply tag changes to
          winrt::Windows::UI::Xaml::Data::ItemIndexRange range (0, 0);
          this->imageGrid().SelectRange(range);

          this->people().IsEnabled(false);
          this->places().IsEnabled(false);
          this->times().IsEnabled(false);
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

             path = m_pModel->pathAsUserString();
             this->directoryPath().Text(path);
          }
       }
    }

    void Page::onImageSelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->imageGrid().SelectedItems().Size() > 0)
          enable = true;

       this->people().IsEnabled(enable);
       this->places().IsEnabled(enable);
       this->times().IsEnabled(enable);
    }

    void Page::onNewPersonChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->addPersonText().Text().size() > 0)
          enable = true;

       this->addPersonButton().IsEnabled(enable);
    }

    void Page::onRemovePersonTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onAddPerson(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onNewPlaceChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->addPlaceText().Text().size() > 0)
          enable = true;

       this->addPlaceButton().IsEnabled(enable);
    }

    void Page::onAddPlace(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }

    void Page::onNewTimeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->addTimeText().Text().size() > 0)
          enable = true;

       this->addTimeButton().IsEnabled(enable);
    }

    void Page::onAddTime(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
    }
}





