@echo off
:: https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/CommandLineArguments/

:: This first part of pathing can be automated refer to the link below
:: https://gist.github.com/Ji-Rath/5eadf6259c61e85ad5382f50023dcfeb
set EditorLocation="C:\Program Files\Epic Games\UE_5.2\Engine\Binaries\Win64\UnrealEditor.exe"
set UProjectLocation="%~dp0FPS_Matchmade\FPS_Matchmade.uproject"
set ServerBaseParameters=-server -log

:: If you need to override, pass a parameter with the map when you execute this batch file
:: This Should Be Configured To Where The Game Should Start, If It's a Single Gamemode you can set it up via the editor
set OverrideInitialMap=%1%

:: We only need to configure a open port to use, as the IP is bound to 0.0.0.0 which means it can bind to whatever is available
set Port=-port=7777
:: We Override With Parameter 2 If There Is Something Special To Do So
if not "%2"=="" set Port=-port=%2%

:: Actual Part Of Sticking It All Together
set GameParams=%OverrideInitialMap% %ServerBaseParameters% %Port%

:: Actually Run The Game Instance
%EditorLocation% %UProjectLocation% %GameParams%