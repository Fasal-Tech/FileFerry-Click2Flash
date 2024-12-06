@echo off
setlocal enabledelayedexpansion

set "DIRS=User_Files"
set "SOURCES="

rem Find .c, .h, and .cpp files in the specified directories
for /r "%DIRS%" %%f in (*.c) do (
    set "SOURCES=!SOURCES! %%f"
)
for /r "%DIRS%" %%f in (*.h) do (
    set "SOURCES=!SOURCES! %%f"
)
for /r "%DIRS%" %%f in (*.cpp) do (
    set "SOURCES=!SOURCES! %%f"
)

rem Run clang-format with the given arguments and format the found files
for %%f in (!SOURCES!) do (
	echo "formatting %%f"
    clang-format %* -style=file -i "%%f"
)

endlocal

pause
