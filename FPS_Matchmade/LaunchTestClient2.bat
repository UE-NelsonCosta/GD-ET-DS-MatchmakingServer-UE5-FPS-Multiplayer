@echo off
:: https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/CommandLineArguments/

:: This first part of pathing can be automated refer to the link below
:: https://gist.github.com/Ji-Rath/5eadf6259c61e85ad5382f50023dcfeb
set EditorLocation="C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe"
set UProjectLocation="C:\Repositories\GD-ET-DS-MatchmakingServer-UE5-FPS-Multiplayer\FPS_Matchmade\FPS_Matchmade.uproject"
set ClientBaseParameters=-game

:: If you need to override, pass a parameter with the map when you execute this batch file
:: You can pass an IP and port if you are just debugging some Server Client Stuff
set OverrideInitialMap=%1

:: Optional Stuff
set WindowOffsetX=-WinX=1224
set WindowOffsetY=-WinY=300

::set to 1024x576 (16:9)
set DimensionX=-ResX=1024
set DimensionY=-ResY=576

set Windowed=-Windowed

:: For Debugging Purposes

::"-D3DDEBUG"
set UseD3DDebugMode=
::"-LOG"
set ShowLog=
set ShowFPS="-FPS"

:: Actual Part Of Sticking It All Together
set GameParams=%OverrideInitialMap% %ClientBaseParameters% %UseD3DDebugMode% %ShowLog% %ShowFPS% %WindowOffsetX% %WindowOffsetY% %DimensionX% %DimensionY% %Windowed%

:: Actually Run The Game Instance
%EditorLocation% %UProjectLocation% %GameParams%