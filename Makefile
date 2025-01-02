# 定义编译器
CXX = g++
CXXFLAGS = -std=c++1z -O2 -Wall
LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp -lpthread

# 定义目标文件
TARGET = renju
# 定义源文件
SRCS = main.cpp AI.cpp AIAlgorithms.cpp Board.cpp Evaluate.cpp ForbiddenMove.cpp GameManager.cpp
# 定义对象文件
OBJS = $(SRCS:.cpp=.o)


# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标
.PHONY: all clean