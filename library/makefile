STDCXX ?= c++14

CXX ?= g++

CXXFLAGS = -I. -MMD -std=$(STDCXX) -O2 -pedantic -Wall -Wextra -Wshadow -Wno-unused-parameter

TARGET = test.exe

GTEST_LIBS = -lgtest -lgtest_main -pthread

LDFLAGS = $(GTEST_LIBS)

SRCS = test/main.cpp

-include *.d

test: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.d

.PHONY: all clean
