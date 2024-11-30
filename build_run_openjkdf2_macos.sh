#!/bin/sh

export PATH="${HOMEBREW_PREFIX}/opt/llvm/bin:$PATH"
export MACOSX_DEPLOYMENT_TARGET=10.15
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++

cd /Users/maxamillion/workspace/OpenJKDF2/

export OPENJKDF2_RELEASE_COMMIT=$(git log -1 --format="%H")
export OPENJKDF2_RELEASE_COMMIT_SHORT=$(git rev-parse --short=8 HEAD)

mkdir -p build_darwin64 && cd build_darwin64

export PKG_CONFIG_PATH_OLD=$PKG_CONFIG_PATH
TARGET_BUILD_TESTS=1 DEBUG_QOL_CHEATS=1 OPENJKDF2_NO_ASAN=0 cmake .. &&
TARGET_BUILD_TESTS=1 DEBUG_QOL_CHEATS=1 OPENJKDF2_NO_ASAN=0 make -j10 &&
cd .. &&
#cp resource/shaders/* DF2/resource/shaders/ &&
mkdir -p "/Users/maxamillion/Library/Application Support/OpenJKDF2/openjkdf2/resource/shaders/" &&
cp resource/shaders/* "/Users/maxamillion/Library/Application Support/OpenJKDF2/openjkdf2/resource/shaders/" &&
echo "Running..." &&
#codesign -s - openjkdf2-64 &&
#OPENJKDF2_ROOT="/Users/maxamillion/.local/share/openjkdf2_todoa" LSAN_OPTIONS="suppressions=/Users/maxamillion/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/maxamillion/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
#OPENJKDF2_ROOT="/Users/maxamillion/.local/share/openjkdf2_todoa2012" LSAN_OPTIONS="suppressions=/Users/maxamillion/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/maxamillion/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
#OPENJKDF2_ROOT="/Users/maxamillion/.local/share/openjkdf2-cds" LSAN_OPTIONS="suppressions=/Users/maxamillion/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/maxamillion/workspace/OpenJKDF2/asan.log"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 -- -enhancedCogVerbs #-- -verboseNetworking
LSAN_OPTIONS="suppressions=/Users/maxamillion/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="log_path=/Users/maxamillion/workspace/OpenJKDF2/asan.log detect_leaks=0"  lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 #-- -dedicatedServer -episode "JK1" -map "01narshadda.jkl" #-- -autostart -sp -episode "JK1" -map "01narshadda.jkl" #-- -verboseNetworking
#lldb -o run ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64
#open OpenJKDF2.app


#LSAN_OPTIONS="suppressions=/Users/maxamillion/workspace/OpenJKDF2/suppr.txt" ASAN_OPTIONS="detect_leaks=1:log_path=/Users/maxamillion/workspace/OpenJKDF2/asan.log" ./OpenJKDF2.app/Contents/MacOS/openjkdf2-64 