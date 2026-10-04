#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# VS Code 构建助手：编译"当前文件所属的项目"
#
#   tools/build-current.sh <当前文件的绝对路径>
#
# 规则：
#   1) 目录里有 Makefile   → 直接 make（例如 tinyrenderer/mine）
#   2) 否则自动向上查找含 tgaimage.h 的"项目根"（tinyrenderer/）：
#        · 该目录只有一个 main() → 编译该目录所有 .cpp + 项目根的公共实现
#        · 该目录有多个 main()   → 只编译当前文件      + 项目根的公共实现
#   3) 找不到项目根就按普通目录处理（单 main 编全部 / 多 main 只编当前）
#
# 产物统一输出到 <当前文件所在目录>/app
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

# 2) 向上找项目根（含 tgaimage.h 的最近祖先）
INC=""
COMMON=""
OMP=""
p="$DIR"
while [ "$p" != "/" ]; do
    if [ -f "$p/tgaimage.h" ]; then
        if [ "$p" != "$DIR" ]; then
            INC="-I$p"
            OMP="-fopenmp"
            for f in tgaimage.cpp model.cpp our_gl.cpp; do
                [ -f "$p/$f" ] && COMMON="$COMMON $p/$f"
            done
        fi
        break
    fi
    p="$(dirname -- "$p")"
done

# 3) 数一数当前目录有几个 main()
mains="$(grep -lE 'int[[:space:]]+main[[:space:]]*\(' "$DIR"/*.cpp 2>/dev/null || true)"
count="$(printf '%s' "$mains" | grep -c '[.]cpp' || true)"

if [ "$count" -le 1 ]; then
    echo "==> 编译整个目录: $DIR/*.cpp ${COMMON:+(+ 公共文件)}"
    # shellcheck disable=SC2086
    "$CXX" $CXXFLAGS $INC $OMP "$DIR"/*.cpp $COMMON -o "$OUT"
else
    echo "==> 只编译当前文件: $FILE ${COMMON:+(+ 公共文件)}"
    # shellcheck disable=SC2086
    "$CXX" $CXXFLAGS $INC $OMP "$FILE" $COMMON -o "$OUT"
fi
