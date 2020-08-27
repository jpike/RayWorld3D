CALL D:\tools\Emscripten\emsdk\emsdk_env.bat

REM em++ -v

IF NOT EXIST "build" MKDIR "build"
IF NOT EXIST "build/emscripten" MKDIR "build/emscripten"

REM -x c++ tells the compiler to treat the input files as C++.
em++ code/main.cpp -Icode -I ThirdParty -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -o build/emscripten/index.html --shell-file code/emscripten_shell.html ThirdParty/raylib/libraylib.bc

ECHO Done.
