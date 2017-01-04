// SnakeGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SnakeGame.h"

#include "Game.h"

#define MAX_LOADSTRING 100

#define GAME_SPEED_MSECS 300
#define SNAKE_COLOR RGB(128, 128, 128)
#define FOOD_COLOR RGB(220, 200, 0)
#define COLLISION_COLOR RGB(200, 0, 0)
#define STROKE_COLOR RGB(0, 0, 0)

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

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

   // Initialize global strings
   LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(hInstance, IDC_SNAKEGAME, szWindowClass, MAX_LOADSTRING);
   MyRegisterClass(hInstance);

   // Perform application initialization:
   if (!InitInstance(hInstance, nCmdShow))
   {
      return FALSE;
   }

   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKEGAME));

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

   return (int)msg.wParam;
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

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKEGAME));
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SNAKEGAME);
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
   auto const squareSize = Game::instance().squareSize();
   auto const width = Game::instance().fieldWidth();
   auto const height = Game::instance().fieldHeight();
   RECT rect = {0, 0, LONG(width * squareSize), LONG(height * squareSize)};

   DWORD const dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

   AdjustWindowRect(&rect, dwStyle, TRUE);

   HWND hWnd = CreateWindowW
      ( szWindowClass           // lpClassName
      , szTitle                 // lpWindowName
      , dwStyle                 // dwStyle
      , CW_USEDEFAULT           // x
      , CW_USEDEFAULT           // y
      , rect.right - rect.left  // nWidth
      , rect.bottom - rect.top  // nHeight
      , nullptr                 // hWndParent
      , nullptr                 // hMenu
      , hInstance               // hInstance
      , nullptr                 // lpParam
      );

   if (!hWnd)
   {
      return FALSE;
   }

   SetTimer(hWnd, 0, GAME_SPEED_MSECS, nullptr);

   // Yeah... These are not ideal handlers
   Game::instance().setCollisionHandler([hWnd]() {
      MessageBox(hWnd, _T("Game over!"), _T("You lose!"), MB_OK | MB_ICONEXCLAMATION);
   });

   Game::instance().setVictoryHandler([hWnd]() {
      MessageBox(hWnd, _T("Victory!"), _T("You won!"), MB_OK | MB_ICONEXCLAMATION);
   });

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
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
      case IDC_LEFT:
         Game::instance().turn(Game::Direction::Left);
         break;
      case IDC_RIGHT:
         Game::instance().turn(Game::Direction::Right);
         break;
      case IDC_UP:
         Game::instance().turn(Game::Direction::Up);
         break;
      case IDC_DOWN:
         Game::instance().turn(Game::Direction::Down);
         break;
      case ID_GAME_PAUSE:
         {
            auto result = KillTimer(hWnd, 0);
            if(!result)
            {
               SetTimer(hWnd, 0, GAME_SPEED_MSECS, nullptr);
            }
         }
         break;
      case ID_GAME_RESET:
         Game::instance().reset();
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
      }
   }
   break;
   case WM_TIMER:
      {
         Game::instance().singleStep();
         RECT rect;
         GetClientRect(hWnd, &rect);
         InvalidateRect(hWnd, &rect, TRUE);
      }
      break;
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hWnd, &ps);
         auto const squareSize = Game::instance().squareSize();

         auto drawPoint = [&](Game::Point const & point, LONG color) {
            auto x = point.x;
            auto y = point.y;
            RECT rect = {
               LONG(x * squareSize), 
               LONG(y * squareSize), 
               LONG((x + 1) * squareSize), 
               LONG((y + 1) * squareSize) 
            };
            HBRUSH hBrush = CreateSolidBrush(color);
            HPEN hPen = CreatePen(PS_SOLID, 1, STROKE_COLOR);
            SelectObject(hdc, hBrush);
            SelectObject(hdc, hPen);
            FillRect(hdc, &rect, hBrush);
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            DeleteObject(hBrush);
            DeleteObject(hPen);
         };
         // Drawing snake's body
         for(auto const & snakeBlock: Game::instance().snake())
         {
            drawPoint(snakeBlock, SNAKE_COLOR);
         }
         
         if(!Game::instance().isVictory())
         {
            // Drawing food
            auto food = Game::instance().food();
            drawPoint(food, FOOD_COLOR);
         }

         if(Game::instance().isCollision())
         {
            // Drawing the collision point
            auto collisionPoint = Game::instance().collisionPoint();
            drawPoint(collisionPoint, COLLISION_COLOR);
         }
         ValidateRect(hWnd, NULL);
         EndPaint(hWnd, &ps);
      }
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
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
