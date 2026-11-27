#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME=rime_of_the_ancient_mariner
BUILD="$ROOT/build-windows"
DEPS="$BUILD/deps"
OUT="$ROOT/release"
STAGE="$OUT/windows/$NAME"
GLFW_VERSION=3.4
GLEW_VERSION=2.2.0
CXX=x86_64-w64-mingw32-g++-posix
CC=x86_64-w64-mingw32-gcc-posix

mkdir -p "$DEPS" "$BUILD/obj"

if [ ! -d "$DEPS/glfw-$GLFW_VERSION.bin.WIN64" ]
then
    curl -L -o "$DEPS/glfw.zip" "https://github.com/glfw/glfw/releases/download/$GLFW_VERSION/glfw-$GLFW_VERSION.bin.WIN64.zip"
    python3 -m zipfile -e "$DEPS/glfw.zip" "$DEPS"
fi

if [ ! -d "$DEPS/glew-$GLEW_VERSION" ]
then
    curl -L -o "$DEPS/glew.tgz" "https://github.com/nigels-com/glew/releases/download/glew-$GLEW_VERSION/glew-$GLEW_VERSION.tgz"
    tar -xzf "$DEPS/glew.tgz" -C "$DEPS"
fi

mkdir -p "$DEPS/glm"
ln -sfn /usr/include/glm "$DEPS/glm/glm"

GLFW="$DEPS/glfw-$GLFW_VERSION.bin.WIN64"
GLEW="$DEPS/glew-$GLEW_VERSION"
FLAGS="-std=gnu++17 -O2 -DGLEW_STATIC -I$ROOT/src -I$ROOT/external -I$GLFW/include -I$GLEW/include -isystem $DEPS/glm"

objects=()
for source in $(find "$ROOT/src" "$ROOT/external" -name "*.cpp")
do
    object="$BUILD/obj/$(echo "${source#$ROOT/}" | tr '/' '_').o"
    objects+=("$object")
    if [ ! -f "$object" ] || [ "$source" -nt "$object" ]
    then
        echo "compiling ${source#$ROOT/}"
        $CXX $FLAGS -c "$source" -o "$object" &
    fi
done

$CC -O2 -DGLEW_STATIC -I"$GLEW/include" -c "$GLEW/src/glew.c" -o "$BUILD/obj/glew.o"
wait

$CXX -o "$BUILD/$NAME.exe" "${objects[@]}" "$BUILD/obj/glew.o" "$GLFW/lib-mingw-w64/libglfw3.a" \
    -static -static-libgcc -static-libstdc++ -mwindows -lopengl32 -lgdi32 -luser32 -lshell32

rm -rf "$OUT/windows" "$OUT/$NAME-windows.zip"
mkdir -p "$STAGE/licenses"
cp "$BUILD/$NAME.exe" "$STAGE/"
cp -r "$ROOT/assets" "$STAGE/"
cp "$ROOT/README.md" "$ROOT/CREDITS.md" "$STAGE/"
cp "$GLFW/LICENSE.md" "$STAGE/licenses/glfw.txt"
cp "$GLEW/LICENSE.txt" "$STAGE/licenses/glew.txt"

cd "$OUT/windows"
python3 -m zipfile -c "$OUT/$NAME-windows.zip" "$NAME"
rm -rf "$OUT/windows"
echo "created $OUT/$NAME-windows.zip"
