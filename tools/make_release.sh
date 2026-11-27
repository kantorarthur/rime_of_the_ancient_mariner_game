#!/usr/bin/env bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME=rime_of_the_ancient_mariner
BUILD="$ROOT/build-release"
OUT="$ROOT/release"
STAGE="$OUT/linux/$NAME"

cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD" -j

rm -rf "$OUT/linux" "$OUT/$NAME-linux.zip"
mkdir -p "$STAGE/lib" "$STAGE/licenses"
cp "$BUILD/$NAME" "$STAGE/"
cp -r "$ROOT/assets" "$STAGE/"
cp "$ROOT/README.md" "$ROOT/CREDITS.md" "$STAGE/"

for library in $(ldd "$BUILD/$NAME" | awk '/libglfw|libGLEW/ {print $3}')
do
    cp -L "$library" "$STAGE/lib/"
done

for package in libglfw3 libglew2.2 libglew2.1
do
    if [ -f "/usr/share/doc/$package/copyright" ]
    then
        cp "/usr/share/doc/$package/copyright" "$STAGE/licenses/$package.txt"
    fi
done

cat > "$STAGE/$NAME.sh" <<'EOF'
#!/usr/bin/env bash
cd "$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$PWD/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "./rime_of_the_ancient_mariner" "$@"
EOF
chmod +x "$STAGE/$NAME.sh" "$STAGE/$NAME"

cd "$OUT/linux"
python3 -m zipfile -c "$OUT/$NAME-linux.zip" "$NAME"
rm -rf "$OUT/linux"
echo "created $OUT/$NAME-linux.zip"
