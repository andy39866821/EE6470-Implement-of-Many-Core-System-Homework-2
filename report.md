# EE6470 Homework 2
## Problems andd Solutions
### Fixed point datatype declaration
Q: Failed to declare **sc_uint** array.
```
sc_uint<8> width;
width = i_width.read();
sc_uint<32> data[3][width];
```
A: Set **width** to **static int** type.
```
static int width;
width = i_width.read();
sc_uint<32> data[3][width];
```
## Implementation details 


### Part 1
I used modified lab02: sobel filter and homework 1 to complete this part.
#### Block diagram
![](https://i.imgur.com/TF99fxH.jpg)

#### filter parameters
I copy them from [here](https://lodev.org/cgtutor/filtering.html#Gaussian_Blur_), which is TA provied.

```
double filter[filterHeight][filterWidth] = {
  {0.077847, 0.123317, 0.077847},
  {0.123317, 0.195346, 0.123317},
  {0.077847, 0.123317, 0.077847}
};

```
#### convolution function
This function do the convolution by the data that testbench provided, it doesn't control this IO with testbench, it only do the convolution part.
```
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
```
#### testbench
Just like lab02 showing, testbench control which pixel to be convoluted, then send data to filter, also waiting the result that filter send back.
```
void Testbench::input_data(){
    int offset = 28;
    int i, j, x, y;
    int filterHeight = 3, filterWidth = 3;
    int cnt = 0;

    for (y = 0; y != height; ++y) {
        for (x = 0; x != width; ++x) {
            for (i=-1 ; i<filterHeight-1 ; ++i) {
                for (j=-1 ; j<filterWidth-1 ; ++j) {
                    if(0<=y+i && y+i<height && 0<=x+j && x+j<width) {
                        o_r.write(*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 2));
						o_g.write(*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 1));
						o_b.write(*(image_s + byte_per_pixel * (width * (y+i) + x + j + offset) + 0));
                    }
					else{
 						o_r.write(0);
						o_g.write(0);
						o_b.write(0);
					}
                    cnt += 3;
                }
            }
        }
    }

    cout << "total sent: " << cnt << " pixels" << endl;
}
void Testbench::output_data(){
	int x, y;
	sc_uint<8> R, G, B;
	for (y = 0; y != height; ++y) {
		for (x = 0; x != width; ++x) {
			R = i_r.read();
			G = i_g.read();
			B = i_b.read();
			*(image_t + byte_per_pixel * (width * y + x) + 2) = R;
			*(image_t + byte_per_pixel * (width * y + x) + 1) = G;
			*(image_t + byte_per_pixel * (width * y + x) + 0) = B;
		}
	}
    sc_stop();
}
```
### Part 2
Just like part 1, the changes are data sending order and filter calculating logic.
#### Testbench
Instead of sending 9 pixels for 1 convolutional result needed, it send data row by row to **filter**, then filter will store them for IO saving.
```
for (y = 0; y != height; y++) {
        for (x = 0; x != width; x++) {
            o_r.write(*(image_s + byte_per_pixel * (width * y + x + offset) + 2));
            o_g.write(*(image_s + byte_per_pixel * (width * y + x + offset) + 1));
            o_b.write(*(image_s + byte_per_pixel * (width * y + x + offset) + 0));
            cnt += 3;
        }
    }
```
#### Filter
for stroing rows of data, I set below variable to store 3 needed rows of data. First it read **width** then declare the array.
```
width = i_width.read();
height = i_height.read();

sc_uint<8> source_r[3][width],  source_g[3][width],  source_b[3][width];
```
for every row result to calculate, I first read first row.
```
    for(cnt = 0 ;cnt < width ; cnt++){
        source_r[0][cnt] = source_g[0][cnt] = source_b[0][cnt] = 0;
        source_r[2][cnt] = source_g[2][cnt] = source_b[2][cnt] = 0;
        source_r[1][cnt] = i_r.read();
        source_g[1][cnt] = i_g.read();
        source_b[1][cnt] = i_b.read();
    }

```
Then start the for-loop to calculate result, which will read second row and calculate first row result. After calculating and writing out result, the storing rows will be shifted to clear the unused first row and free up the third row space for next round reading.
```
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
```


## Additional features
### Design architecture
In this homework, testbench controlls which pixel to be calculated, but not filter decides. which make filter is just a convolution calculater with 3 by 3 kernel.

## Experimental results
###  pixel transfering count
I set counters in input_data() of both part1 and part2 to get how many pixels are input to **filter**.
The result shows that part 2 has only **11%** transfered pixels of part1, which has the ligher loading on input funciton.
#### part1
![](https://i.imgur.com/Xyn7TXH.jpg)
#### part2
![](https://i.imgur.com/ArsziQk.jpg)



## Discussions and conclusions
Most of the design architecture are limited by I/O bandwidth, that is, the slowest working frequency device would be the bottleneck of the whole system. Beside the block diagram of system, we also need to consider the data transfering ordering, which can really influence the running time. 