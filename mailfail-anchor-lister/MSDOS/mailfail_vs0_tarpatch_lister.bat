@echo off

set "Partition=vs0"

set "ScriptPath=%~dp0"
set "ScriptPath=%ScriptPath:\=/%"
set "HtmlPath=%ScriptPath%/mailfail_vs0_tarpatch.html"

cd "%~1"
set "StartPath=%cd%"

echo. > %HtmlPath%

SetLocal EnableDelayedExpansion
FOR /f "tokens=*" %%f in ('dir /B /A-D /ON /S "!StartPath!"') DO (
    set "SubDirsAndFiles=%%f"
    set "SubDirsAndFiles=!SubDirsAndFiles:%StartPath%=!"
    set "SubDirsAndFiles=!SubDirsAndFiles:\=/!"
    set "SubDirsAndFiles=!Partition!:/!SubDirsAndFiles:~1!"
    echo ^<a href="email:send?attach=!SubDirsAndFiles!."^>!SubDirsAndFiles!^</a^>^<br /^> >> %HtmlPath%
)