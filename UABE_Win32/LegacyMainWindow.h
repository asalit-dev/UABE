#pragma once

#include <windows.h>
#include <commctrl.h>
#include <string>

// A source-available approximation of the UABE 2.2 stable-d shell.
// The modern MainWindow2 remains unchanged; this class is intentionally
// isolated so the legacy shell can be wired to the existing backend safely.
class LegacyMainWindow
{
public:
    LegacyMainWindow(HINSTANCE instance, std::wstring initialDirectory = L".");
    ~LegacyMainWindow();

    LegacyMainWindow(const LegacyMainWindow&) = delete;
    LegacyMainWindow& operator=(const LegacyMainWindow&) = delete;

    bool create(int nShowCmd = SW_SHOWNORMAL);
    int runMessageLoop();
    HWND window() const { return hWindow; }

    // These hooks are the seam for connecting the existing AssetsTools/AppContext
    // operations without making the UI depend on the beta MainWindow2 class.
    void setFileOpenCallback(void (*callback)(const std::wstring&)) { openCallback = callback; }
    void setInfoCallback(void (*callback)()) { infoCallback = callback; }

private:
    HINSTANCE hInstance;
    HWND hWindow = nullptr;
    HWND hPath = nullptr;
    HWND hTypeTree = nullptr;
    HWND hExport = nullptr;
    HWND hImport = nullptr;
    HWND hInfo = nullptr;
    HWND hTree = nullptr;
    HWND hFiles = nullptr;
    HWND hStatus = nullptr;
    std::wstring directory;
    void (*openCallback)(const std::wstring&) = nullptr;
    void (*infoCallback)() = nullptr;

    static constexpr UINT WM_LEGACY_REFRESH = WM_APP + 0x221;
    static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    bool registerClass();
    void createControls();
    void layoutControls(int width, int height);
    void refreshFiles();
    void addDirectoryTree();
    void openSelectedFile();
    void setStatus(const std::wstring& text);
    static std::wstring formatSize(unsigned long long size);
};
