CC     := g++
TESTPY := --repository-url https://test.pypi.org/legacy/
LIBS   := -I./src -L./src -lm
CFLAGS := -std=c++11 -g -Wall -O3
CXX    := $(CC) $(LIBS) $(CFLAGS)

HEADER := src/minimalKNN.h
SOURCE := $(wildcard src/*.cc)
OBJECT := $(patsubst %.cc,%.o,$(SOURCE))

.PHONY: clean build test build_pypi upload_test upload_pypi

all: test/sample_box

test/sample_%: test/sample_%.cc $(OBJECT) $(HEADER)
	$(CXX) -o $@ $< $(OBJECT)

.cc.o: $(HEADER)
	$(CXX) -o $@ -c $<

build:
	python setup.py build_ext --inplace

test: build
	python -c 'import minimalKNN as m; m.simple_demo_box()'

build_pypi: build
	python -m build

upload_test: build_pypi
	twine upload --skip-existing $(TESTPY) dist/*

upload_pypi: build_pypi
	twine upload --skip-existing dist/*

clean:
	rm -r $(OBJECT)
