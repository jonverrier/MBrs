#include "pch.h"
#include "Page.h"
#if __has_include("Page.g.cpp")
#include "Page.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MbrsUI::implementation
{
    Page::Page()
    {
        InitializeComponent();
    }

    void Page::onLoad(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {

    }

    void Page::changeDirectory(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
    {

    }
}





