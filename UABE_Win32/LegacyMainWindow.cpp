#include "stdafx.h"
#include "LegacyMainWindow.h"

#include <algorithm>
#include <cwchar>
#include <filesystem>
#include <utility>
#include <vector>

namespace {
constexpr wchar_t kClassName[] = L"UABE.LegacyMainWindow.2.2";
constexpr wchar_t kWindowTitle[] = L"Unity Assets Bundle Extractor";
constexpr int kTopPanelHeight = 170;
constexpr int kTreeWidth = 155;
constexpr int kMargin = 8;
constexpr int kControlHeight = 22;
constexpr int kButtonWidth = 92;
constexpr int kButtonGap = 8;
constexpr int kIdPath = 2101;
constexpr int kIdTypeTree = 2102;
constexpr int kIdExport = 2103;
constexpr int kIdImport = 2104;
constexpr int kIdInfo = 2105;
constexpr int kIdTree = 2106;
constexpr int kIdFiles = 2107;
constexpr int kIdStatus = 2108;
constexpr int kIdRefresh = 2109;
}

LegacyMainWindow::LegacyMainWindow(HINSTANCE instance, std::wstring initialDirectory)
    : hInstance(instance), directory(std::move(initialDirectory))
{
    if (directory.empty())
        directory = L".";
}

LegacyMainWindow::~LegacyMainWindow()
{
    if (hWindow)
        DestroyWindow(hWindow);
}

bool LegacyMainWindow::registerClass()
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &LegacyMainWindow::wndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    wc.lpszClassName = kClassName;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    return RegisterClassExW(&wc) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool LegacyMainWindow::create(int nShowCmd)
{
    if (!registerClass())
        return false;

    hWindow = CreateWindowExW(
        0, kClassName, kWindowTitle,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 1045, 610,
        nullptr, nullptr, hInstance, this);
    if (!hWindow)
        return false;

    ShowWindow(hWindow, nShowCmd);
    UpdateWindow(hWindow);
    return true;
}

void LegacyMainWindow::createControls()
{
    HMENU fileMenu = CreateMenu();
    HMENU optionsMenu = CreateMenu();
    HMENU helpMenu = CreateMenu();
    AppendMenuW(fileMenu, MF_STRING, 2201, L"Open...");
    AppendMenuW(fileMenu, MF_STRING, 2202, L"Exit");
    AppendMenuW(optionsMenu, MF_STRING, 2203, L"Settings");
    AppendMenuW(helpMenu, MF_STRING, 2204, L"About...");
    HMENU mainMenu = CreateMenu();
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"File");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(optionsMenu), L"Options");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"Help");
    SetMenu(hWindow, mainMenu);

    CreateWindowExW(0, L"STATIC", L"No file opened.", WS_CHILD | WS_VISIBLE | SS_CENTER,
        16, 14, 370, 22, hWindow, nullptr, hInstance, nullptr);
    hPath = CreateWindowExW(WS_EX_CLIENTEDGE, WC_COMBOBOXW, L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        16, 42, 370, 25, hWindow, reinterpret_cast<HMENU>(kIdPath), hInstance, nullptr);
    hTypeTree = CreateWindowExW(0, WC_BUTTONW,
        L"Remove the TypeTree (use this for UAE only)",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        16, 72, 370, 22, hWindow, reinterpret_cast<HMENU>(kIdTypeTree), hInstance, nullptr);

    hExport = CreateWindowExW(0, WC_BUTTONW, L"Export",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        80, 108, kButtonWidth, 26, hWindow, reinterpret_cast<HMENU>(kIdExport), hInstance, nullptr);
    hImport = CreateWindowExW(0, WC_BUTTONW, L"Import",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        180, 108, kButtonWidth, 26, hWindow, reinterpret_cast<HMENU>(kIdImport), hInstance, nullptr);
    hInfo = CreateWindowExW(0, WC_BUTTONW, L"Info",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        280, 108, kButtonWidth, 26, hWindow, reinterpret_cast<HMENU>(kIdInfo), hInstance, nullptr);
    EnableWindow(hExport, FALSE);
    EnableWindow(hImport, FALSE);
    EnableWindow(hInfo, FALSE);

    hTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
        0, kTopPanelHeight, kTreeWidth, 350, hWindow,
        reinterpret_cast<HMENU>(kIdTree), hInstance, nullptr);
    hFiles = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        kTreeWidth, kTopPanelHeight, 800, 350, hWindow,
        reinterpret_cast<HMENU>(kIdFiles), hInstance, nullptr);
    ListView_SetExtendedListViewStyle(hFiles, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    LVCOLUMNW col{};
    col.mask = LVCF_TEXT | LVCF_WIDTH;
    col.pszText = const_cast<LPWSTR>(L"Name");
    col.cx = 360;
    ListView_InsertColumn(hFiles, 0, &col);
    col.pszText = const_cast<LPWSTR>(L"Size");
    col.cx = 110;
    ListView_InsertColumn(hFiles, 1, &col);
    col.pszText = const_cast<LPWSTR>(L"Type");
    col.cx = 140;
    ListView_InsertColumn(hFiles, 2, &col);

    hStatus = CreateWindowExW(0, STATUSCLASSNAMEW, L"Ready",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWindow,
        reinterpret_cast<HMENU>(kIdStatus), hInstance, nullptr);
    int parts[] = { 120, 260, -1 };
    SendMessageW(hStatus, SB_SETPARTS, 3, reinterpret_cast<LPARAM>(parts));
    SendMessageW(hStatus, SB_SETTEXT, 0, reinterpret_cast<LPARAM>(L"Start"));
    SendMessageW(hStatus, SB_SETTEXT, 1, reinterpret_cast<LPARAM>(L"32bit"));
    SendMessageW(hStatus, SB_SETTEXT, 2, reinterpret_cast<LPARAM>(L"Unity Assets Bundle Extractor"));

    addDirectoryTree();
    refreshFiles();
}

void LegacyMainWindow::layoutControls(int width, int height)
{
    RECT statusRect{};
    GetWindowRect(hStatus, &statusRect);
    const int statusHeight = statusRect.bottom - statusRect.top;
    const int contentTop = kTopPanelHeight;
    const int contentHeight = std::max(80, height - contentTop - statusHeight);
    MoveWindow(hTree, 0, contentTop, kTreeWidth, contentHeight, TRUE);
    MoveWindow(hFiles, kTreeWidth, contentTop, std::max(120, width - kTreeWidth), contentHeight, TRUE);
}

void LegacyMainWindow::addDirectoryTree()
{
    TVINSERTSTRUCTW insert{};
    insert.hParent = TVI_ROOT;
    insert.item.mask = TVIF_TEXT;
    wchar_t drive[] = L"C:";
    insert.item.pszText = drive;
    HTREEITEM root = TreeView_InsertItem(hTree, &insert);
    insert.hParent = root;
    wchar_t child[] = L"Current folder";
    insert.item.pszText = child;
    TreeView_InsertItem(hTree, &insert);
    TreeView_Expand(hTree, root, TVE_EXPAND);
}

std::wstring LegacyMainWindow::formatSize(unsigned long long size)
{
    wchar_t buffer[64]{};
    if (size >= 1024 * 1024)
        swprintf_s(buffer, L"%.2f MB", static_cast<double>(size) / (1024.0 * 1024.0));
    else if (size >= 1024)
        swprintf_s(buffer, L"%.2f KB", static_cast<double>(size) / 1024.0);
    else
        swprintf_s(buffer, L"%llu B", size);
    return buffer;
}

void LegacyMainWindow::refreshFiles()
{
    if (!hFiles)
        return;
    ListView_DeleteAllItems(hFiles);
    std::error_code error;
    std::filesystem::path base(directory);
    if (!std::filesystem::exists(base, error))
        base = std::filesystem::current_path(error);
    int row = 0;
    for (const auto& entry : std::filesystem::directory_iterator(base, error))
    {
        if (error)
            break;
        const bool isDirectory = entry.is_directory(error);
        const std::wstring name = entry.path().filename().wstring();
        LVITEMW item{};
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iItem = row;
        item.pszText = const_cast<LPWSTR>(name.c_str());
        item.lParam = isDirectory ? 1 : 0;
        ListView_InsertItem(hFiles, &item);
        std::wstring size = isDirectory ? L"" : formatSize(entry.file_size(error));
        ListView_SetItemText(hFiles, row, 1, const_cast<LPWSTR>(size.c_str()));
        const std::wstring type = isDirectory ? L"Folder" :
            (entry.path().extension().empty() ? L"File" : entry.path().extension().wstring());
        ListView_SetItemText(hFiles, row, 2, const_cast<LPWSTR>(type.c_str()));
        ++row;
    }
    setStatus(std::to_wstring(row) + L" items");
}

void LegacyMainWindow::openSelectedFile()
{
    const int row = ListView_GetNextItem(hFiles, -1, LVNI_SELECTED);
    if (row < 0)
        return;
    wchar_t name[MAX_PATH]{};
    ListView_GetItemText(hFiles, row, 0, name, MAX_PATH);
    std::filesystem::path selected = std::filesystem::path(directory) / name;
    if (std::filesystem::is_directory(selected))
    {
        directory = selected.wstring();
        refreshFiles();
        return;
    }
    if (openCallback)
        openCallback(selected.wstring());
    setStatus(L"Selected: " + selected.filename().wstring());
}

void LegacyMainWindow::setStatus(const std::wstring& text)
{
    if (hStatus)
        SendMessageW(hStatus, SB_SETTEXT, 2, reinterpret_cast<LPARAM>(text.c_str()));
}

LRESULT CALLBACK LegacyMainWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LegacyMainWindow* self = reinterpret_cast<LegacyMainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE)
    {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<LegacyMainWindow*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hWindow = hwnd;
    }
    return self ? self->handleMessage(message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT LegacyMainWindow::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        createControls();
        return 0;
    case WM_SIZE:
        layoutControls(LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 2201:
            openSelectedFile();
            return 0;
        case 2202:
            DestroyWindow(hWindow);
            return 0;
        case kIdInfo:
            if (infoCallback) infoCallback();
            return 0;
        case kIdExport:
        case kIdImport:
            setStatus(LOWORD(wParam) == kIdExport ? L"Export requested" : L"Import requested");
            return 0;
        }
        break;
    case WM_NOTIFY:
        if (reinterpret_cast<LPNMHDR>(lParam)->idFrom == kIdFiles &&
            reinterpret_cast<LPNMHDR>(lParam)->code == NM_DBLCLK)
        {
            openSelectedFile();
            return 0;
        }
        break;
    case WM_CONTEXTMENU:
        if (reinterpret_cast<HWND>(wParam) == hFiles)
            refreshFiles();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWindow, message, wParam, lParam);
}

int LegacyMainWindow::runMessageLoop()
{
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}
