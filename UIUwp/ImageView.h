#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"

#include "ImageView.g.h"

namespace winrt::MbrsUI::implementation
{
    struct ImageView :ImageViewT<ImageView>
    {
        ImageView(hstring path, hstring fileName);

        hstring path () const;

        hstring fileName() const;

        hstring m_path;
        hstring m_fileName;
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct ImageView : ImageViewT<ImageView, implementation::ImageView>
    {
    };
}