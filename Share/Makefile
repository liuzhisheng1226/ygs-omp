.PHONY: clean all

CC = g++ -O3

CFLAGS = -g -std=c++11

LDFLAGS = -lboost_regex

SRCS = 	Point.cpp \
	PreOrb.cpp \
	RdWrIni.cpp \
	Rect.cpp \
        RMGHeader.cpp \
	RMGBase.cpp \
	RMGImage.cpp \
	SpMatrix.cpp \
	SpStatistics.cpp

INCS = $(SRCS:.cpp=.h)

%.o: %.cpp $(INCS)
	$(CC) $(CFLAGS) -c $< 

OBJS = $(SRCS:.cpp=.o)

TEST_FILE = testRMGImage.cpp

all: test

test: $(OBJS) $(TEST_FILE) 
	$(CC) $^ -o $@ $(LDFLAGS)

clean: 
	rm test *.o
