::===== Main routine ======

:: Optional parameters (to customize the build process from another scripts):

:: %INSTALL_PATH%	- where the OneTouch SDK jars are installed.
:: %BIN_PATH%		- where the DigitalPersona binaries are installed
:: %BUILD_PATH%		- a root of building folder. There will be "build" (for classes) and "dist" (for jars) folders created in it. By default it is a current dir.
:: %CLASSPATH%	- a standard Java classpath
::
:: NOTE: All paths should use BACKSLASHES (\) and should not contain spaces (java.exe doesn't like them)! 

@echo off
setlocal enableextensions
set ERRORCODE=0

set PROJECT_NAME=UISupport
set MAIN_CLASS=com.digitalpersona.onetouch.ui.swing.sample.UISupport.MainForm

set PACKAGE_NAME=%PROJECT_NAME%Sample.jar

if "%INSTALL_PATH%" == "" set INSTALL_PATH=..\..\..\..\bin\Java
if "%BIN_PATH%" == "" set BIN_PATH=%INSTALL_PATH%
if "%BUILD_PATH%" == "" set BUILD_PATH=.
set DISTRO_PATH=%BUILD_PATH%\dist

::call :check_existence "%BIN_PATH%"		otdpfpjni.dll
::call :check_existence "%BIN_PATH%"		otfxjni.dll
::call :check_existence "%BIN_PATH%"		otmcjni.dll
call :add_classpath "%BIN_PATH%" 		dpotjni.jar
call :add_classpath "%INSTALL_PATH%" 	DPOTAPI.jar
call :add_classpath "%INSTALL_PATH%"	DPFPEnrollment.jar
call :add_classpath "%INSTALL_PATH%"	DPFPVerification.jar
if errorlevel 1 goto error

:: *** Check java binaries ***
if "%JAVA_HOME%" == "" set JAVA_HOME=%ProgramFiles%\Java\jre1.5.0_15
set JAVA_BIN=%JAVA_HOME%\bin
if not exist "%JAVA_BIN%\java.exe" goto java_error

set PATH=%PATH%;%INSTALL_PATH%;%BIN_PATH%

@echo on
"%JAVA_BIN%\java.exe" -Djava.library.path="%PATH%" -cp "%DISTRO_PATH%\%PACKAGE_NAME%;%CLASSPATH%" %MAIN_CLASS%
@echo off
if errorlevel 1 goto error

goto quit
::===== End of main routine ======


::===== Checks a path/file for existence, writes an error if failed ======
:check_existence
if not exist "%~1\%~2" (
	echo ERROR: Cannot find "%~2" in "%~1"
	exit /B 1
)
goto :EOF

::===== Checks a path/package for existence, adds the packege to the CLASSPATH, writes an error if failed ======
:add_classpath
call :check_existence %1 %2
if errorlevel 1 exit /B %errorlevel%
set CLASSPATH=%CLASSPATH%;%~1\%~2
goto :EOF

::====== Reports a trouble with Java installation ======
:java_error
echo Cannot find Java installation. Please make sure that your JAVA_HOME environment variable is set to the Java Runtime Engine (JRE) installation directory.
goto error

::====== Common error reporting point ======
:error
if not ERRORLEVEL 0 (set ERRORCODE=%ERRORLEVEL%)
if %ERRORCODE% EQU 0 (set ERRORCODE=1)
goto quit

::====== Exit point ======
:quit
echo.
if %ERRORCODE% NEQ 0 pause
exit /B %ERRORCODE%
