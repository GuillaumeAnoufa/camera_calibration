CFLAGS = -fPIC -Wall -std=c++14 -Wno-deprecated-declarations
ifeq ($(RELEASE), 1)
	CFLAGS += -O2
else
	CFLAGS += -ggdb -Og
endif

CXX = g++

TEST_PATH = tests
APP_PATH = main_apps
OBJ_PATH = build
BIN_PATH = bin

OPENCV_INC = -I$(OPENCV_DIR)/include/opencv4
OTHER_INC = -I.

OPENCV_LIBS = -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_videoio -lopencv_calib3d -lopencv_imgcodecs
OTHER_LIBS 	= -lpthread -lconfig++ -lglog

LIBS = $(QT_LIBS) $(OPENCV_LIBS) $(OTHER_LIBS)
LFLAGS 		= -L$(CUDA_BIN_PATH)/lib -L$(OPENCV_DIR)/lib -L$(QT_PATH)/lib

#---commands alliases---#
.PHONY: clean

#---commands---#
all: test

test: mkdir $(BIN_PATH)/TEST

mkdir:
	mkdir -p $(BUILD_PATH) $(OBJ_PATH) $(BIN_PATH)

#---COMMON---#

COMMON_INCLUDE = $(OTHER_INC) $(OPENCV_INC)
OTHER_OBJECTS =
#---sensorFusion
$(BIN_PATH)/TEST: $(OBJ_PATH)/test.o $(OTHER_OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LFLAGS) $(LIBS)

$(OBJ_PATH)/test.o: $(TEST_PATH)/test.cpp
	$(CXX) $(CFLAGS) -o $@ -c $< $(COMMON_INCLUDE)

#---repository management
clean:
	rm -rvf build/
	rm -rvf bin/