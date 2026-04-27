/*

Quick and dirty program to solve the "Melbourne Sofitel Lifts (Elevators)" problem
Six elevators are arranged in an L shape as below. Where is the best place to stand?

#
# y=8
#



#
# y=5
#



#
# y=2
#

      x=2            x=5              x=8
     #####          #####            #####

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cmath>
#include <string>

void make_bmp();
void make_gradients();
static unsigned char pixels[800*3*800];
static unsigned char red_gradient  [512];
static unsigned char blue_gradient [512];
static unsigned char green_gradient[512];

/*

Distance to each list is as follows;

    d1 = sqrt( (x-x1)^2 + (y1-y)^2 )
       = sqrt( (x-x1)^2 + (y-y1)^2 )
    d2 = sqrt( (x-x2)^2 + (y-y2)^2 )
    d3 = sqrt( (x-x3)^2 + (y-y3)^2 )

    d4 = sqrt( (x-x4)^2 + (y-y4)^2 )
    d5 = sqrt( (x-x5)^2 + (y-y5)^2 )
    d6 = sqrt( (x-x6)^2 + (y-y6)^2 )

Minimise the average distance to a lift

*/

static const double x1 = 2.0;
static const double x2 = 5.0;
static const double x3 = 8.0;
static const double x4 = 0.0;
static const double x5 = 0.0;
static const double x6 = 0.0;
static const double Y1 = 0.0;
static const double y2 = 0.0;
static const double y3 = 0.0;
static const double y4 = 2.0;
static const double y5 = 5.0;
static const double y6 = 8.0;

double avg_distance( double x, double y )
{
    double d1 = sqrt( (x-x1) * (x-x1) + (y-Y1) * (y-Y1) );
    double d2 = sqrt( (x-x2) * (x-x2) + (y-y2) * (y-y2) );
    double d3 = sqrt( (x-x3) * (x-x3) + (y-y3) * (y-y3) );
    double d4 = sqrt( (x-x4) * (x-x4) + (y-y4) * (y-y4) );
    double d5 = sqrt( (x-x5) * (x-x5) + (y-y5) * (y-y5) );
    double d6 = sqrt( (x-x6) * (x-x6) + (y-y6) * (y-y6) );
    double distance = (d1+d2+d3+d4+d5+d6)/6;
    return distance;
}

// For the high precision calculation, use symmetry and only
//  consider points on the x=y 45 degree line
void high_precision() 
{
    double min_x = 0.0;
    double min_dist = 1000000.0;
    for( int i=0; i<=8000000; i++ )
    {
        double x = i/1000000.0;
        double dist = avg_distance(x,x);
        if( dist < min_dist )
        {
            min_x    = x;
            min_dist = dist;
        }
    }
    printf( "High precision calculation, assuming hot point is on the x==y line; x=y= %f, dist=%f\n", min_x, min_dist );
}

//
// Make a nice heat map
//
int main()
{

    // Show high precision results
    high_precision();

    // Make a nice colour gradient
    make_gradients();

    // Traverse every square centimetre
    // First, find the optimum square centimetre (should contain high precision optimum)
    double lowest_dist  = 8.00;
    double lowest_x     = 0.0;
    double lowest_y     = 0.0;
    double highest_dist = 0.00;
    double highest_x    = 0.0;
    double highest_y    = 0.0;
    int lowest_i = 0;
    int lowest_j = 0;

    for( int i=0; i<800; i++ )
    {
        double y = (i*1.0 / 100.0);
        for( int j=0; j<800; j++ )
        {
            double x = (j*1.0 / 100.0);
            double val = avg_distance( x, y );
            if( val < lowest_dist )
            {
                lowest_dist = val;
                lowest_x    = x;
                lowest_y    = y;
                lowest_i    = i;
                lowest_j    = j;
            }
            if( val > highest_dist )
            {
                highest_dist = val;
                highest_x    = x;
                highest_y    = y;
            }
        }
    }

    printf( "lowest_dist = %.5f\n",  lowest_dist );
    printf( "lowest_x = %.5f\n",     lowest_x );
    printf( "lowest_y = %.5f\n",     lowest_y );

    printf( "highest_dist = %.5f\n",  highest_dist );
    printf( "highest_x = %.5f\n",     highest_x );
    printf( "highest_y = %.5f\n",     highest_y );

    // Second, make the bitmap
    for( int i=0; i<800; i++ )
    {
        double y = (i*1.0 / 100.0);
        for( int j=0; j<800; j++ )
        {
            double x = (j*1.0 / 100.0);
            double val = avg_distance( x, y );
            int sz = 3;
            bool cross = (i==lowest_i && lowest_j-sz<=j && j<=lowest_j+sz ) ||
                         (j==lowest_j && lowest_i-sz<=i && i<=lowest_i+sz );
            double heat = (val-lowest_dist) / (highest_dist-lowest_dist);    // 0-1   smaller is hotter
            heat *= 512.0;
            int idx = (int)heat;
            if( idx > 511 )
                idx = 511;
            if( idx < 0 )
                idx = 0;
            int offset = i*800*3 + j*3;
            unsigned char *p = &pixels[offset];
            double delta = 0.01;
            double delta4 = 0.0015;
            bool isotherm =
                (4.0-delta4 < val && val < 4.0+delta4) ||
                (5.0-delta < val && val < 5.0+delta) ||
                (6.0-delta < val && val < 6.0+delta) ||
                (7.0-delta < val && val < 7.0+delta) ||
                (8.0-delta < val && val < 8.0+delta);
            bool cartesian = (i%100==0 || j%100==0) && i!=0 && j!=0;
            if( i==0 && j%100==0 && j!=0 )
                cartesian = true;
            if( j==0 && i%100==0 && i!=0 )
                cartesian = true;
            if( cartesian || isotherm )
            {
                // White
                *p++ = 255;
                *p++ = 255;
                *p++ = 255;
            }
            else if( cross )
            {
                // Green
                *p++ = 0;
                *p++ = 255;
                *p++ = 0;
            }
            else
            {
                *p++ = blue_gradient [idx];
                *p++ = green_gradient[idx];
                *p++ = red_gradient  [idx];
            }
        }

    }
    make_bmp();
    return 0;

    // Not used at the moment - show every square centimetre in text form
    char buf[200];
    char buf2[200];
    std::string head;
    sprintf( buf2, "%8s ",  " " );
    head += buf2;
    for( int j=0; j<800; j++ )
    {
        double x = (j*1.0 / 100.0);
        sprintf( buf,  "%.5f", x );
        sprintf( buf2, "%8s",  buf );
        head += buf2;
    }
    printf( "%s\n", head.c_str() );
    for( int i=0; i<800; i++ )
    {
        double y = (i*1.0 / 100.0);
        std::string row;
        sprintf( buf,  "%.5f",  y );
        sprintf( buf2, "%8s:",  buf );
        row += buf2;
        for( int j=0; j<800; j++ )
        {
            double x = (j*1.0 / 100.0);
            double val = avg_distance( x, y );
            sprintf( buf,  "%.5f", val );
            sprintf( buf2, "%8s",  buf );
            row += buf2;
        }
        printf( "%s\n", row.c_str() );
    }
    return 0;
}


// From Wikipedia Following is an example of a 2×2 pixel, 24-bit bitmap (Windows DIB header BITMAPINFOHEADER) with pixel format RGB24.

static unsigned char bmp_example[] = {
/* 00 2 */ 0x42, 0x4D,              // "BM" ID field (42 4D)
/* 02 4 */ 0x46, 0x00, 0x00, 0x00,  // 70 bytes (54+16) Size of the BMP file (54 bytes header + 16 bytes data)
/* 06 2 */ 0x00, 0x00,              // Unused   Application specific
/* 08 2 */ 0x00, 0x00,              // Unused   Application specific
/* 0A 4 */ 0x36, 0x00, 0x00, 0x00,  // 54 bytes (14+40) Offset where the pixel array (bitmap data) can be found
                                    // DIB Header
/* 0E 4 */ 0x28, 0x00, 0x00, 0x00,  // 40 bytes Number of bytes in the DIB header (from this point)
/* 12 4 */ 0x02, 0x00, 0x00, 0x00,  // 2 pixels (left to right order)   Width of the bitmap in pixels
/* 16 4 */ 0x02, 0x00, 0x00, 0x00,  // 2 pixels (bottom to top order)   Height of the bitmap in pixels. Positive for bottom to top pixel order.
/* 1A 2 */ 0x01, 0x00,              // 1 plane  Number of color planes being used
/* 1C 2 */ 0x18, 0x00,              // 24 bits  Number of bits per pixel
/* 1E 4 */ 0x00, 0x00, 0x00, 0x00,  // 0 BI_RGB, no pixel array compression used
/* 22 4 */ 0x10, 0x00, 0x00, 0x00,  // 16 bytes Size of the raw bitmap data (including padding)
/* 26 4 */ 0x13, 0x0B, 0x00, 0x00,  // 2835 pixels/metre horizontal Print resolution of the image,
                                    // 72 DPI × 39.3701 inches per metre yields 2834.6472
/* 2A 4 */ 0x13, 0x0B, 0x00, 0x00,  // 2835 pixels/metre vertical
/* 2E 4 */ 0x00, 0x00, 0x00, 0x00,  // 0 colors Number of colors in the palette
/* 32 4 */ 0x00, 0x00, 0x00, 0x00,  // 0 important colors   0 means all colors are important
                                    // Start of pixel array (bitmap data)
/* 36 3 */ 0x00, 0x00, 0xFF,        // 0 0 255  Red, Pixel (x=0, y=1)
/* 39 3 */ 0xFF, 0xFF, 0xFF,        // 255 255 255  White, Pixel (x=1, y=1)
/* 3C 2 */ 0x00, 0x00,              // 0 0  Padding for 4 byte alignment (could be a value other than zero)
/* 3E 3 */ 0xFF, 0x00, 0x00,        // 255 0 0  Blue, Pixel (x=0, y=0)
/* 41 3 */ 0x00, 0xFF, 0x00,        // 0 255 0  Green, Pixel (x=1, y=0)
/* 44 2 */ 0x00, 0x00               // 0 0  Padding for 4 byte alignment (could be a value other than zero)
};

// Modify to create an 800 x 800 header
static unsigned char bmp_header[] = {
/* 00 2 */ 0x42, 0x4D,              // "BM" ID field (42 4D)
/* 02 4 */ 0x36, 0x4c, 0x1d, 0x00,  // 1,920,054 bytes (54+1920000) Size of the BMP file (54 bytes header + 1,920,000 bytes data)
                                    // 1,920,054 = 0x1d4c36
/* 06 2 */ 0x00, 0x00,              // Unused   Application specific
/* 08 2 */ 0x00, 0x00,              // Unused   Application specific
/* 0A 4 */ 0x36, 0x00, 0x00, 0x00,  // 54 bytes (14+40) Offset where the pixel array (bitmap data) can be found
                                    // DIB Header
/* 0E 4 */ 0x28, 0x00, 0x00, 0x00,  // 40 bytes Number of bytes in the DIB header (from this point)
/* 12 4 */ 0x20, 0x03, 0x00, 0x00,  // 2 pixels (left to right order)   Width of the bitmap in pixels (800=0x320)
/* 16 4 */ 0x20, 0x03, 0x00, 0x00,  // 2 pixels (bottom to top order)   Height of the bitmap in pixels. Positive for bottom to top pixel order.
/* 1A 2 */ 0x01, 0x00,              // 1 plane  Number of color planes being used
/* 1C 2 */ 0x18, 0x00,              // 24 bits  Number of bits per pixel
/* 1E 4 */ 0x00, 0x00, 0x00, 0x00,  // 0 BI_RGB, no pixel array compression used
/* 22 4 */ 0x00, 0x4c, 0x1d, 0x00,  // 1,920,000 = 0x1d4c00 bytes, size of the raw bitmap data (including padding, but 800*3 divisible by 4, so no padding)
/* 26 4 */ 0x13, 0x0B, 0x00, 0x00,  // 2835 pixels/metre horizontal Print resolution of the image,
                                    // 72 DPI × 39.3701 inches per metre yields 2834.6472
/* 2A 4 */ 0x13, 0x0B, 0x00, 0x00,  // 2835 pixels/metre vertical
/* 2E 4 */ 0x00, 0x00, 0x00, 0x00,  // 0 colors Number of colors in the palette
/* 32 4 */ 0x00, 0x00, 0x00, 0x00   // 0 important colors   0 means all colors are important
                                    // Start of pixel array (bitmap data)
};

void make_bmp()
{
    FILE *f = fopen("sofitel.bmp", "wb" );
    if( !f )
        return;
    fwrite( bmp_header, 1, sizeof(bmp_header), f );

    // Test - make nice stripes
    /*
    unsigned char *p = pixels;
    for( int i=0; i<800; i++ )
    {
        for( int j=0; j<100; j++ )
        {
            *p++ = 255; // Blue
            *p++ = 0;   // Green
            *p++ = 0;   // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 0;   // Blue
            *p++ = 255; // Green
            *p++ = 0;   // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 0;   // Blue
            *p++ = 0;   // Green
            *p++ = 255; // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 255; // Blue
            *p++ = 0;   // Green
            *p++ = 0;   // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 0;   // Blue
            *p++ = 255; // Green
            *p++ = 0;   // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 0;   // Blue
            *p++ = 0;   // Green
            *p++ = 255; // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 255; // Blue
            *p++ = 0;   // Green
            *p++ = 0;   // Red
        }
        for( int j=0; j<100; j++ )
        {
            *p++ = 0;   // Blue
            *p++ = 255; // Green
            *p++ = 0;   // Red
        }
    } */
    fwrite( pixels, 1, sizeof(pixels), f );
    fclose(f);
}

// First half:  (Red to White) Blue and Green channels increase from 0 to 255, while Red stays at 255
// Second half: (White to Blue)  Green and Red channels decrease from 255 to 0, while Blue stays at 255 
void make_gradients()
{

    // First half:  (Red to White) Blue and Green channels increase from 0 to 255, while Red stays at 255
    for( int i=0; i<256; i++ )
    {
        red_gradient  [i] = 255;
        blue_gradient [i] = (unsigned char)i;
        green_gradient[i] = (unsigned char)i;
    }

    // Second half: (White to Blue)  Green and Red channels decrease from 255 to 0, while Blue stays at 255 
    for( int i=256; i<512; i++ )
    {
        red_gradient  [i] = (unsigned char)(255 - (i-256));
        blue_gradient [i] = 255;
        green_gradient[i] = (unsigned char)(255 - (i-256));
    }
}


/*

Some notes for Alexandre:

It's easy to build this on Windows, Mac or Linux (especially the latter two, although I used Windows)

For Unix operating systems use the command line like this;

g++ sofitel.cpp
chmod +x ./a.out
./a.out

The first line compiles sofitel.cpp as a C++ program
The second gives the resulting executable file a.out executable privileges (weird I know) 
The third line runs the program

For Windows install Microsoft Visual C++ (community edition, free) and go from there. It's a monster
but easy to use once you spend half an hour installing it!

Here's an experiment to start with;

Temporarily modify main() to just call this function;

void make_blue_square()
{
    FILE *f = fopen("blue-800x800.bmp", "wb" );
    if( !f )
        return;
    fwrite( bmp_header, 1, sizeof(bmp_header), f );
    unsigned char *p = pixels;
    for( int i=0; i<800*800; i++ )
    {
        *p++ = 255; // Blue
        *p++ = 0;   // Green
        *p++ = 0;   // Red
    }
    fwrite( pixels, 1, sizeof(pixels), f );
    fclose(f);
}

It just makes a big 800x800 blue square.

The bmp_header[] array is basically straight from the Examples section of the Wikipedia article
on the .bmp format.

The pixels array is declared as;

static unsigned char pixels[800*3*800];

(Don't worry about the 'static' keyword initially)

It's basically just an array of 800*800*3 bytes (unsigned char is a byte, an integer in the range 0-255)
This is because three bytes (red, green and blue) are needed for each pixel.

This line;

    unsigned char *p = pixels;

Declares variable p, a pointer to a byte, and points it at the pixels array

This line;

        *p++ = 255; // Blue

Sets the contents of the pointer to value 255 and bumps the pointer to the next address in memory.

Understand (or just accept the meaning of) those two lines and you're well on the way to understanding
function make_blue_square(). Understand that function and you're well on the way to understanding the
actual program!

*/




