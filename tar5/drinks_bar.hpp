
#ifndef MOLECULE_SUPPLIER_HPP
#define MOLECULE_SUPPLIER_HPP
#include <string>
#include <getopt.h>

        long long atom_count=0;
        long long carbon_count=0;
        long long hydrogen_count=0;
        long long oxygen_count=0;
        int timeout= 60;
        bool has_stream_path = false;
        bool has_datagram_path = false;
        std::string stream_path;
        std::string datagram_path;
        static struct option long_options[] = {
        {"oxygen",    required_argument, 0, 'o'},
        {"carbon",    required_argument, 0, 'c'},
        {"hydrogen",  required_argument, 0, 'h'},
        {"timeout",   required_argument, 0, 't'},
        {"tcp-port",  required_argument, 0, 'T'},
        {"udp-port",  required_argument, 0, 'U'},
        {"stream-path", required_argument, 0, 's'},
        {"datagram-path", required_argument, 0, 'd'},    
        {0, 0, 0, 0}
    };

        void cleanup(int signum);
        void printStock();
        bool isInteger(const std::string& s);
        long long get_atom_count() {
            return atom_count;
        }

        long long get_carbon_count() {
            return carbon_count;
        }

        long long get_hydrogen_count() {
            return hydrogen_count;
        }

        long long get_oxygen_count() {
            return oxygen_count;
        }

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

        long long num_of_soft_drinks();
        long long num_of_vodka();
        long long num_of_champagne();

        void run_server(int port, int port_udp);
        void run_server_uds(bool has_datagram_path, bool has_stream_path);
        int main(int argc, char *argv[]);




#endif