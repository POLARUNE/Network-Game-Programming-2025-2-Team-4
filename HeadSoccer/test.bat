@echo off
rem 명령어를 화면에 출력하지 않음

rem --- 변수 설정 (경로 확인 필수!) ---
set "SOURCE_DIR=WP Final Project\x64\Release"
set "TARGET_DIR=WP Final Project\WP Final Project"
set "FILENAME=WP Final Project.exe"
set "FULL_TARGET_PATH=%TARGET_DIR%\%FILENAME%"
rem --------------------------------------

:MENU
echo.
echo ===================================================
echo               작업 메뉴 선택
echo ===================================================
echo [1] : 클라이언트 파일 복사
echo [2] : Target 폴더 이동 후 EXE 파일 실행
echo [3] : 프로그램 종료
echo ===================================================
echo.

:GET_INPUT
rem 사용자로부터 1, 2, 또는 3의 입력값을 받습니다.
set /p CHOICE="원하는 작업 번호(1, 2, 3)를 입력하세요: "

rem 입력값 유효성 검사 및 분기
if "%CHOICE%"=="1" goto :TASK_1
if "%CHOICE%"=="2" goto :TASK_2
if "%CHOICE%"=="3" goto :END_PROGRAM

echo.
echo 오류: 1, 2, 3 중 하나만 입력해 주세요.
goto :GET_INPUT


:TASK_1
echo.
echo [1번 작업: 파일 복사]를 시작합니다.
echo 소스: %SOURCE_DIR%
echo 대상: %TARGET_DIR%

rem 대상 디렉토리가 없으면 생성합니다.
if not exist "%TARGET_DIR%" (
    echo 대상 폴더 생성 중...
    mkdir "%TARGET_DIR%"
)

rem 파일 복사 실행
copy "%SOURCE_DIR%\%FILENAME%" "%TARGET_DIR%"

if exist "%FULL_TARGET_PATH%" (
    echo.
    echo 복사가 완료되었습니다.
) else (
    echo.
    echo 복사에 실패했습니다. 경로를 확인해 주세요.
)
goto :WAIT_AND_RETURN


:TASK_2
echo.
echo [2번 작업: 파일 실행]을 시작합니다.

rem 대상 디렉토리로 이동합니다.
cd "%TARGET_DIR%"
echo 현재 디렉토리: %CD%

rem EXE 파일을 실행합니다.
if exist "%FILENAME%" (
    echo "%FILENAME%" 파일을 실행합니다...
    start "" "%FILENAME%"
) else (
    echo 오류: 실행 파일이 대상 폴더에 없습니다. 먼저 1번 작업을 수행하세요.
)
goto :WAIT_AND_RETURN


:WAIT_AND_RETURN
echo.
echo 작업이 완료되었습니다. 메뉴로 돌아가려면 아무 키나 누르십시오...
pause >nul
cls
goto :MENU


:END_PROGRAM
echo.
echo 프로그램을 종료합니다.
pause