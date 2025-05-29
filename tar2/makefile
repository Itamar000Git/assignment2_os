# 
C = g++
CFLAGS = -c -g -Wall -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage -lgcov

OBJDIR = obj_files
COVERAGE_DIR = coverage_files

OBJECTS1 = $(OBJDIR)/molecule_supplier.o
OBJECTS2 = $(OBJDIR)/atom_supplier.o
OBJECTS3 = $(OBJDIR)/molecule_requester.o

TARGET1 = molecule_supplier
TARGET2 = atom_supplier
TARGET3 = molecule_requester

all: $(OBJDIR) $(TARGET1) $(TARGET2) $(TARGET3)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET1): $(OBJECTS1)
	$(C) -o $(TARGET1) $(OBJECTS1) $(LDFLAGS)

$(TARGET2): $(OBJECTS2)
	$(C) -o $(TARGET2) $(OBJECTS2) $(LDFLAGS)

$(TARGET3): $(OBJECTS3)
	$(C) -o $(TARGET3) $(OBJECTS3) $(LDFLAGS)

$(OBJDIR)/molecule_supplier.o: molecule_supplier.cpp molecule_supplier.hpp
	$(C) $(CFLAGS) molecule_supplier.cpp -o $(OBJDIR)/molecule_supplier.o

$(OBJDIR)/atom_supplier.o: atom_supplier.cpp
	$(C) $(CFLAGS) atom_supplier.cpp -o $(OBJDIR)/atom_supplier.o

$(OBJDIR)/molecule_requester.o: molecule_requester.cpp
	$(C) $(CFLAGS) molecule_requester.cpp -o $(OBJDIR)/molecule_requester.o

.PHONY: clean run atom_warehouse molecule_requester atom_supplier atom_supplier coverage

clean:
	rm -rf $(OBJDIR) $(TARGET1) $(TARGET2) $(TARGET3) *.gcda *.gcno *.gcov $(COVERAGE_DIR)

run: $(TARGET1)
	./$(TARGET1)

coverage: $(TARGET1) $(TARGET2) $(TARGET3)
	mkdir -p $(COVERAGE_DIR)
	./$(TARGET1) ./$(TARGET2) ./$(TARGET3)
	gcov molecule_supplier.cpp
	gcov atom_supplier.cpp
	mv *.gcda $(COVERAGE_DIR) 2>/dev/null || true
	mv *.gcno $(COVERAGE_DIR) 2>/dev/null || true
	mv *.gcov $(COVERAGE_DIR) 2>/dev/null || true