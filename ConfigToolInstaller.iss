; ==================================================================
; 授权配置工具安装包 (ConfigToolInstaller.iss) - 优化版
; = 部署 MonitorConfig, KeyGenTool, SharedAuthDLL
; ==================================================================
[Setup]
AppName=授权配置工具
AppVersion=1.0.0
AppPublisher=您的公司名称
DefaultDirName={pf}\YourCompanyName\AuthTools
OutputDir=.\Output
OutputBaseFilename=AuthToolsInstaller
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; ****** 静默和隐蔽性设置 ******
Uninstallable=yes
; 必须管理员权限
PrivilegesRequired=admin      
; 隐藏安装路径选择页         
DisableDirPage=yes         
; 隐藏快捷方式组选择页            
DisableProgramGroupPage=yes 
; 隐藏欢迎页           
DisableWelcomePage=yes    
; 隐藏安装完成页             
DisableFinishedPage=yes 
; 移除许可文件页               
LicenseFile=                          

; 设置最小安装模式，以减少用户交互
; SetupType=full
; UseSetupGroups=no

[Files]
Source: "D:\project\MonitorService\Release\SharedAuthDLL.dll"; DestDir: "{app}"
Source: "D:\project\MonitorService\Release\MonitorConfig.exe"; DestDir: "{app}"
Source: "D:\project\MonitorService\Release\KeyGenTool.exe"; DestDir: "{app}"
Source: "D:\project\MonitorService\monitor.ico"; DestDir: "{app}"

; ------------------------------------------------------------------
; 快捷方式和图标 (创建可见图标并保留快捷键)
; ------------------------------------------------------------------
[Icons]
; MonitorConfig (配置试用天数)

; 1. 开始菜单程序组图标 (Program Group)
Name: "{group}\配置试用期"; Filename: "{app}\MonitorConfig.exe"; \
    Comment: "配置试用期"; HotKey: "Ctrl+Alt+B"; \
    Flags: runmaximized; IconFilename: "{app}\monitor.ico"

; 2. 桌面快捷方式 (Desktop)
Name: "{userdesktop}\配置试用期"; Filename: "{app}\MonitorConfig.exe"; \
    Comment: "配置试用期"; HotKey: "Ctrl+Alt+B"; \
    Flags: runmaximized; IconFilename: "{app}\monitor.ico"

; KeyGenTool (生成密钥)

; 3. 开始菜单程序组图标 (Program Group)
Name: "{group}\生成密钥"; Filename: "{app}\KeyGenTool.exe"; \
    Comment: "生成客户密钥"; HotKey: "Ctrl+Alt+C"; \
    Flags: runmaximized ; IconFilename: "{app}\monitor.ico"

; 4. 桌面快捷方式 (Desktop)
Name: "{userdesktop}\生成密钥"; Filename: "{app}\KeyGenTool.exe"; \
    Comment: "生成客户密钥"; HotKey: "Ctrl+Alt+C"; \
    Flags: runmaximized; IconFilename: "{app}\monitor.ico"
    
    [UninstallDelete]
; 确保卸载时清理所有安装的文件和目录
Type: filesandordirs; Name: "{app}"

[Run]
; 无特殊运行命令