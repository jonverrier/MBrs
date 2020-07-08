#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"

#include "TagCheckbox.g.h"

namespace winrt::MbrsUI::implementation
{
    struct TagCheckbox :TagCheckboxT<TagCheckbox>
    {
        TagCheckbox(hstring name, Windows::Foundation::IReference<bool> isUsed);

        hstring name() const;

        Windows::Foundation::IReference<bool> isUsed() const;
        void isUsed(Windows::Foundation::IReference<bool> used);

        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

        hstring m_name;
        Windows::Foundation::IReference<bool> m_isUsed;

    private:
       winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct TagCheckbox : TagCheckboxT<TagCheckbox, implementation::TagCheckbox>
    {
    };
}