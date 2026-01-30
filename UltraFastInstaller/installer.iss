[Setup]
AppName=UltraFast Receiver
AppVersion=1.0
DefaultDirName={autopf}\UltraFastReceiver
DefaultGroupName=UltraFastReceiver
OutputBaseFilename=UltraFastReceiverSetup
OutputDir=.
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
DisableProgramGroupPage=yes
DisableWelcomePage=yes

[Files]
Source: "UltraFastReceiver.exe"; DestDir: "{app}"; Flags: ignoreversion

[Run]
; --- Firewall inbound rule add ---
Filename: "netsh"; \
 Parameters: "advfirewall firewall add rule name=""UltraFast Receiver"" dir=in action=allow program=""{app}\UltraFastReceiver.exe"" enable=yes profile=public,private"; \
 Flags: runhidden

; --- Auto-run UltraFastReceiver after install ---
Filename: "{app}\UltraFastReceiver.exe"; \
 Flags: nowait postinstall runascurrentuser

[UninstallRun]
; --- Firewall rule remove on uninstall ---
Filename: "netsh"; \
 Parameters: "advfirewall firewall delete rule name=""UltraFast Receiver"""; \
 Flags: runhidden
