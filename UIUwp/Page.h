﻿#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "Page.g.h"

#include "CoreMbrsModelCommand.h"
#include "UIDesktopCallback.h"


namespace winrt::MbrsUI::implementation
{
    struct Page :PageT<Page>
    {
        Page();

        void setModel(uint64_t ptrAsUint64);

        void setDesktopCallback (uint64_t ptrAsUint64);

        void onLoad(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void onChangeDirectory (winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);


        DesktopCallback    *m_pDesktop;
        CoreImageListModel *m_pModel;        
    };
}

namespace winrt::MbrsUI::factory_implementation
{
    struct Page : PageT<Page, implementation::Page>
    {
    };
}