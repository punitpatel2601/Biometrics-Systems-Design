::===== Main routine ======

:: Optional parameters (to customize the build process from another scripts):
::
:: %JAVA_HOME%		- where the Java SDK is intalled. By default it takes a standard JDK 1.5.0_15 installation path
:: %INSTALL_PATH%	- where the DigitalPersona java binaries are installed.
:: %SOURCE_PATH%	- where sample's source files are. All *.java files must be placed under the %SOURCE_PATH%\src folder
:: %SOURCE_FILES%	- list of Java files. If not defined, all java files will be compiled. May be a @listfile (see javac.exe /?)
:: %BUILD_PATH%		- a root of building folder. There will be "build" (for classes) and "dist" (for jars) folders created in it. By default it is a current dir.

:: Variables:

:: %PROJECT_NAME%	- the project name (as the sample's folder name is)
:: %PACKAGE_NAME%	- the package name (as the sample's JAR name will be)
:: %CLASS_PATH%		- semicolon-separated list of paths to libraries used by the package
:: %OUTPUT_PATH%	- where intermediate binary files and resources will be put before packaging
:: %DISTRO_PATH%	- where the resulting package (%PACKAGE_NAME%.jar) will be put.
::
:: NOTE: All paths (except those in java_sources.lst) should use BACKSLASHes (\)
@echo off
setlocal enableextensions
set ERRORCODE=0

set PROJECT_NAME=UISupport

set PACKAGE_NAME=%PROJECT_NAME%Sample
set SOURCE_PATH=.

if "%BUILD_PATH%" == "" set BUILD_PATH=.
set OUTPUT_PATH=%BUILD_PATH%\build\classes
set DISTRO_PATH=%BUILD_PATH%\dist

if "%INSTALL_PATH%" == "" set INSTALL_PATH=..\..\..\..\bin\Java
call :add_classpath "%INSTALL_PATH%" 	DPOTAPI.jar
call :add_classpath "%INSTALL_PATH%"	DPFPEnrollment.jar
call :add_classpath "%INSTALL_PATH%"	DPFPVerification.jar
if errorlevel 1 goto error

:: *** Check java binaries ***
if "%JAVA_HOME%" == "" set JAVA_HOME=%ProgramFiles%\Java\jdk1.5.0_15
set JAVA_BIN=%JAVA_HOME%\bin
if not exist "%JAVA_BIN%\javac.exe" goto java_error

:: *** Prepare a java sources list
del java_sources.lst > nul
for /R "%SOURCE_PATH%" %%i in ("*.java") do @call call :reslash "%%~i"

if "%SOURCE_FILES%" == "" set SOURCE_FILES=@java_sources.lst

:: *** Prepare output directories ***
rmdir /q /s "%OUTPUT_PATH%" > nul
mkdir "%OUTPUT_PATH%" > nul
mkdir "%DISTRO_PATH%" > nul


:: *** Compile classes ***
@echo on
"%JAVA_BIN%\javac.exe" -nowarn   -cp "%OUTPUT_PATH%;%CLASSPATH%"   -d %OUTPUT_PATH%   %SOURCE_FILES%
@echo off
if errorlevel 1 goto error

:: *** Copy all package sources except *.java
@echo .java > exclude_sources.lst
xcopy "%SOURCE_PATH%\src" "%OUTPUT_PATH%" /Y /S /I /EXCLUDE:exclude_sources.lst > nul

:: *** Make a package ***
@echo on
"%JAVA_BIN%\jar.exe" -cmvf "%SOURCE_PATH%\manifest.mf" "%DISTRO_PATH%\%PACKAGE_NAME%.jar" -C "%OUTPUT_PATH%" .
@echo off
if errorlevel 1 goto error

:: *** Sign the package with the default .keystore file ***
::@echo on
::"%JAVA_BIN%\jarsigner.exe" "%DISTRO_PATH%\%PACKAGE_NAME%.jar" -storepass <MYPASSWORD>
::@echo off
::if errorlevel 1 goto error

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


:: ======== Changes backslashes to forward slashes ========
:reslash
@set FN=%~1
@echo "%FN:\=/%" >> java_sources.lst
@goto :EOF

::====== Reports a trouble with Java installation ======
:java_error
@echo Cannot find Java SDK installation. Please make sure that your JAVA_HOME environment variable is set to the Java Development Kit (JDK) installation directory.
goto error

::====== Common error reporting point ======
:error
if not ERRORLEVEL 0 (set ERRORCODE=%ERRORLEVEL%)
if %ERRORCODE% EQU 0 (set ERRORCODE=1)
goto quit

::====== Exit point ======
:quit
:: *** Cleanup ***
if exist "exclude_sources.lst" del exclude_sources.lst > nul
if exist "java_sources.lst" del java_sources.lst > nul
@echo.
exit /B %ERRORCODE%