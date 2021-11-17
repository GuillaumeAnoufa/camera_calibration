CFLAGS = -fPIC -Wall -std=c++2a -Wno-deprecated-declarations
ifeq ($(RELEASE), 1)
	CFLAGS += -O2
else
	CFLAGS += -ggdb -Og -DDEBUG
endif

CXX = g++-9

APP_PATH = apps
OBJ_PATH = build
BIN_PATH = bin

OPENCV_INC = -I$(OPENCV_DIR)/include/opencv4
OTHER_INC = -I. -Isrc/
INCLUDE = $(OTHER_INC) $(OPENCV_INC)

OPENCV_LIBS = -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_videoio -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d
OTHER_LIBS 	= -lpthread -lconfig++ -lglog

LIBS = $(QT_LIBS) $(OPENCV_LIBS) $(OTHER_LIBS)
LFLAGS 		= -L$(CUDA_BIN_PATH)/lib -L$(OPENCV_DIR)/lib -L$(QT_PATH)/lib

#---commands alliases---#
.PHONY: clean

#---commands---#
all: test

test: mkdir $(BIN_PATH)/getData $(BIN_PATH)/calib

mkdir:
	mkdir -p $(BUILD_PATH) $(OBJ_PATH) $(BIN_PATH)

$(BIN_PATH)/getData: $(APP_PATH)/getData.cpp
	$(CXX) $(CFLAGS) $^ $(INCLUDE) -o $@  $(LFLAGS) $(LIBS)

$(BIN_PATH)/calib: $(APP_PATH)/calib.cpp
	$(CXX) $(CFLAGS) $^ $(INCLUDE) -o $@  $(LFLAGS) $(LIBS)

#---repository management
clean:
	rm -rvf build/
	rm -rvf bin/