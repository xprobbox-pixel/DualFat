#ifndef BuildRoot
  #define BuildRoot "..\build-windows\CobaltDualFilter_artefacts\Release"
#endif
#ifndef OutputRoot
  #define OutputRoot "..\dist"
#endif
[Setup]
AppId={{28858570-4073-4D3E-8AB7-604026B48677}
AppName=DUALFAT
AppVersion=1.2.0
AppPublisher=Cobalt Audio
DefaultDirName={autopf}\DUALFAT
DefaultGroupName=DUALFAT
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
PrivilegesRequired=admin
OutputDir={#OutputRoot}
OutputBaseFilename=DUALFAT-1.2.0-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\DUALFAT.exe
CloseApplications=yes
RestartApplications=no
[Files]
Source: "{#BuildRoot}\Standalone\DUALFAT.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildRoot}\VST3\DUALFAT.vst3\*"; DestDir: "{commoncf64}\VST3\DUALFAT.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\Presets\*.dualfat"; DestDir: "{app}\Presets"; Flags: ignoreversion
Source: "..\Assets\Fonts\LICENSE-Roboto.txt"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\DUALFAT"; Filename: "{app}\DUALFAT.exe"
Name: "{group}\Uninstall DUALFAT"; Filename: "{uninstallexe}"
[Run]
Filename: "{app}\DUALFAT.exe"; Description: "Launch DUALFAT"; Flags: nowait postinstall skipifsilent
