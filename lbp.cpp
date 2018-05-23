#include "lodepng.h"
#include <iostream>

struct image {
    // pixel width of image
    unsigned width;
    // pixel height of image
    unsigned height;
    // all pixels of image, ordered as RGBARGBARGBA...
    std::vector<unsigned char> pixels;
    // not used yet 
    std::vector<unsigned char> masks;

    image() {} 

    // loads png from disk into struct
    image(const char* f) {
        if(lodepng::decode(pixels, width, height, f))
            std::cout << "decode error" << std::endl;
    }

    // saves png to disk from struct
    void save(const char* f) {
        if(lodepng::encode(f, pixels, width, height))
            std::cout << "encode error" << std::endl;
    }

    // returns index for accessing gray value in pixels vector
    unsigned offset(unsigned x, unsigned y) {
        return ((y * (width * 4)) + (x * 4));
    }

    // calculates local binary pattern for image and pushes output to o param
    void lbp(image* o) {
        // convert image to grayscale
        average();

        // reserve enough space for pixel masks
        masks.reserve((width - 2) * (height - 2));

        // output will have one less pixel around border
        o->width = (width - 2);
        o->height = (height - 2);

        unsigned char m, c;
        for(unsigned y = 1; y < (height - 1); y++) {
            for(unsigned x = 1; x < (width - 1); x++) {
                // reset mask value
                m = 0;

                // get intensity of center value
                c = pixels[offset(x, y)];

                // calculate 8-bit mask value from neighboring pixels
                m |= (pixels[offset(x - 1, y - 1)] > c) << 7; // (-1, -1)
                m |= (pixels[offset(x,     y - 1)] > c) << 6; // ( 0, -1)
                m |= (pixels[offset(x + 1, y - 1)] > c) << 5; // ( 1, -1)
                m |= (pixels[offset(x + 1, y    )] > c) << 4; // ( 1,  0)
                m |= (pixels[offset(x + 1, y + 1)] > c) << 3; // ( 1,  1)
                m |= (pixels[offset(x,     y + 1)] > c) << 2; // ( 0,  1)
                m |= (pixels[offset(x - 1, y + 1)] > c) << 1; // (-1,  1)
                m |= (pixels[offset(x - 1, y    )] > c) << 0; // (-1,  0)

                // add mask to vector
                masks.push_back(m);

                // push mask to output image
                o->pixels.push_back(m);
                o->pixels.push_back(m);
                o->pixels.push_back(m);
                o->pixels.push_back(0xff);
            }
        }
    }

    // converts image into grayscale based on rgb average: ((r + g + b) / 3)
    void average() {
        // will hold 8-bit (r, g, b, a) values for pixel
        unsigned r, g, b, a;

        // will hold 8-bit gray value for pixel
        unsigned char n;

        for(unsigned i = 0; i < pixels.size(); i = i + 4) {
            // get r, g, b values for pixel 
            r = (+pixels[i]     & 0xff);
            g = (+pixels[i + 1] & 0xff);
            b = (+pixels[i + 2] & 0xff); 
            
            // calculate the average
            a = ((r + g + b) / 3) + ((r + g + b) % 3);

            // convert unsigned into unsigned char
            n = a & 0xff;

            // set new pixel color to average (gray)
            pixels[i] = n;
            pixels[i + 1] = n;
            pixels[i + 2] = n;
        }
    }
};

int main(void) {
    // load input image
    image i("data/lenna.png");
    
    // output image
    image o;

    // calculate local binary pattern
    i.lbp(&o);

    // save output to disk
    o.save("data/lenna_lbp.png");
  
    return 0;
}
