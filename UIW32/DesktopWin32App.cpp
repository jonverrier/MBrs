// DesktopWin32App.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DesktopWin32App.h"

#include "CoreMbrsModelCommand.h"
#include "UIDesktopCallback.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Xaml::Controls;

class DesktopCallbackImpl : public DesktopCallback
{
public:
   DesktopCallbackImpl(HWND hwnd);
   virtual ~DesktopCallbackImpl();

   virtual bool chooseFolder(HString& newPath);

   virtual void setSaveFlag(ESaveMode mode);

private:
   HWND m_hwnd;
};


#define MAX_LOADSTRING 100

void AdjustLayout(HWND);

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND      hMainWindow;                          // Main window
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// the main window class name
winrt::MbrsUI::App hostApp{ nullptr };
winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource _desktopWindowXamlSource{ nullptr };
winrt::MbrsUI::Page _myUserControl{ nullptr };

DesktopCallbackImpl::DesktopCallbackImpl(HWND hwnd)
   : m_hwnd(hwnd)
{
}

DesktopCallbackImpl::~DesktopCallbackImpl()
{
}

bool DesktopCallbackImpl::chooseFolder(HString& newPath)
{
   PWSTR pszFilePath = NULL;
   bool succeeded = false;
   HRESULT hr;

   // Create the FileOpenDialog object.
   CComPtr<IFileOpenDialog> pFileOpen;
   hr = pFileOpen.CoCreateInstance(__uuidof(FileOpenDialog));

   if (SUCCEEDED(hr))
   {
      FILEOPENDIALOGOPTIONS options = FOS_PICKFOLDERS | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST;
      pFileOpen->SetOptions(options);
      pFileOpen->SetTitle(H_TEXT("Choose a different folder."));

      // Set up current directory
      LPITEMIDLIST pidl = SHSimpleIDListFromPath(newPath.c_str());
      if (pidl)
      {
         IShellItem* pCurrentFolder;
         SHCreateItemFromIDList(pidl, __uuidof(IShellItem),  (void **) &pCurrentFolder);
         CoTaskMemFree(static_cast<void *> (pidl));
         if (pCurrentFolder)
         {
            pFileOpen->SetFolder(pCurrentFolder);
            pCurrentFolder->Release();
         }
      }

      // Show the Open dialog box.
      hr = pFileOpen->Show(NULL);

      // Get the file name from the dialog box.
      if (SUCCEEDED(hr))
      {
         CComPtr <IShellItem> pItem;
         hr = pFileOpen->GetResult(&pItem);
         if (SUCCEEDED(hr))
         {
            PWSTR pszFilePath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

            // Put the path name in output value.
            if (SUCCEEDED(hr))
            {
               newPath = HString(pszFilePath);
               succeeded = true;
               CoTaskMemFree(pszFilePath);
            }
         }
      }
   }

   return succeeded;
}

void DesktopCallbackImpl::setSaveFlag(ESaveMode mode)
{
   HString caption; 

   switch (mode)
   {
   case ESaveMode::kNone:
      caption = HString(szTitle);
      break;
   case ESaveMode::kPending:
      caption = HString(szTitle) + H_TEXT(" (") + H_TEXT("Pending") + H_TEXT(")");
      break;
   case ESaveMode::kSaving:
      caption = HString(szTitle) + H_TEXT(" (") + H_TEXT("Saving") + H_TEXT(")");
      break;
   case ESaveMode::kSaved:
      caption = HString(szTitle) + H_TEXT(" (") + H_TEXT("Saved") + H_TEXT(")");
      break;
   }
   ::SetWindowText(m_hwnd, caption.c_str());
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    hostApp = winrt::MbrsUI::App{};
    _desktopWindowXamlSource = winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource{};

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DESKTOPWIN32APP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
   
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESKTOPWIN32APP));

    DesktopCallbackImpl* pDesktopCallbackImpl = COMMON_NEW DesktopCallbackImpl (hMainWindow);
    uint64_t u = reinterpret_cast<uint64_t> (pDesktopCallbackImpl);
    _myUserControl.setDesktopCallback(u);

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESKTOPWIN32APP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DESKTOPWIN32APP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hMainWindow = hWnd;

   // XAML Island
   if (_desktopWindowXamlSource != nullptr)
   {
       // Get handle to corewindow
       auto interop = _desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
       // Parent the DesktopWindowXamlSource object to current window
       check_hresult(interop->AttachToWindow(hWnd));
       // This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.
       HWND hWndXamlIsland = nullptr;
       // Get the new child window's hwnd
       interop->get_WindowHandle(&hWndXamlIsland);
       RECT windowRect;
       ::GetClientRect(hWnd, &windowRect);
       ::SetWindowPos(hWndXamlIsland, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_SHOWWINDOW);

       _myUserControl = winrt::MbrsUI::Page();
       
       _desktopWindowXamlSource.Content(_myUserControl);

   }
   //END XAML Island
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE: 
        AdjustLayout(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        if (_desktopWindowXamlSource != nullptr)
        {
            _desktopWindowXamlSource.Close();
            _desktopWindowXamlSource = nullptr;
        }
        break;
    case WM_MOVE:
        if (_desktopWindowXamlSource != nullptr)
        {
            auto content = _desktopWindowXamlSource.Content().as< winrt::MbrsUI::Page>();
            if (content != nullptr)
            {
                auto popups = Windows::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(content.XamlRoot());
                if (popups.Size() > 0)
                {
                    /*for (Windows::UI::Xaml::Controls::Primitives::Popup&& popup : popups)
                    {
                        popup.IsOpen(true);
                    }*/
                }
            }
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void AdjustLayout(HWND hWnd)
{
    if (_desktopWindowXamlSource != nullptr)
    {
        auto interop = _desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
        HWND xamlHostHwnd = NULL;
        check_hresult(interop->get_WindowHandle(&xamlHostHwnd));
        RECT windowRect;
        ::GetClientRect(hWnd, &windowRect);
        ::SetWindowPos(xamlHostHwnd, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_SHOWWINDOW);
    }
}

