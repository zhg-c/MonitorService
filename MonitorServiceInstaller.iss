; ==================================================================
; 监控服务安装包 (MonitorServiceInstaller.iss)
; = 部署 MonitorService, MonitorAgent, ActivationHelper, SharedAuthDLL
; ==================================================================
[Setup]
AppName=系统核心监控组件
AppVersion=1.0.0
AppPublisher=您的公司名称
DefaultDirName={pf}\SystemCore\MonitorSvc
OutputDir=.\Output
OutputBaseFilename=MonitorServiceInstaller
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; ****** 安全和隐蔽性设置 ******
; 必须管理员权限
PrivilegesRequired=admin     
; **关键：禁止通过控制面板卸载**            
Uninstallable=no     
; 隐藏安装路径选择页                    
DisableDirPage=yes    
; 隐藏快捷方式组选择页                   
DisableProgramGroupPage=yes    
; 移除许可文件页          
LicenseFile=    

; 定义 ActivationHelper 的隐蔽安装目录
#define HelperDir "{commonappdata}\SystemTools\AuthHelper"                        

[Files]
; 假设您的 Release 目录已整理好
Source: "D:\project\MonitorService\Release\SharedAuthDLL.dll"; DestDir: "{app}"
Source: "D:\project\MonitorService\Release\MonitorService.exe"; DestDir: "{app}"
Source: "D:\project\MonitorService\Release\MonitorAgent.exe"; DestDir: "{app}"

Source: "D:\project\MonitorService\Release\SharedAuthDLL.dll"; DestDir: "{#HelperDir}"
Source: "D:\project\MonitorService\Release\ActivationHelper.exe"; DestDir: "{#HelperDir}"
Source: "D:\project\MonitorService\monitor.ico"; DestDir: "{#HelperDir}"



[Run]
; 注册和启动服务
Filename: "sc.exe"; Parameters: "create MonitorSvc binPath= ""{app}\MonitorService.exe"" start= auto obj= LocalSystem DisplayName= ""System Core Monitor"""; Flags: runhidden
Filename: "sc.exe"; Parameters: "failure MonitorSvc reset= 0 actions= restart/5000/restart/5000/restart/5000"; Flags: runhidden
Filename: "sc.exe"; Parameters: "start MonitorSvc"; Flags: runhidden

; ------------------------------------------------------------------
; 快捷方式和图标 (仅 ActivationHelper)
; ------------------------------------------------------------------
[CustomMessages]
CreateDesktopIcon=创建桌面快捷方式
CreateIcons=创建快捷方式

[Tasks]
; 无

[Icons]
; ActivationHelper 快捷方式：不创建组，不创建桌面图标，但提供快捷键启动
; 部署到 ProgramData (所有用户都能访问，用于 Service 交互)
Name: "{commonprograms}\授权助手"; Filename: "{#HelperDir}\ActivationHelper.exe"; \
    Comment: "授权验证及状态查询"; HotKey: "Ctrl+Alt+A"; Flags: runmaximized; IconFilename: "{#HelperDir}\monitor.ico"

; ------------------------------------------------------------------
; 卸载任务 (无，因为 Uninstallable=no)
; *注意：要删除此服务，需要手动运行 sc stop/delete 命令*
; ------------------------------------------------------------------