# tinyrenderer/mine —— 每章一个子目录，各自编译成 <子目录>/app
#
#   make                    编译所有章节
#   make run                编译并运行所有章节
#   make 01_bresenham/app   只编译某一章
#   make clean              清理
#
# 公共文件（tgaimage / model / our_gl）直接复用上级目录的，不用复制过来。
CXX      ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2 -fopenmp

ROOT   := $(abspath ..)
COMMON := $(ROOT)/tgaimage.cpp $(ROOT)/model.cpp $(ROOT)/our_gl.cpp
MODELS := $(ROOT)/obj/diablo3_pose/diablo3_pose.obj $(ROOT)/obj/floor.obj

SUBDIRS := $(patsubst %/main.cpp,%,$(wildcard */main.cpp))
BINS    := $(addsuffix /app,$(SUBDIRS))

all: $(BINS)

%/app: %/main.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -I$(ROOT) $< $(COMMON) -o $@

run: all
	@for b in $(BINS); do \
		echo "========== $$b =========="; \
		(cd $$(dirname $$b) && ./app $(MODELS)); \
		echo; \
	done

clean:
	rm -f $(BINS)
	rm -f */framebuffer.tga */framebuffer.png

.PHONY: all run clean
