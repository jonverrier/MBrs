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
       TagCheckbox(hstring name, bool isUsed);

        hstring name() const;

        bool isUsed() const;

        hstring m_name;
        bool m_isUsed;
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct TagCheckbox : TagCheckboxT<TagCheckbox, implementation::TagCheckbox>
    {
    };
}