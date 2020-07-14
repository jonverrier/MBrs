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
using namespace Windows::UI::Core;

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

    void TagCheckbox::isUsed(Windows::Foundation::IReference<bool> used)
    {
       m_isUsed = used;
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
       
       // Set the time for 2 seconds interval. every two seconds after a tag checkbox change we batch up all changes & save.
       // Or we save when different images are selected
       m_changeTimer.Interval(std::chrono::milliseconds{ 2000 });
       auto registrationtoken = m_changeTimer.Tick({ this, &Page::onTick });
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
          const std::list<HString> tags = pModel->tagsFor(path);
          counter.addTags(tags);
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
          auto ref = whereUsed == CoreSubjectTagCounter::EUsed::kAll ? Windows::Foundation::IReference<bool>(true)
                   : whereUsed == CoreSubjectTagCounter::EUsed::kNone ? Windows::Foundation::IReference<bool>(false) 
                   : nullptr;

          auto cb = winrt::make<MbrsUI::implementation::TagCheckbox>(key.c_str(),
                                                                     ref);
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

    void setupImagesImpl(bool first, std::shared_ptr<CoreImageListModel>& pModel,
       winrt::Windows::Foundation::Collections::IObservableVector <MbrsUI::ImageView>& view,
       winrt::Windows::UI::Xaml::Controls::GridView& grid)
    {
       view.Clear();

       for (auto image : pModel->filteredImages())
       {
          auto imageView = winrt::make<MbrsUI::implementation::ImageView>(image.path().c_str(), image.filename().c_str());
          view.Append(imageView);
       }

       if (first)
       {
          // Connect the UI grid to data
          grid.ItemsSource(view);
       }

       // Select no images initially
       winrt::Windows::UI::Xaml::Data::ItemIndexRange range(0, view.Size());
       grid.DeselectRange(range);
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
       std::map<HString, HUint> tags (counter.tags());
       std::map<HString, HUint>::const_iterator iter;

       // Build the list of 'extra' tags 
       // Use an array of pointers to the lists to avoid 'copy' semantics
       imageTags.Clear();
       std::vector<winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox> *> puiTagLists(3);
       puiTagLists[0] = &peopleTags;
       puiTagLists[1] = &placeTags;
       puiTagLists[2] = &timeTags;

       // Use this to accumulate matches against tages on the specific image
       std::vector<bool> tagsFound(tags.size(), false);
       bool found = false;

       // Iterate through the tag lists, then the tags in each list
       HUint listIndex, tagIndex;
       for (auto plist : puiTagLists)
       {
          for (listIndex = 0; listIndex < plist->Size(); listIndex++)
          {
             MbrsUI::TagCheckbox listItem = plist->GetAt(listIndex);
             CoreSubjectTagCounter::EUsed whereUsed = CoreSubjectTagCounter::EUsed::kNone;

             for (tagIndex = 0, iter = tags.begin(), found = false; tagIndex < tags.size() && ! found; tagIndex++, iter++)
             {
                HString tag((*iter).first);
                
                if (tag == listItem.name().c_str())
                {
                   tagsFound[tagIndex] = true;
                   found = true;
                   whereUsed = counter.countOf(tag);
                }
             }
             
             Windows::Foundation::IReference<bool> ref;
             if (found)
             {
                // Set check to 'all' or 'some ' if found
                ref = (whereUsed == CoreSubjectTagCounter::EUsed::kAll ? Windows::Foundation::IReference<bool>(true)
                   : nullptr);
             }
             else
             {
                // Set check to 'none' if not found
                ref = Windows::Foundation::IReference<bool>(false);
             }
             auto value = winrt::make<MbrsUI::implementation::TagCheckbox>(listItem.name(),
                ref);
             plist->SetAt(listIndex, value);
          }
       }

       // Add an 'other' entry if not found. Can only be kAll (true) or KSome (null), not kNone (false)
       for (tagIndex = 0, iter = tags.begin(); tagIndex < tags.size(); tagIndex++, iter++)
       {
          if (!tagsFound[tagIndex])
          {
             HString tag((*iter).first);
             auto whereUsed = counter.countOf(tag);
             auto cb = winrt::make<MbrsUI::implementation::TagCheckbox>(tag.c_str(),
                whereUsed == CoreSubjectTagCounter::EUsed::kAll ?
                Windows::Foundation::IReference<bool>(true) :
                nullptr);
             imageTags.Append(cb);
          }
       }
    }

    void setFilterPeriodImpl(const CoreDateFilter& filter,
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem& monthToggle,
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem& yearToggle)
    {
       monthToggle.IsChecked(filter.period() == CoreDateFilter::EPeriod::kMonth);
       yearToggle.IsChecked(filter.period() == CoreDateFilter::EPeriod::kYear);
    }

    void Page::onLoad(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (m_pModel)
       {
          // Setup the menu bar 
          HString path = m_pModel->imageSpecAsUIString();
          directoryPath().Text(path);
          CoreDateFilter filter = m_pModel->filter();
          winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem monthToggle = monthFilter();
          winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem yearToggle = yearFilter();
          setFilterPeriodImpl (filter, monthToggle, yearToggle);

          // set date filter in menu bar
          winrt::Windows::UI::Xaml::Controls::CalendarDatePicker date = filterDate();
          time_t ft = std::chrono::system_clock::to_time_t(filter.date());
          winrt::Windows::Foundation::DateTime dt = winrt::clock::from_time_t (ft);
          date.Date(IReference<DateTime>(dt));
          date.DateChanged({ this, &Page::onFilterDateChanged });

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
          setupImagesImpl(true, m_pModel, m_uiImages, grid);

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
           picker.as<IInitializeWithWindow>()->Initialize(hwnd);

           picker.SuggestedStartLocation(PickerLocationId::PicturesLibrary);
           picker.FileTypeFilter().Append(H_TEXT("*"));

           // set up an asynch operations
           StorageFolder folder = co_await  picker.PickSingleFolderAsync();
           */

          HString path = m_pModel->path();
          if (m_pDesktop->chooseFolder(path))
          {
             m_pModel->setPath(path);
             path = m_pModel->imageSpecAsUIString();
             this->directoryPath().Text(path);

             winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
             setupImagesImpl(false, m_pModel, m_uiImages, grid);
          }
       }
    }

    void winrt::MbrsUI::implementation::Page::onDayFilter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
       m_pModel->setFilterPeriod(CoreDateFilter::EPeriod::kDay);
       CoreDateFilter filter = m_pModel->filter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem monthToggle = monthFilter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem yearToggle = yearFilter();
       setFilterPeriodImpl(filter, monthToggle, yearToggle);

       // Refresh Images grid
       winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
       setupImagesImpl(false, m_pModel, m_uiImages, grid);

       // Set the caption so the user sees a representation of the filter 
       HString path = m_pModel->imageSpecAsUIString();
       directoryPath().Text(path);
    }

    void winrt::MbrsUI::implementation::Page::onMonthFilter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
       m_pModel->setFilterPeriod(CoreDateFilter::EPeriod::kMonth);
       CoreDateFilter filter = m_pModel->filter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem monthToggle = monthFilter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem yearToggle = yearFilter();
       setFilterPeriodImpl(filter, monthToggle, yearToggle);

       // Refresh Images grid
       winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
       setupImagesImpl(false, m_pModel, m_uiImages, grid);

       // Set the caption so the user sees a representation of the filter 
       HString path = m_pModel->imageSpecAsUIString();
       directoryPath().Text(path);
    }

    void winrt::MbrsUI::implementation::Page::onYearFilter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
       m_pModel->setFilterPeriod(CoreDateFilter::EPeriod::kYear);
       CoreDateFilter filter = m_pModel->filter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem monthToggle = monthFilter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem yearToggle = yearFilter();
       setFilterPeriodImpl(filter, monthToggle, yearToggle);

       // Refresh Images grid
       winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
       setupImagesImpl(false, m_pModel, m_uiImages, grid);

       // Set the caption so the user sees a representation of the filter 
       HString path = m_pModel->imageSpecAsUIString();
       directoryPath().Text(path);
    }

    void winrt::MbrsUI::implementation::Page::onNoFilter(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
       m_pModel->setFilterPeriod(CoreDateFilter::EPeriod::kNone);
       CoreDateFilter filter = m_pModel->filter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem monthToggle = monthFilter();
       winrt::Windows::UI::Xaml::Controls::ToggleMenuFlyoutItem yearToggle = yearFilter();
       setFilterPeriodImpl(filter, monthToggle, yearToggle);

       // Refresh Images grid
       winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
       setupImagesImpl(false, m_pModel, m_uiImages, grid);

       // Set the caption so the user sees a representation of the filter 
       HString path = m_pModel->imageSpecAsUIString();
       directoryPath().Text(path);
    }

    void Page::onFilterDateChanged(winrt::Windows::UI::Xaml::Controls::CalendarDatePicker const& sender, 
                             winrt::Windows::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs const& args)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(args);

       IReference<DateTime> rdt = args.NewDate();
       if (rdt)
       {
          DateTime dt = rdt.Value();
          time_t ft = winrt::clock::to_time_t(dt);

          m_pModel->setFilterDate(std::chrono::system_clock::from_time_t(ft));

          // Refresh Images grid
          winrt::Windows::UI::Xaml::Controls::GridView grid = imageGrid();
          setupImagesImpl(false, m_pModel, m_uiImages, grid);

          // Set the caption so the user sees a representation of the filter 
          HString path = m_pModel->imageSpecAsUIString();
          directoryPath().Text(path);
       }
    }

    void Page::onImageSelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       // Save any pending changes
       if (isChangeTimerRunning())
          saveChanges();

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

    void Page::onClearSelection(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       // Select no images 
       winrt::Windows::UI::Xaml::Data::ItemIndexRange range(0, m_uiImages.Size());
       imageGrid().DeselectRange(range);
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

    void Page::onAddTimeTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       winrt::Windows::UI::Xaml::Controls::TextBox tb = newTimeTag();
       onAddTagImpl(tb, m_storedTimesTags, m_uiTimesTags);
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

    void winrt::MbrsUI::implementation::Page::onImageTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);
       resetChangeTimer();
    }

    void Page::onTick(winrt::Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e)
    {
       UNREFERENCED_PARAMETER(sender);
       UNREFERENCED_PARAMETER(e);

       if (isChangeTimerRunning())
          saveChanges();
    }

    void Page::resetChangeTimer()
    {
       m_pDesktop->setSaveFlag(DesktopCallback::ESaveMode::kPending);

       // Stop time if it is running
       if (m_changeTimer.IsEnabled())
       {
          m_changeTimer.Stop();
       }
       m_changeTimer.Start();
    }

    bool Page::isChangeTimerRunning()
    {
       return m_changeTimer.IsEnabled();
    }

    void Page::saveChanges()
    {
       // Stop time if it is running
       if (m_changeTimer.IsEnabled())
       {
          m_changeTimer.Stop();
       }

       std::list<HString> add, remove;
       std::vector<HString> selected;

       // Gather selected image paths in 'selected'
       winrt::Windows::Foundation::Collections::IVector<IInspectable> selectedItems = imageGrid().SelectedItems();
       for (auto image : selectedItems)
       {
          selected.push_back(image.as<IImageView>().path().c_str());
       }

       if (selected.size() > 0)
       {
          std::vector <winrt::Windows::Foundation::Collections::IObservableVector<MbrsUI::TagCheckbox>> tagLists (4);
          tagLists[0] = m_uiPeopleTags;
          tagLists[1] = m_uiPlacesTags;
          tagLists[2] = m_uiTimesTags;
          tagLists[3] = m_uiImageTags;

          for (auto list : tagLists)
          {
             for (auto cb : list)
             {
                if (cb.isUsed() && cb.isUsed().GetBoolean())
                   add.push_back(cb.name().c_str());
                else 
                if (cb.isUsed() && cb.isUsed().GetBoolean() == false)
                   remove.push_back(cb.name().c_str());
             }
          }

          std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(selected));
          std::shared_ptr<CoreCommand> pCompoundCmd(COMMON_NEW CoreCompoundImageTagChangeCommand(add, remove, m_pModel, pSelection));

          m_pDesktop->setSaveFlag(DesktopCallback::ESaveMode::kSaving);
         
          winrt::Windows::UI::Core::CoreCursor wait (winrt::Windows::UI::Core::CoreCursorType::Wait, 0); 
          winrt::Windows::UI::Core::CoreCursor arrow (winrt::Windows::UI::Core::CoreCursorType::Arrow, 0);

          winrt::Windows::UI::Core::CoreWindow window( winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread() );
          window.PointerCursor(wait);

          m_pCommandProcessor->adoptAndDo(pCompoundCmd);
          m_pDesktop->setSaveFlag(DesktopCallback::ESaveMode::kSaved);

          window.PointerCursor(arrow);
       }
    }
}
