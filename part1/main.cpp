#define SC_INCLUDE_FX // enable the fixed-point data types

#include <iostream>
#include <string>
#include <systemc>
#include <sys/time.h>
#include "filter.h"
#include "testbench.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char *argv[]) {
    Testbench tb("tb");
    Filter filt("filt");

    sc_fifo<sc_uint<8> > source_r;
    sc_fifo<sc_uint<8> > source_g;
    sc_fifo<sc_uint<8> > source_b;
    sc_fifo<sc_uint<8> > result_r;
    sc_fifo<sc_uint<8> > result_g;
    sc_fifo<sc_uint<8> > result_b;
    
    tb.o_r(source_r);
    tb.o_g(source_g);
    tb.o_b(source_b);
    tb.i_r(result_r);
    tb.i_g(result_g);
    tb.i_b(result_b);
    filt.i_r(source_r);
    filt.i_g(source_g);
    filt.i_b(source_b);
    filt.o_r(result_r);
    filt.o_g(result_g);
    filt.o_b(result_b);

    cout << "reading bitmap..." << '\t' << sc_time_stamp()<< endl;
    tb.read_bmp();
    
    cout << "starting simulation..." << '\t' << sc_time_stamp()<< endl;

    sc_start();
    
    cout << "finish simulation" << '\t' << sc_time_stamp()<< endl;
    tb.write_bmp();
    cout << "writing bitmap..." << '\t' << sc_time_stamp()<< endl;
    return (0);
}