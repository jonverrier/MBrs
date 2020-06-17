#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "Page.g.h"

namespace winrt::MbrsUI::implementation
{
    struct Page :PageT<Page>
    {
        Page();

        void onLoad(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void changeDirectory (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct Page : PageT<Page, implementation::Page>
    {
    };
}