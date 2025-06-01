# 
C = g++
CFLAGS = -c -g -Wall -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage -lgcov

OBJDIR = obj_files
COVERAGE_DIR = coverage_files

OBJECTS1 = $(OBJDIR)/atom_warehouse.o
OBJECTS2 = $(OBJDIR)/atom_supplier.o

TARGET1 = atom_warehouse
TARGET2 = atom_supplier

all: $(OBJDIR) $(TARGET1) $(TARGET2)

$(OBJDIR):
	mkdir -p $(OBJDIR)


$(TARGET1): $(OBJECTS1)
	$(C) -o $(TARGET1) $(OBJECTS1) $(LDFLAGS)

$(TARGET2): $(OBJECTS2)
	$(C) -o $(TARGET2) $(OBJECTS2) $(LDFLAGS)

$(OBJDIR)/atom_warehouse.o: atom_warehouse.cpp atom_warehouse.hpp
	$(C) $(CFLAGS) atom_warehouse.cpp -o $(OBJDIR)/atom_warehouse.o

$(OBJDIR)/atom_supplier.o: atom_supplier.cpp
	$(C) $(CFLAGS) atom_supplier.cpp -o $(OBJDIR)/atom_supplier.o

.PHONY: clean run atom_warehouse atom_supplier coverage

clean:
	rm -rf $(OBJDIR) $(TARGET1) $(TARGET2) *.gcda *.gcno *.gcov $(COVERAGE_DIR)

run: $(TARGET1)
	./$(TARGET1)

coverage: $(TARGET1) $(TARGET2)
	mkdir -p $(COVERAGE_DIR)
	./$(TARGET1) ./$(TARGET2)
	gcov atom_warehouse.cpp
	gcov atom_supplier.cpp
	mv *.gcda $(COVERAGE_DIR) 2>/dev/null || true
	mv *.gcno $(COVERAGE_DIR) 2>/dev/null || true
	mv *.gcov $(COVERAGE_DIR) 2>/dev/null || true