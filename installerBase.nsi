;-------------------------------------------------------------------------------
; Includes
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"
!include "x64.nsh"

;-------------------------------------------------------------------------------
; Constants
!define PRODUCT_NAME "MikuMikuWorld for Chart Cyanvas"
!define PRODUCT_DESCRIPTION "MikuMikuWorld for Chart Cyanvas"
!define COPYRIGHT "Copyright (c) 2022 Crash5b, 2024 Nanashi."
!define PRODUCT_VERSION "{version}.0"
!define SETUP_VERSION "{version}.0"

;-------------------------------------------------------------------------------
; Attributes
Name "MikuMikuWorld for Chart Cyanvas"
OutFile "build/mmw4cc-{version}-setup.exe"
InstallDir "$LOCALAPPDATA\Programs\mmw4cc"
InstallDirRegKey HKCU "Software\mmw4cc" ""
RequestExecutionLevel user

;-------------------------------------------------------------------------------
; Version Info
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "ProductVersion" "${PRODUCT_VERSION}"
VIAddVersionKey "FileDescription" "${PRODUCT_DESCRIPTION}"
VIAddVersionKey "LegalCopyright" "${COPYRIGHT}"
VIAddVersionKey "FileVersion" "${SETUP_VERSION}"

;-------------------------------------------------------------------------------
; Modern UI Appearance
!define MUI_ICON "MikuMikuWorld\mmw_icon.ico"

;-------------------------------------------------------------------------------
; Variables
Var StartMenuFolder

;-------------------------------------------------------------------------------
; Installer Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\mmw4cc"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;-------------------------------------------------------------------------------
; Uninstaller Pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_DIRECTORY
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;-------------------------------------------------------------------------------
; Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Japanese"

;-------------------------------------------------------------------------------
; Installer Sections
Section "MikuMikuWorld for Chart Cyanvas" Mmw4cc
	SetOutPath "$INSTDIR"
  File /r "build\MikuMikuWorld\*.*"

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\MikuMikuWorld for Chart Cyanvas.lnk" "$INSTDIR\MikuMikuWorld.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmw4cc" \
                   "DisplayName" "MikuMikuWorld for Chart Cyanvas"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmw4cc" \
                   "UninstallString" "$\"$INSTDIR\uninstall.exe$\""

  WriteRegStr HKCU "Software\Classes\.usc" "" "MikuMikuWorld.usc"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.usc" "" "Universal Sekai Chart File"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.usc\DefaultIcon" "" "$INSTDIR\MikuMikuWorld.exe"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.usc\shell\open\command" "" "$INSTDIR\MikuMikuWorld.exe $\"%1$\""

  WriteRegStr HKCU "Software\Classes\.ccmmws" "" "MikuMikuWorld.ccmmws"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.ccmmws" "" "MMW4CC Chart File"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.ccmmws\DefaultIcon" "" "$INSTDIR\MikuMikuWorld.exe"
  WriteRegStr HKCU "Software\Classes\MikuMikuWorld.ccmmws\shell\open\command" "" "$INSTDIR\MikuMikuWorld.exe $\"%1$\""
SectionEnd

;-------------------------------------------------------------------------------
; Uninstaller Sections
Section "Uninstall"
	RMDir /r "$INSTDIR"
  Delete "$INSTDIR\Uninstall.exe"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmw4cc"
  DeleteRegKey HKCU "Software\Classes\.usc"
  DeleteRegKey HKCU "Software\Classes\MikuMikuWorld.usc"
  DeleteRegKey HKCU "Software\Classes\.ccmmws"
  DeleteRegKey HKCU "Software\Classes\MikuMikuWorld.ccmmws"

  RMDir /r "$SMPROGRAMS\$StartMenuFolder"
SectionEnd
