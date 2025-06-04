 
C = g++
CFLAGS = -c -g -Wall -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage -lgcov

COVERAGE_DIR = coverage_files

OBJECTS1 = molecule_supplier.o
OBJECTS2 = atom_supplier.o
OBJECTS3 = molecule_requester.o

TARGET1 = molecule_supplier
TARGET2 = atom_supplier
TARGET3 = molecule_requester

all: $(OBJDIR) $(TARGET1) $(TARGET2) $(TARGET3)


$(TARGET1): $(OBJECTS1)
	$(C) -o $(TARGET1) $(OBJECTS1) $(LDFLAGS)

$(TARGET2): $(OBJECTS2)
	$(C) -o $(TARGET2) $(OBJECTS2) $(LDFLAGS)

$(TARGET3): $(OBJECTS3)
	$(C) -o $(TARGET3) $(OBJECTS3) $(LDFLAGS)

molecule_supplier.o: molecule_supplier.cpp molecule_supplier.hpp
	$(C) $(CFLAGS) molecule_supplier.cpp -o molecule_supplier.o

atom_supplier.o: atom_supplier.cpp
	$(C) $(CFLAGS) atom_supplier.cpp -o atom_supplier.o

molecule_requester.o: molecule_requester.cpp
	$(C) $(CFLAGS) molecule_requester.cpp -o molecule_requester.o

.PHONY: clean molecule_requester all atom_supplier coverage

clean:
	rm -rf $(TARGET1) $(TARGET2) $(TARGET3) *.o *.gcda *.gcno *.gcov $(COVERAGE_DIR)

coverage: $(TARGET1) $(TARGET2) $(TARGET3)
	mkdir -p $(COVERAGE_DIR)
	mv *.gcda $(COVERAGE_DIR) || true
	mv *.gcno $(COVERAGE_DIR) || true
	mv *.gcov $(COVERAGE_DIR) || true