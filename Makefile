# 用法：
#   make                       # 编译 examples/ 和 mycode/ 下所有 .cpp
#   make run                   # 编译并逐个运行
#   make examples/04_vec3      # 只编译某一个
#   make clean                 # 删除所有产物
#
# 注意：make 会自动发现 examples/ 和 mycode/ 里新增的 .cpp，不用改这个文件。
CXX      ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2

EXAMPLE_SRCS := $(wildcard examples/*.cpp)
MY_SRCS      := $(wildcard mycode/*.cpp)

EXAMPLE_BINS := $(EXAMPLE_SRCS:.cpp=)
MY_BINS      := $(MY_SRCS:.cpp=)
MULTI_DIR    := examples/multifile
MULTI_BIN    := $(MULTI_DIR)/app

BINS := $(EXAMPLE_BINS) $(MY_BINS)

all: $(BINS) $(MULTI_BIN)

# 单个 .cpp → 同名可执行文件
%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# 多文件编译示范
$(MULTI_BIN): $(MULTI_DIR)/main.cpp $(MULTI_DIR)/vec3.cpp $(MULTI_DIR)/vec3.h
	$(CXX) $(CXXFLAGS) $(MULTI_DIR)/main.cpp $(MULTI_DIR)/vec3.cpp -o $@

run: all
	@for b in $(BINS); do \
		echo "========== $$b =========="; \
		./$$b </dev/null; \
		echo; \
	done

clean:
	rm -f $(BINS) $(MULTI_BIN) $(MULTI_DIR)/*.o

.PHONY: all run clean
