#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# VS Code 构建助手：编译"当前文件所属的项目"
#
#   tools/build-current.sh <当前文件的绝对路径>
#
# 规则：
#   1) 目录里有 Makefile        → 直接 make（例如 tinyrenderer/mine）
#   2) 目录里只有一个 main()    → 把该目录所有 .cpp 当一个项目一起编译
#   3) 目录里有多个 main()      → 只编译当前文件（例如 examples/、mycode/ 里放多个练习）
#
# 产物统一输出到 <目录>/app，方便 F5 用固定路径启动。
# -----------------------------------------------------------------------------
set -euo pipefail

FILE="${1:?用法: build-current.sh <file.cpp>}"
DIR="$(cd -- "$(dirname -- "$FILE")" && pwd)"
CXX="${CXX:-/usr/bin/g++}"
CXXFLAGS="${CXXFLAGS:--std=c++20 -Wall -Wextra -g -O0}"

# 1) 有 Makefile 就交给 make
if [ -f "$DIR/Makefile" ]; then
    echo "==> make -C $DIR"
    make -C "$DIR"
    exit 0
fi

OUT="$DIR/app"

# 2) 数一数同目录下有几个文件定义了 main
mains="$(grep -lE 'int[[:space:]]+main[[:space:]]*\(' "$DIR"/*.cpp 2>/dev/null || true)"
count="$(printf '%s' "$mains" | grep -c '[.]cpp' || true)"

if [ "$count" -le 1 ]; then
    # 目录整体当成一个项目
    echo "==> 编译整个目录: $DIR/*.cpp -> $OUT"
    # shellcheck disable=SC2086
    "$CXX" $CXXFLAGS "$DIR"/*.cpp -o "$OUT"
else
    # 多个 main，只编译当前文件
    echo "==> 只编译当前文件: $FILE -> $OUT"
    # shellcheck disable=SC2086
    "$CXX" $CXXFLAGS "$FILE" -o "$OUT"
fi
