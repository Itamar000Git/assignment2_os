#ifndef ATOM_WAREHOUSE
#define ATOM_WAREHOUSE
#include <string>


        long long atom_count=0;
        long long carbon_count=0;
        long long hydrogen_count=0;
        long long oxygen_count=0;

    
        void printStock();
        bool isInteger(const std::string& s);

        

        void add_carbon(long long count) {
            carbon_count += count;
            atom_count += count;
        }

        void add_hydrogen(long long count) {
            hydrogen_count += count;
            atom_count += count;
        }

        void add_oxygen(long long count) {
            oxygen_count += count;
            atom_count += count;
        }


void run_server(int port);
int main(int argc, char *argv[]);




#endif