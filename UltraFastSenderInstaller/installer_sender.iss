[Setup]
AppName=UltraFast Sender
AppVersion=1.0
DefaultDirName={autopf}\UltraFastSender
DefaultGroupName=UltraFastSender
OutputBaseFilename=UltraFastSenderSetup
OutputDir=.
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
DisableProgramGroupPage=yes
DisableWelcomePage=yes

[Files]
Source: "UltraFastSender.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; --- Desktop shortcut (All users) ---
Name: "{commondesktop}\UltraFast Sender"; Filename: "{app}\UltraFastSender.exe"

[Run]
; --- Firewall inbound rule add (Sender) ---
Filename: "netsh"; \
 Parameters: "advfirewall firewall add rule name=""UltraFast Sender"" dir=in action=allow program=""{app}\UltraFastSender.exe"" enable=yes profile=public,private"; \
 Flags: runhidden

; --- Auto-run Sender after install ---
Filename: "{app}\UltraFastSender.exe"; \
 Flags: nowait postinstall runascurrentuser

[UninstallRun]
; --- Firewall rule remove on uninstall ---
Filename: "netsh"; \
 Parameters: "advfirewall firewall delete rule name=""UltraFast Sender"""; \
 Flags: runhidden
