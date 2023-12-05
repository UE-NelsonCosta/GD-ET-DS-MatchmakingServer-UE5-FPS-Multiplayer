@echo off
:: https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/CommandLineArguments/

:: This first part of pathing can be automated refer to the link below
:: https://gist.github.com/Ji-Rath/5eadf6259c61e85ad5382f50023dcfeb
set ExecutableLocation="%~dp0ProjectClient/FPS_Matchmade.exe"
set ClientBaseParameters=-game

:: Optional Stuff
set WindowOffsetX=-WinX=1224
set WindowOffsetY=-WinY=300

::set to 1024x576 (16:9)
set DimensionX=-ResX=1024
set DimensionY=-ResY=576

set Windowed=-Windowed

:: Actual Part Of Sticking It All Together
set GameParams=%OverrideInitialMap% %ClientBaseParameters% %WindowOffsetX% %WindowOffsetY% %DimensionX% %DimensionY% %Windowed%

:: Actually Run The Game Instance
start "" /B %ExecutableLocation% %GameParams%