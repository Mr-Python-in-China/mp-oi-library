STDCXX ?= c++14

CXX ?= g++

CXXFLAGS = -Wall -Wextra -g -I. -MMD -std=$(STDCXX)

TARGET = test.exe

GTEST_LIBS = -lgtest -lgtest_main -pthread

LDFLAGS = $(GTEST_LIBS)

SRCS = test/main.cpp

-include *.d

test: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
