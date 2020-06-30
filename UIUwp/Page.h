#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"

#include "StringWrapper.h"
#include "StringWrapper.g.h"
#include "Page.g.h"

#include "CoreMbrsModelCommand.h"
#include "CoreKeywords.h"

namespace winrt::MbrsUI::implementation
{
    struct Page :PageT<Page>
    {
        Page();

        void setDesktopCallback (uint64_t ptr);

        void onLoad(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void onChangeDirectory (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onImageSelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewPersonDefaultTagChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onPersonDefaultTagRightTap(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddPersonDefaultTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onRemovePersonDefaultTag(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewPlaceChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddPlace(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        void onNewTimeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        void onAddTime(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        DesktopCallback    *m_pDesktop;   
        std::shared_ptr<CoreCommandProcessor> m_pCommandProcessor;
        std::shared_ptr<CoreImageListModel> m_pModel;

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_uiPeopleDefaultTags;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_placeTags;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> m_timeTags;

        CoreCategoryKeywords m_storedPeopleDefaultTags;

        HString m_personContext; 
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct Page : PageT<Page, implementation::Page>
    {
    };
}