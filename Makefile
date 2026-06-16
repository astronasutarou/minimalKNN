CC     := g++
TESTPY := --repository-url https://test.pypi.org/legacy/
LIBS   := -I./src -L./src -lm
CFLAGS := -std=c++11 -g -Wall -O3
CXX    := $(CC) $(LIBS) $(CFLAGS)

HEADER := src/minimalKNN.h
SOURCE := $(wildcard src/*.cc)
OBJECT := $(patsubst %.cc,%.o,$(SOURCE))
EGGDIR := minimalKNN.egg-info
BUILD_DIR := build
BUILD_LIB := $(BUILD_DIR)/lib
BUILD_TEMP := $(BUILD_DIR)/temp
DIST_DIR := $(BUILD_DIR)/dist

.PHONY: clean build build_pypi upload_test upload_pypi

all: test/sample_box

test/sample_%: test/sample_%.cc $(OBJECT) $(HEADER)
	$(CXX) -o $@ $< $(OBJECT)

%.o: %.cc $(HEADER)
	$(CXX) -o $@ -c $<

build:
	python setup.py build_ext \
		--build-lib $(BUILD_LIB) --build-temp $(BUILD_TEMP)

build_pypi: build
	python -m build --outdir $(DIST_DIR)

upload_pypi: build_pypi
	twine upload --skip-existing $(DIST_DIR)/*

clean:
	rm -rf $(BUILD_DIR) $(EGGDIR) $(OBJECT)
