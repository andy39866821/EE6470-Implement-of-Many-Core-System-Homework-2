#include "filter.h"

Filter::Filter(sc_module_name n): sc_module(n){
  SC_THREAD(convolution);
}

void Filter::convolution(){
    sc_uint<8> source_r, source_g, source_b, i ,j;
    sc_fixed<32, 24>  R, G, B;      // color of R, G, B

    while(1) {
        R = G = B = 0;
        for (i=0 ; i!=filterHeight ; ++i) {
            for (j=0 ; j!=filterWidth ; ++j) {
                source_r = i_r.read();
                source_g = i_g.read();
                source_b = i_b.read();

                R += (sc_fixed<32, 24>)source_r * filter[i][j];
                G += (sc_fixed<32, 24>)source_g * filter[i][j];
                B += (sc_fixed<32, 24>)source_b * filter[i][j];
                
            }
        }
        o_r.write((sc_uint<8>)(R/factor));
        o_g.write((sc_uint<8>)(G/factor));
        o_b.write((sc_uint<8>)(B/factor));
    }
    
}
