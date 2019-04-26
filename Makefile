bin_name = game

export PKG_CONFIG_PATH=./deps

LIBS=-lglfw3 -lGL -lGLEW -lIL -lILU `pkg-config glfw3 --libs --static`

IDIR=./include
LDIR=./deps

CXX=c++
CXXFLAGS=-g -std=c++14 -Wall -Werror -I$(IDIR)
LDFLAGS=-g -std=c++14 -L$(LDIR) $(LIBS)

SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:%.cpp=%.o)

.PHONY: clean

all: $(bin_name)

src/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(bin_name): $(OBJS)
	export PKG_CONFIG_PATH=./deps
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	@echo "cleaning..."
	@rm -f $(OBJS)
	@rm -f $(bin_name)

