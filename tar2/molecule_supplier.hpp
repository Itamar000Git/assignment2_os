
#ifndef MOLECULE_SUPPLIER_HPP
#define MOLECULE_SUPPLIER_HPP
#include <string>


        long long atom_count=0;
        long long carbon_count=0;
        long long hydrogen_count=0;
        long long oxygen_count=0;

        void cleanup(int signum);
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

        bool deliver_water(long long count);
        bool deliver_carbon_dioxide(long long count);
        bool deliver_alcohol(long long count);
        bool deliver_glucose(long long count);


void run_server(int port, int port_udp);
int main(int argc, char *argv[]);




#endif