#include "pch.h"
#include "CoreImageFile.h"

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
    ImageView::ImageView(hstring path, hstring fileName) 
       : m_path (path), m_fileName (fileName)
    {
    }

    hstring ImageView::path() const
    {
       return m_path;
    }

    hstring ImageView::fileName() const
    {
       return m_fileName;
    }

    TagCheckbox::TagCheckbox(hstring name, Windows::Foundation::IReference<bool> isUsed)
       : m_name(name), m_isUsed(isUsed)
    {
       m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"isUsed" });
    }

    hstring TagCheckbox::name() const
    {
       return m_name;
    }

    Windows::Foundation::IReference<bool> TagCheckbox::isUsed() const
    {
       return m_isUsed;
    }  

    winrt::event_token TagCheckbox::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
       return m_propertyChanged.add(handler);
    }

    void TagCheckbox::PropertyChanged(winrt::event_token const& token) noexcept
    {
       m_propertyChanged.remove(token);
    }

    Page::Page()
       : m_pDesktop(nullptr), m_pModel (nullptr), m_pCommandProcessor (nullptr), 
         m_uiPeopleTags (nullptr), m_uiPlacesTags (nullptr), m_uiTimesTags (nullptr),
         m_storedPeopleTags(CoreCategoryKeywords::peopleKey()),
         m_storedPlacesTags(CoreCategoryKeywords::placesKey()),
         m_storedTimesTags(CoreCategoryKeywords::timesKey()),
         m_personContext (), m_placeContext (), m_timeContext (),
         m_uiImageTags (nullptr)
    {
       InitializeComponent();

       m_pModel.reset (COMMON_NEW CoreImageListModel());
       m_pCommandProcessor.reset (COMMON_NEW CoreCommandProcessor (m_pModel));

       m_uiPeopleTags = winrt::single_threaded_observable_vector<MbrsUI::TagCheckbox>();
       m_uiPlacesTags = winrt::single_threaded_observable_vector<MbrsUI::TagCheckbox>();
       m_uiTimesTags = winrt::single_threaded_observable_vector<MbrsUI::TagCheckbox>();
       m_uiImageTags = winrt::single_threaded_observable_vector<MbrsUI::TagCheckbox>();

       m_uiImages = winrt::single_threaded_observable_vector<MbrsUI::ImageView>();
    }

    void Page::setDesktopCallback(uint64_t p)
    {
       m_pDesktop = reinterpret_cast<DesktopCallback *>(p);
    }

    CoreSubjectTagCounter makeImageTagsFromSelection(const std::shared_ptr<CoreImageListModel>& pModel, const std::list<HString>& selectedPaths)
    {
       CoreSubjectTagCounter counter;
       for (auto path : selectedPaths)
       {
          const std::list<CoreImageFile>& images = pModel->images();

          for (auto image : images)
          {
             if (image.path() == path)
                counter.addTags(image.subjectTags());
          }
       }

       return counter;
    }

    void buildViewTagsImpl(const std::shared_ptr<CoreImageListModel>& pModel, const std::list<HString>& selectedPaths, 
                           const CoreCategoryKeywords& defaults,
                           winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& uiTags)
    {
       // find the selected images & build a dictionary of tags
       CoreSubjectTagCounter counter = makeImageTagsFromSelection(pModel, selectedPaths);

       uiTags.Clear();

       for (auto key : defaults.keywords())
       {
          auto whereUsed = counter.countOf(key);

          auto cb = winrt::make<MbrsUI::implementation::TagCheckbox>(key.c_str(),
                                                                     whereUsed == CoreSubjectTagCounter::EUsed::kAll ? Windows::Foundation::IReference<bool>(true)
                                                                    : whereUsed == CoreSubjectTagCounter::EUsed::kNone ? Windows::Foundation::IReference<bool>(false) :
                                                                      nullptr);
          uiTags.Append(cb);
       }
    }

    void setupTagsImpl(const std::shared_ptr<CoreImageListModel>& pModel, const std::list<HString>& selectedPaths, 
                   const CoreCategoryKeywords& stored,
                   winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& view,
                   winrt::Windows::UI::Xaml::Controls::ListView& list,
                   Windows::UI::Xaml::Input::RightTappedEventHandler const& handler,
                   winrt::Windows::UI::Xaml::Controls::Button& btn)
    {
       // add keywords to the View, then connect to the list on screen
       buildViewTagsImpl(pModel, selectedPaths, stored, view);
       list.ItemsSource(view);

       // Tag lists are disabled as there is initially no selection to apply tag changes to
       list.IsEnabled(false);

       list.RightTapped(handler);

       // All the 'addXXX' buttons are initially disabled because there are no changes in the text box
       btn.IsEnabled(false);
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

    void onRemoveTagImpl(HString& context, CoreCategoryKeywords& storedTags, 
       winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox> uiTags)
    {

       if (context.size() == 0)
          return;

       if (storedTags.hasKeyword(context))
       {
          storedTags.removeKeyword(context);
       }
       for (auto i = 0u; i < uiTags.Size(); i++)
       {
          if (uiTags.GetAt(i).name().c_str() == context)
          {
             uiTags.RemoveAt(i);
             break;
          }
       }
       context.clear();
    }

    void onAddTagImpl(winrt::Windows::UI::Xaml::Controls::TextBox& textBox,
       CoreCategoryKeywords& storedTags,
       winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& uiTags)
    {
       HString keyword(textBox.Text().c_str());
       if (!storedTags.hasKeyword(keyword))
       {
          storedTags.addKeyword(keyword); // Save to storage

          // Add new tag to the UI - it is not used, as it is new
          auto cb = winrt::make<MbrsUI::implementation::TagCheckbox>(keyword.c_str(), Windows::Foundation::IReference<bool>(false));
          uiTags.Append(cb); 
          textBox.Text(H_TEXT("")); // Clear the field
       }
    }

    void Page::onPersonTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);

       onRightTapImpl(e, m_personContext);
    }

    void setupImagesImpl(std::shared_ptr<CoreImageListModel>& pModel,
       winrt::Windows::Foundation::Collections::IObservableVector <MbrsUI::ImageView>& view,
       winrt::Windows::UI::Xaml::Controls::GridView& grid)
    {
       view.Clear();

       for (auto image : pModel->images())
       {
          auto imageView = winrt::make<MbrsUI::implementation::ImageView>(image.path().c_str(), image.filename().c_str());
          view.Append(imageView);
       }

       // Connect the UI grid to data
       grid.ItemsSource (view);

       // Select no images initially
       winrt::Windows::UI::Xaml::Data::ItemIndexRange range(0, 0);
       grid.SelectRange(range);
    }

    void setupImageTagsImpl(winrt::Windows::Foundation::Collections::IObservableVector <MbrsUI::TagCheckbox>& view,
                            winrt::Windows::UI::Xaml::Controls::StackPanel& panel,
                            winrt::Windows::UI::Xaml::Controls::Border& border,
                            winrt::Windows::UI::Xaml::Controls::ListView& list)
    {
       // 'Other' tags initially collapsed as there is no selection
       panel.Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);
       border.Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);

       // Connect the UI grid to data
       list.ItemsSource(view);
    }

    void refreshTagsImpl(const std::shared_ptr<CoreImageListModel>& pModel, const std::list<HString>& selectedPaths,
                         winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& peopleTags,
                         winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& placeTags,
                         winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& timeTags,
                         winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>& imageTags)
    {
       // find the selected images & build a dictionary of tags
       CoreSubjectTagCounter counter = makeImageTagsFromSelection(pModel, selectedPaths);

       // Build the list of 'extra' tags 
       imageTags.Clear();
       std::vector<winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox> > uiTagLists(3);
       uiTagLists[0] = peopleTags;
       uiTagLists[1] = placeTags;
       uiTagLists[2] = timeTags;

       for (auto tag : counter.tags())
       {
          auto whereUsed = counter.countOf(tag.first);

          // Iterate through the tag lists, then the tags in each list
          HUint index;
          bool found = false;

          for (auto list : uiTagLists)
          {
             for (index = 0; index < list.Size(); index++)
             {
                MbrsUI::TagCheckbox item = list.GetAt(index);
                if (tag.first == item.name().c_str())
                {
                   found = true;
                   auto ref = (whereUsed == CoreSubjectTagCounter::EUsed::kAll ? Windows::Foundation::IReference<bool>(true)
                              : nullptr);
                   auto value = winrt::make<MbrsUI::implementation::TagCheckbox>(item.name(),
                                                                                 ref);
                   list.SetAt(index, value);
                }
                else
                {
                   auto ref = Windows::Foundation::IReference<bool>(false);
                   auto value = winrt::make<MbrsUI::implementation::TagCheckbox>(item.name(),
                                                                                 ref);
                   list.SetAt(index, value);
                }
             }
          }

          // Add an 'other' entry if not found. Can only be kAll (true) or KSome (null), not kNone (false)
          if (!found)
          {
             auto cb = winrt::make<MbrsUI::implementation::TagCheckbox>(tag.first.c_str(), whereUsed == CoreSubjectTagCounter::EUsed::kAll ? 
                                                                                           Windows::Foundation::IReference<bool>(true) :
                                                                                           nullptr);
             imageTags.Append(cb);
          }
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

          // Do common setup across the three Tag views
          winrt::Windows::UI::Xaml::Controls::ListView peopleList = peopleTags();
          winrt::Windows::UI::Xaml::Controls::ListView placeList = placeTags();
          winrt::Windows::UI::Xaml::Controls::ListView timeList = timeTags();

          winrt::Windows::UI::Xaml::Controls::Button addPerson = addPersonTagButton();
          winrt::Windows::UI::Xaml::Controls::Button addPlace = addPlaceTagButton();
          winrt::Windows::UI::Xaml::Controls::Button addTime = addTimeTagButton();

          std::list<HString> emptySelection;
          setupTagsImpl(m_pModel, emptySelection, m_storedPeopleTags, m_uiPeopleTags, peopleList, { this, &Page::onPersonTagRightTap }, addPerson);
          setupTagsImpl(m_pModel, emptySelection, m_storedPlacesTags, m_uiPlacesTags, placeList, { this, &Page::onPlaceTagRightTap }, addPlace);
          setupTagsImpl(m_pModel, emptySelection, m_storedTimesTags, m_uiTimesTags, timeList, { this, &Page::onTimeTagRightTap }, addTime);

          // Connect the UI grid to data
          winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
          setupImagesImpl(m_pModel, m_uiImages, grid);

          // Set up the 'other tags on the image' panel
          winrt::Windows::UI::Xaml::Controls::StackPanel imageTagsPanel = imageOtherTagsPanel();
          winrt::Windows::UI::Xaml::Controls::ListView imageTagsList = imageOtherTagsList();
          winrt::Windows::UI::Xaml::Controls::Border imageTagsBorder = imageOtherTagsBorder();
          setupImageTagsImpl(m_uiImageTags, imageTagsPanel, imageTagsBorder, imageTagsList);
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

             winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
             setupImagesImpl(m_pModel, m_uiImages, grid);
          }
       }
    }

    void Page::onImageSelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       bool enable = false;

       winrt::Windows::Foundation::Collections::IVector<IInspectable> selectedItems = imageGrid().SelectedItems();
       if (selectedItems.Size() > 0)
          enable = true;

       std::list<HString> selectedPaths;
       for (auto image : selectedItems)
       {
          selectedPaths.push_back(image.as<IImageView>().path().c_str());
       }

       peopleTags().IsEnabled(enable);
       placeTags().IsEnabled(enable);
       timeTags().IsEnabled(enable);

       refreshTagsImpl (m_pModel, selectedPaths, m_uiPeopleTags, m_uiPlacesTags, m_uiTimesTags, m_uiImageTags);

       imageOtherTagsPanel().Visibility (enable ? winrt::Windows::UI::Xaml::Visibility::Visible : winrt::Windows::UI::Xaml::Visibility::Collapsed);
       imageOtherTagsBorder().Visibility(enable ? winrt::Windows::UI::Xaml::Visibility::Visible : winrt::Windows::UI::Xaml::Visibility::Collapsed);
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

