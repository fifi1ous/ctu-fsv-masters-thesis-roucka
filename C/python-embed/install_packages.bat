@echo off
REM ============================================================
REM  Install pdf2image, pypdf, ultralytics, Pillow into
REM  a Python embeddable distribution.
REM
REM  HOW TO USE:
REM  1. Place this .bat file inside your Python embed folder
REM     (the folder that contains python.exe)
REM  2. Double-click it, or run it from cmd.
REM ============================================================

cd /d "%~dp0"

echo.
echo === Step 1: Enabling site-packages in ._pth file ===
echo.

REM Uncomment "import site" in any pythonXX._pth file
for %%F in (python*._pth) do (
    echo Patching %%F
    powershell -NoProfile -Command "(Get-Content '%%F') -replace '^#import site','import site' | Set-Content '%%F'"
)

echo.
echo === Step 2: Downloading get-pip.py ===
echo.

if not exist get-pip.py (
    curl -O https://bootstrap.pypa.io/get-pip.py
) else (
    echo get-pip.py already present, skipping download.
)

echo.
echo === Step 3: Installing pip ===
echo.
python.exe get-pip.py --no-warn-script-location

echo.
echo === Step 4: Installing packages ===
echo.
python.exe -m pip install --upgrade pip
python.exe -m pip install pdf2image pypdf ultralytics Pillow --target=".\Lib\site-packages" --no-warn-script-location

echo.
echo === Step 5: Verifying installation ===
echo.
python.exe -c "import pdf2image, pypdf, ultralytics, PIL; print('All packages imported successfully!')"

echo.
echo === DONE ===
echo.
echo NOTE: pdf2image also requires Poppler binaries (not a pip package).
echo Download from: https://github.com/oschwartz10612/poppler-windows/releases
echo Extract it and either add its \bin folder to PATH, or pass
echo poppler_path=r"C:\path\to\poppler\bin" to convert_from_path().
echo.
pause
