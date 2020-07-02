#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"

#include "Page.g.h"
#include "ImageView.g.h"
#include "TagCheckbox.g.h"

#include "ImageView.h"
#include "TagCheckbox.h"

#include "CoreMbrsModelCommand.h"
#include "CoreKeywords.h"

#include "UIDesktopCallback.h"

namespace winrt::MbrsUI::implementation
{
    struct Page :PageT<Page>
    {
        Page();

        void setDesktopCallback (uint64_t ptr);

        void onLoad(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

        void onChangeDirectory (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onImageSelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewPersonTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onPersonTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddPersonTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onRemovePersonTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewPlaceTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onPlaceTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddPlaceTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onRemovePlaceTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewTimeTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onTimeTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddTimeTag (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onRemoveTimeTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        DesktopCallback    *m_pDesktop;   
        std::shared_ptr<CoreCommandProcessor> m_pCommandProcessor;
        std::shared_ptr<CoreImageListModel> m_pModel;

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_uiPeopleTags;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_uiPlacesTags;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_uiTimesTags;

        CoreCategoryKeywords m_storedPeopleTags;
        CoreCategoryKeywords m_storedPlacesTags;
        CoreCategoryKeywords m_storedTimesTags;

        HString m_personContext; 
        HString m_placeContext;
        HString m_timeContext;

        winrt::Windows::Foundation::Collections::IObservableVector <MbrsUI::ImageView> m_uiImages;
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct Page : PageT<Page, implementation::Page>
    {
    };
}