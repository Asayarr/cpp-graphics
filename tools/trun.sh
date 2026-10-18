#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# trun —— 编译 + 运行 + 转 PNG，一条命令搞定
#
#   trun 02                 # 编译并运行 02_ 开头的章节（前缀匹配），默认用人脸模型
#   trun 02 diablo3_pose    # 换成恶魔模型
#   trun 02 floor           # 用 floor.obj
#   trun                    # 不带参数 = 运行最近改过的一章
#
# 安装：见同目录 README，或
#   ln -s ~/cpp-graphics/tools/trun.sh ~/.local/bin/trun
# -----------------------------------------------------------------------------
set -euo pipefail

MINE="$HOME/cpp-graphics/tinyrenderer/mine"
ROOT="$(cd "$MINE/.." && pwd)"
T2P="$HOME/cpp-graphics/tga2png.py"

# 第二个参数选模型，默认用教程里的 african_head（人脸）
MODEL_NAME="${2:-african_head}"
if [ "$MODEL_NAME" = "floor" ]; then
    MODEL="$ROOT/obj/floor.obj"
else
    MODEL="$ROOT/obj/$MODEL_NAME/$MODEL_NAME.obj"
fi

list_chapters() { ls -d "$MINE"/*/ 2>/dev/null | xargs -n1 basename; }

CH="${1:-}"

if [ -z "$CH" ]; then
    CH="$(ls -dt "$MINE"/*/ 2>/dev/null | head -1 | xargs -n1 basename)"   # 默认：最近改过的
    echo "==> 未指定章节，用最近修改的: $CH"
elif [ ! -d "$MINE/$CH" ]; then
    matches="$(list_chapters | grep "^${CH}" || true)"
    n="$(printf '%s' "$matches" | grep -c . || true)"
    if [ "$n" -eq 1 ]; then
        CH="$matches"
    elif [ "$n" -gt 1 ]; then
        echo "前缀 '$1' 匹配到多个章节："; echo "$matches"; exit 1
    else
        echo "找不到章节 '$1'。现有章节："; list_chapters; exit 1
    fi
fi

echo "==> 编译 $CH"
make -C "$MINE" "$CH/app"

echo "==> 运行（模型: $MODEL_NAME）"
cd "$MINE/$CH"
./app "$MODEL"

echo "==> 转 PNG"
made=0
for f in framebuffer.tga zbuffer.tga; do
    if [ -f "$f" ]; then
        python3 "$T2P" "$f"
        made=1
    fi
done
[ "$made" -eq 1 ] || echo "  (程序没有生成 .tga 文件)"

echo
echo "结果目录: $MINE/$CH/"
ls -1 "$MINE/$CH"/*.png 2>/dev/null | sed 's|.*/|  |' || true
