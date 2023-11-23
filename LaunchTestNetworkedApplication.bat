:: Simple Batch File To Run All Batch Files
:: Launch Server First To Give It Time To Launch
start "" "%~dp0MatchmakingServer/Binaries/Debug_x64/MatchmakingServer.exe" 127.0.0.1 42069

:: Launch Clients That Will Connect To The Server Through The Frontend
:: https://ss64.com/nt/start.html
start /B "" "%~dp0FPS_Matchmade/LaunchTestClient1.bat" 
start /B "" "%~dp0FPS_Matchmade/LaunchTestClient2.bat" 