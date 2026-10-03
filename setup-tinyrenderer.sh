#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# 一键准备 tinyrenderer（含 VS Code / Visual Studio 的 CMake 配置）
#
# 用法:
#   bash setup-tinyrenderer.sh              # 装到 ~/cpp-graphics/tinyrenderer
#   bash setup-tinyrenderer.sh /some/dir    # 装到指定目录
# -----------------------------------------------------------------------------
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "$0")" && pwd)"
TARGET="${1:-$HOME/cpp-graphics/tinyrenderer}"
REPO="https://github.com/ssloy/tinyrenderer.git"

CXX=/usr/bin/g++
CMAKE=/usr/bin/cmake
NINJA=/usr/bin/ninja

if [ ! -d "$TARGET/.git" ]; then
    echo "==> git clone 到 $TARGET"
    git clone --depth 1 "$REPO" "$TARGET"
else
    echo "==> $TARGET 已存在，跳过 clone"
fi

echo "==> 写入编辑器 / CMake 配置 ($TARGET/.vscode/settings.json)"
mkdir -p "$TARGET/.vscode"
cat > "$TARGET/.vscode/settings.json" <<JSON
{
  "cmake.cmakePath": "$CMAKE",
  "cmake.generator": "Ninja",
  "cmake.buildDirectory": "\${workspaceFolder}/build",
  "cmake.configureSettings": {
    "CMAKE_CXX_COMPILER": "$CXX",
    "CMAKE_MAKE_PROGRAM": "$NINJA"
  },
  "C_Cpp.default.compilerPath": "$CXX",
  "C_Cpp.default.cppStandard": "c++20",
  "C_Cpp.default.intelliSenseMode": "linux-gcc-x64"
}
JSON

echo "==> CMake 配置 + 构建（Ninja, Release, OpenMP）"
cmake -B "$TARGET/build" -S "$TARGET" -G Ninja -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER="$CXX" \
      -DCMAKE_MAKE_PROGRAM="$NINJA"
cmake --build "$TARGET/build"

cat <<EOF

============================================================
完成！运行示例：
  cd $TARGET
  ./build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj

把输出转成 PNG 查看：
  python3 $SCRIPT_DIR/tga2png.py framebuffer.tga

在编辑器里打开：
  VS Code  : WSL: Open Folder in WSL -> $TARGET
  VS 2026  : 文件 -> 打开 -> 文件夹 -> \\\\wsl.localhost\\Ubuntu$TARGET
============================================================
EOF
