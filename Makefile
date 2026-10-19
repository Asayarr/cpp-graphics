# 根目录 Makefile —— 只是个入口，实际工作交给 tinyrenderer/mine/Makefile
#
#   make                    编译所有章节
#   make run                编译并运行所有章节
#   make clean              清理所有产物
#   make 07_reflection/app  只编译某一章
#
MINE := tinyrenderer/mine

all:
	$(MAKE) -C $(MINE)

run:
	$(MAKE) -C $(MINE) run

clean:
	$(MAKE) -C $(MINE) clean
	rm -rf tinyrenderer/build

# 其它目标原样转发，比如 `make 07_reflection/app`
%:
	$(MAKE) -C $(MINE) $@

.PHONY: all run clean
