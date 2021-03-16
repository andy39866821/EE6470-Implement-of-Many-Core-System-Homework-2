#include "filter.h"

Filter::Filter(sc_module_name n): sc_module(n){
  SC_THREAD(convolution);
}

void Filter::convolution(){
    static int width = 0;
    static int height = 0;
    int i, j, x, y, cnt;

    width = i_width.read();
    height = i_height.read();

    sc_uint<8> source_r[3][width],  source_g[3][width],  source_b[3][width];
    sc_fixed<32, 24>  R, G, B;      // color of R, G, B
    
    for(cnt = 0 ;cnt < width ; cnt++){
        source_r[0][cnt] = source_g[0][cnt] = source_b[0][cnt] = 0;
        source_r[2][cnt] = source_g[2][cnt] = source_b[2][cnt] = 0;
        source_r[1][cnt] = i_r.read();
        source_g[1][cnt] = i_g.read();
        source_b[1][cnt] = i_b.read();
    }

    for(y = 0 ; y < height ; y++) {    
        if(y < height - 1){
            for(cnt = 0 ;cnt < width ; cnt++) { //reading
                source_r[2][cnt] = i_r.read();
                source_g[2][cnt] = i_g.read();
                source_b[2][cnt] = i_b.read();
            }
        }
        
        for(x = 0 ; x < width ; x++){
            R = G = B = 0;
            for (i = -1 ; i < filterHeight - 1 ; i++) {
                for (j = -1 ; j < filterWidth - 1 ; j++) {
                    if(0<=y+i && y+i<height && 0<=x+j && x+j<width){
                        R += (sc_fixed<32, 24>)source_r[i+1][x+j+1] * filter[i+1][j+1];
                        G += (sc_fixed<32, 24>)source_g[i+1][x+j+1] * filter[i+1][j+1];
                        B += (sc_fixed<32, 24>)source_b[i+1][x+j+1] * filter[i+1][j+1];
                    }
                }
            }
            o_r.write((sc_uint<8>)(R/factor));
            o_g.write((sc_uint<8>)(G/factor));
            o_b.write((sc_uint<8>)(B/factor));
            
        }
        for(x = 0 ;x < width ; x++) { //shifting
            source_r[0][x] = source_r[1][x];
            source_g[0][x] = source_g[1][x];
            source_b[0][x] = source_b[1][x];
            source_r[1][x] = source_r[2][x];
            source_g[1][x] = source_g[2][x];
            source_b[1][x] = source_b[2][x];
            source_r[2][x] = source_g[2][x] = source_b[2][x] = 0;
        }
        
    }
    
}
