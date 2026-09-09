option(UABE_BUILD_LEGACY_UI "Compile the source-available UABE 2.2-compatible legacy shell" ON)

set(UABE_WIN32_SOURCES AddAssetDialog.cpp AssetDependDialog.cpp AssetListDialog.cpp AssetViewModifyDialog.cpp BatchImportDialog.cpp BundleDialog.cpp FileDialog.cpp MainWindow2.cpp ModInstallerEditor2.cpp ModPackageLoader.cpp MonoBehaviourManager.cpp ProgressDialog.cpp SelectClassDbDialog.cpp TypeDatabaseEditor.cpp TypeDbPackageEditor.cpp Win32AppContext.cpp Win32ModTreeDialogBase.cpp AssetBundleExtractor.rc UABE_Win32.manifest "Win32BatchImportDesc.h" "api.h" "Win32BatchImportDesc.cpp" "Win32TaskStatusTracker.cpp" "SplitterControlHandler.cpp" "Win32PluginManager.cpp")
if (UABE_BUILD_LEGACY_UI)
    list(APPEND UABE_WIN32_SOURCES LegacyMainWindow.cpp LegacyMainWindow.h)
endif()

add_library (UABE_Win32 SHARED ${UABE_WIN32_SOURCES})

if (UABE_BUILD_LEGACY_UI)
    add_executable(UABE_LegacyPreview WIN32 LegacyMainWindowDemo.cpp LegacyMainWindow.cpp LegacyMainWindow.h)
    target_include_directories(UABE_LegacyPreview PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(UABE_LegacyPreview PRIVATE Comctl32.lib)
    set_target_properties(UABE_LegacyPreview PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
target_include_directories (UABE_Win32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${MCTRL_INCLUDE_DIR})

set_target_properties(UABE_Win32 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
target_link_libraries(UABE_Win32 PUBLIC UABE_Generic AssetsTools libStringConverter)
target_link_libraries(UABE_Win32 PRIVATE ModInstaller libCompression Comctl32.lib Shlwapi.lib mCtrl)
