rem Batch script to create document based on Doxygen and Latex  

@echo off
rem Check if Doxyfile exists
if not exist Doxyfile (
    echo Doxyfile not found!
	pause
)

set PROJECT_NAME=FasalFlasher

set DESIGN_DOC=Design_Document\%PROJECT_NAME%_FirmwareDesignDocument.pdf

rem running Doxygen 
echo "Running Doxygen"
doxygen Doxyfile

rem running make.bat inside Docs/latex
echo "Running make.bat to generate latex file"
cd Docs\latex
call make.bat
cd ..

rem Check if the output PDF exists and rename it
echo "Create design document from generated latex pdf file"
if exist latex\refman.pdf (
    move latex\refman.pdf %DESIGN_DOC%
    echo PDF has been renamed and moved to  %DESIGN_DOC%
) else (
    echo PDF generation failed or refman.pdf not found!
	pause
)

pause
