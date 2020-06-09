//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include "CoreMbrsModelCommand.h"

namespace UI
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
	
	private:
		void onLoad(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void changeDirectory(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
#if 0
		std::shared_ptr< CoreImageListModel> m_pModel;
		CoreCommandProcessor m_processor;
#endif
	};
}
