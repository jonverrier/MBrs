#include "pch.h"

#include "StringWrapper.h"
#if __has_include("StringWrapper.g.cpp")
#include "StringWrapper.g.cpp"
#endif

#include "Page.h"
#if __has_include("Page.g.cpp")
#include "Page.g.cpp"
#endif

#include "UIDesktopCallback.h"

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
         m_uiPeopleDefaultTags (nullptr), m_placeTags (nullptr), m_timeTags (nullptr),
         m_storedPeopleDefaultTags(CoreCategoryKeywords::peopleKey()),
         m_personContext ()
    {
       InitializeComponent();
       m_pModel.reset (COMMON_NEW CoreImageListModel());
       m_pCommandProcessor.reset (COMMON_NEW CoreCommandProcessor (m_pModel));

       m_uiPeopleDefaultTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_placeTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_timeTags = winrt::single_threaded_observable_vector<winrt::hstring>();

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

    void buildTagViewData (winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring>& peopleTags, const CoreCategoryKeywords& peopleDefaults)
    {
       peopleTags.Clear();

       for (auto key : peopleDefaults.keywords())
       {
          peopleTags.Append(key);
       }
    }

    void Page::onLoad(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (m_pModel)
       {
          HString path = m_pModel->pathAsUserString();
          this->directoryPath().Text(path);

          const HString catName1 = H_TEXT("CategoryOp1");
          const HString keyName1 = H_TEXT("Key1");
          const HString keyName2 = H_TEXT("Key2");

          // add keywords
          buildTagViewData(m_uiPeopleDefaultTags, m_storedPeopleDefaultTags);

          this->peopleDefaultTags().ItemsSource(m_uiPeopleDefaultTags);
          this->places().ItemsSource(m_placeTags);
          this->times().ItemsSource(m_timeTags);

          // All the 'addXXX' buttons are initially disabled because there are no changes in the text box
          this->addPersonDefaultTagButton().IsEnabled(false);
          this->addPlaceButton().IsEnabled(false);
          this->addTimeButton().IsEnabled(false);

          // Select no images, and then the tag lists are disabled as there is no selection to apply tag changes to
          winrt::Windows::UI::Xaml::Data::ItemIndexRange range (0, 0);
          this->imageGrid().SelectRange(range);

          this->peopleDefaultTags().IsEnabled(false);
          this->places().IsEnabled(false);
          this->times().IsEnabled(false);

          this->peopleDefaultTags().RightTapped({ this, &Page::onPersonDefaultTagRightTap });
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

       this->peopleDefaultTags().IsEnabled(enable);
       this->places().IsEnabled(enable);
       this->times().IsEnabled(enable);
    }

    void Page::onNewPersonDefaultTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->newPersonDefaultTag().Text().size() > 0)
          enable = true;

       this->addPersonDefaultTagButton().IsEnabled(enable);
    }

    void Page::onPersonDefaultTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       auto os = e.OriginalSource();
       if (os)
       {
          auto fe = os.as<FrameworkElement>();
          if (fe)
          {
             auto ctx = fe.DataContext();
             if (ctx)
             {
                winrt::hstring uiValue = unbox_value_or<hstring>(ctx, L""); // Returns L"" if object is not a boxed string.
                HString value = uiValue.c_str();
                m_personContext = value;                 
             }
          }
       }
    }

    void Page::onRemovePersonDefaultTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (m_personContext.size() == 0)
          return;

       if (m_storedPeopleDefaultTags.hasKeyword(m_personContext))
       {
          m_storedPeopleDefaultTags.removeKeyword(m_personContext);
       }
       for (auto i = 0u; i < m_uiPeopleDefaultTags.Size(); i++)
       {
          if (m_uiPeopleDefaultTags.GetAt(i).c_str() == m_personContext)
          {
             m_uiPeopleDefaultTags.RemoveAt(i);
             break;
          }
       }
       m_personContext.clear();
    }

    void Page::onAddPersonDefaultTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       HString keyword = (newPersonDefaultTag().Text().c_str());
       if (!m_storedPeopleDefaultTags.hasKeyword(keyword))
       {
          m_storedPeopleDefaultTags.addKeyword(keyword); // Save to storage
          m_uiPeopleDefaultTags.Append(newPersonDefaultTag().Text()); // Add new tag to the UI
          newPersonDefaultTag().Text(H_TEXT("")); // Clear the field
       }
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

