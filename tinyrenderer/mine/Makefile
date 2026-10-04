# tinyrenderer/mine —— 你自己的实现
#
#   make        编译（自动包含本目录所有 .cpp + 上级的公共文件）
#   make run    编译并运行
#   make clean
#
# 公共文件（tgaimage / model / our_gl）直接复用上级目录的，不用复制过来。
CXX      ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2 -fopenmp

ROOT   := ..
COMMON := $(ROOT)/tgaimage.cpp $(ROOT)/model.cpp $(ROOT)/our_gl.cpp
SRCS   := $(wildcard *.cpp)
BIN    := app

all: $(BIN)

$(BIN): $(SRCS) $(COMMON)
	$(CXX) $(CXXFLAGS) -I$(ROOT) $(SRCS) $(COMMON) -o $(BIN)

run: $(BIN)
	./$(BIN) $(ROOT)/obj/diablo3_pose/diablo3_pose.obj $(ROOT)/obj/floor.obj

clean:
	rm -f $(BIN) framebuffer.tga framebuffer.png

.PHONY: all run clean
