#if Ver < EncodeVer(6,0,0,0)
  #error This script requires Inno Setup 6 or later
#endif

[Setup]
AppName = AirFreight
AppVersion = 4
AppPublisher = Christopher Schatz & Serhat Sural
AppVerName = AirFreight v4
UsePreviousAppDir = false
DefaultDirName = {sd}\AirFreight
DisableProgramGroupPage = yes

Uninstallable = true
UninstallDisplayIcon = {app}\unins000.exe
UninstallDisplayName = AirFreight
OutputDir = .

SetupIconFile = Logo.ico
WizardSmallImageFile = Plane.bmp

OutputBaseFilename=AirFreight_Installer
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=none

[Languages]
Name: "de"; MessagesFile: "default.isl"

[Files]
Source: "PathMgr.dll";  DestDir: "{app}"; Flags: uninsneveruninstall

Source: "AirFreight_v4GUI.exe"; DestDir: "{app}"
Source: "GUI.glade"; DestDir: "{app}"
Source: "zint.exe"; DestDir: "{app}"
Source: "Logo.png"; DestDir: "{app}"
Source: "Bilder\*"; DestDir: "{app}\pic"
Source: "bin\*"; DestDir: "{app}\bin"
Source: "lib\*"; DestDir: "{app}\lib"; Flags: recursesubdirs
Source: "share\*"; DestDir: "{app}\share"; Flags: recursesubdirs

[Icons]
Name: "{autoprograms}\AirFreight"; Filename: "{app}\AirFreight_v4GUI.exe" ; Comment: "AirFreight starten"
Name: "{commondesktop}\AirFreight"; Filename: "{app}\AirFreight_v4GUI.exe"; Comment: "AirFreight starten"

[Tasks]
Name: modifypath; Description: "&Add to Path"

[Code]
const
  MODIFY_PATH_TASK_NAME = 'modifypath';

var
  PathIsModified: Boolean;

function DLLAddDirToPath(DirName: string; PathType, AddType: DWORD): DWORD;
  external 'AddDirToPath@files:PathMgr.dll stdcall setuponly';

function DLLRemoveDirFromPath(DirName: string; PathType: DWORD): DWORD;
  external 'RemoveDirFromPath@{app}\PathMgr.dll stdcall uninstallonly';

function AddDirToPath(const DirName: string): DWORD;
var
  PathType, AddType: DWORD;
begin
  if IsAdminInstallMode() then
    PathType := 0
  else
    PathType := 1;
  AddType := 0;
  result := DLLAddDirToPath(DirName, PathType, AddType);
end;

function RemoveDirFromPath(const DirName: string): DWORD;
var
  PathType: DWORD;
begin
  if IsAdminInstallMode() then
    PathType := 0
  else
    PathType := 1;
  result := DLLRemoveDirFromPath(DirName, PathType);
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  if PathIsModified or WizardIsTaskSelected(MODIFY_PATH_TASK_NAME) then
    SetPreviousData(PreviousDataKey, MODIFY_PATH_TASK_NAME, 'true');
end;

function InitializeSetup(): Boolean;
begin
  result := true;
  PathIsModified := GetPreviousData(MODIFY_PATH_TASK_NAME, '') = 'true';
end;

function InitializeUninstall(): Boolean;
begin
  result := true;
  PathIsModified := GetPreviousData(MODIFY_PATH_TASK_NAME, '') = 'true';
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    if PathIsModified or WizardIsTaskSelected(MODIFY_PATH_TASK_NAME) then
      AddDirToPath(ExpandConstant('{app}\bin'));
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
    if PathIsModified then
      RemoveDirFromPath(ExpandConstant('{app}\bin'));
  end;
end;

procedure DeinitializeUninstall();
begin
  UnloadDLL(ExpandConstant('{app}\PathMgr.dll'));
  DeleteFile(ExpandConstant('{app}\PathMgr.dll'));
  RemoveDir(ExpandConstant('{app}'));
end;
