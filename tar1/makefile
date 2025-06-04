
C = g++
CFLAGS = -c -g -Wall -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage -lgcov


COVERAGE_DIR = coverage_files

OBJECTS1 = atom_warehouse.o
OBJECTS2 = atom_supplier.o

TARGET1 = atom_warehouse
TARGET2 = atom_supplier

all: $(OBJDIR) $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS1)
	$(C) -o $(TARGET1) $(OBJECTS1) $(LDFLAGS)

$(TARGET2): $(OBJECTS2)
	$(C) -o $(TARGET2) $(OBJECTS2) $(LDFLAGS)

atom_warehouse.o: atom_warehouse.cpp atom_warehouse.hpp
	$(C) $(CFLAGS) atom_warehouse.cpp -o atom_warehouse.o

atom_supplier.o: atom_supplier.cpp
	$(C) $(CFLAGS) atom_supplier.cpp -o atom_supplier.o

.PHONY: clean atom_warehouse atom_supplier coverage

clean:
	rm -rf *.o $(TARGET1) $(TARGET2) *.gcda *.gcno *.gcov $(COVERAGE_DIR)

coverage: $(TARGET1) $(TARGET2)
	mkdir -p $(COVERAGE_DIR)
	mv *.gcda $(COVERAGE_DIR)|| true
	mv *.gcno $(COVERAGE_DIR)|| true
	mv *.gcov $(COVERAGE_DIR)|| true

