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
         m_uiPeopleTags (nullptr), m_uiPlacesTags (nullptr), m_uiTimesTags (nullptr),
         m_storedPeopleTags(CoreCategoryKeywords::peopleKey()),
         m_storedPlacesTags(CoreCategoryKeywords::placesKey()),
         m_storedTimesTags(CoreCategoryKeywords::timesKey()),
         m_personContext (), m_placeContext ()
    {
       InitializeComponent();
       m_pModel.reset (COMMON_NEW CoreImageListModel());
       m_pCommandProcessor.reset (COMMON_NEW CoreCommandProcessor (m_pModel));

       m_uiPeopleTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_uiPlacesTags = winrt::single_threaded_observable_vector<winrt::hstring>();
       m_uiTimesTags = winrt::single_threaded_observable_vector<winrt::hstring>();

       m_uiTimesTags.Append((H_TEXT("Skiing")));
       m_uiTimesTags.Append((H_TEXT("CrossFit")));
       m_uiTimesTags.Append((H_TEXT("Summer Holiday")));
    }

    void Page::setDesktopCallback(uint64_t p)
    {
       m_pDesktop = reinterpret_cast<DesktopCallback *>(p);
    }

    void buildUITagDataImpl(winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring>& peopleTags, const CoreCategoryKeywords& peopleDefaults)
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
          buildUITagDataImpl(m_uiPeopleTags, m_storedPeopleTags);
          buildUITagDataImpl(m_uiPlacesTags, m_storedPlacesTags);
          buildUITagDataImpl(m_uiTimesTags, m_storedTimesTags);

          this->peopleTags().ItemsSource(m_uiPeopleTags);
          this->placeTags().ItemsSource(m_uiPlacesTags);
          this->timeTags().ItemsSource(m_uiTimesTags);

          // All the 'addXXX' buttons are initially disabled because there are no changes in the text box
          this->addPersonTagButton().IsEnabled(false);
          this->addPlaceTagButton().IsEnabled(false);
          this->addTimeTagButton().IsEnabled(false);

          // Select no images, and then the tag lists are disabled as there is no selection to apply tag changes to
          winrt::Windows::UI::Xaml::Data::ItemIndexRange range (0, 0);
          this->imageGrid().SelectRange(range);

          this->peopleTags().IsEnabled(false);
          this->placeTags().IsEnabled(false);
          this->timeTags().IsEnabled(false);

          this->peopleTags().RightTapped({ this, &Page::onPersonTagRightTap });
          this->placeTags().RightTapped({ this, &Page::onPlaceTagRightTap });
          this->timeTags().RightTapped({ this, &Page::onTimeTagRightTap });
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

       this->peopleTags().IsEnabled(enable);
       this->placeTags().IsEnabled(enable);
       this->timeTags().IsEnabled(enable);
    }

    void Page::onNewPersonTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->newPersonTag().Text().size() > 0)
          enable = true;

       this->addPersonTagButton().IsEnabled(enable);
    }

    void onRightTapImpl(winrt::Windows::UI::Xaml::RoutedEventArgs const& e, HString& context)
    {
       auto ctx = e.OriginalSource().as<FrameworkElement>().DataContext();
       if (ctx)
       {
          winrt::hstring uiValue = unbox_value_or<hstring>(ctx, L""); // Returns L"" if object is not a boxed string.
          HString value = uiValue.c_str();
          context = value;
       }
    }

    void onRemoveTagImpl (HString& context, CoreCategoryKeywords& storedTags, winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> uiTags)
    {

       if (context.size() == 0)
          return;

       if (storedTags.hasKeyword(context))
       {
          storedTags.removeKeyword(context);
       }
       for (auto i = 0u; i < uiTags.Size(); i++)
       {
          if (uiTags.GetAt(i).c_str() == context)
          {
             uiTags.RemoveAt(i);
             break;
          }
       }
       context.clear();
    }

    void onAddTagImpl (winrt::Windows::UI::Xaml::Controls::TextBox& textBox,
                       CoreCategoryKeywords& storedTags, 
                       winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring>& uiTags)
    {
       HString keyword(textBox.Text().c_str());
       if (!storedTags.hasKeyword(keyword))
       {
          storedTags.addKeyword(keyword); // Save to storage
          uiTags.Append(textBox.Text()); // Add new tag to the UI
          textBox.Text(H_TEXT("")); // Clear the field
       }
    }

    void Page::onPersonTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);

       onRightTapImpl(e, m_personContext);
    }

    void Page::onRemovePersonTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       onRemoveTagImpl(m_personContext, m_storedPeopleTags, m_uiPeopleTags);
    }

    void Page::onAddPersonTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       winrt::Windows::UI::Xaml::Controls::TextBox tb = newPersonTag();
       onAddTagImpl(tb, m_storedPeopleTags, m_uiPeopleTags);
    }

    void Page::onNewPlaceTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->newPlaceTag().Text().size() > 0)
          enable = true;

       this->addPlaceTagButton().IsEnabled(enable);
    }

    void Page::onPlaceTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);

       onRightTapImpl(e, m_placeContext);
    }

    void Page::onRemovePlaceTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       onRemoveTagImpl(m_placeContext, m_storedPlacesTags, m_uiPlacesTags);
    }

    void Page::onAddPlaceTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       winrt::Windows::UI::Xaml::Controls::TextBox tb = newPlaceTag();
       onAddTagImpl(tb, m_storedPlacesTags, m_uiPlacesTags);
    }

    void Page::onNewTimeTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       if (this->newTimeTag().Text().size() > 0)
          enable = true;

       this->addTimeTagButton().IsEnabled(enable);
    }

    void Page::onAddTimeTag (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       winrt::Windows::UI::Xaml::Controls::TextBox tb = newTimeTag();
       onAddTagImpl (tb, m_storedTimesTags, m_uiTimesTags);
    }

    void Page::onTimeTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);

       onRightTapImpl(e, m_timeContext);
    }

    void Page::onRemoveTimeTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       onRemoveTagImpl(m_timeContext, m_storedTimesTags, m_uiTimesTags);
    }
}

