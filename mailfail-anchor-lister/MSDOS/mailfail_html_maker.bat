@echo off

set "ScriptPath=%~dp0"
set "HtmlPath=%ScriptPath%..\build\mailfail\index.html"
echo %HtmlPath%

mkdir %ScriptPath%..\build
mkdir %ScriptPath%..\build\mailfail

echo ^<!DOCTYPE html^>^<html^>^<body^> > %HtmlPath%

type "%ScriptPath%mailfail_os0.html" >> %HtmlPath%
type "%ScriptPath%mailfail_vs0.html" >> %HtmlPath%
type "%ScriptPath%mailfail_vs0_tarpatch.html" >> %HtmlPath%
type "%ScriptPath%mailfail_sa0.html" >> %HtmlPath%
type "%ScriptPath%mailfail_vd0.html" >> %HtmlPath%
type "%ScriptPath%mailfail_tm0.html" >> %HtmlPath%
type "%ScriptPath%mailfail_ur0.html" >> %HtmlPath%

echo ^</body^>^</html^> >> %HtmlPath%