MIT License

Copyright (c) 2020 Simon Jansen
https://www.asciimation.co.nz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
  
// U8Glib graphics library.
#include "U8glib.h"
// Debouncing library.
#include <Bounce2.h>
// Elapsed millis library.
#include <elapsedMillis.h>
// Wire library.
#include <Wire.h>

// Simon Jansen
// October 2018
// http://www.asciimation.co.nz/bb/2015/03/24/a-three-rotor-enigma-machine-wrist-watch
// http://www.asciimation.co.nz/bb
// www.asciimation.co.nz
// jansens@asciimation.co.nz

// Simulates an M3 Enigma machine as used by the Heer and Lufwaffe in WW2.
// Five rotors of which any three are used in the machine. Single turn over 
// notch per rotor. 10 patch cables.

// Indexing of the machine is done on a 1 to 26 system. Care must be taken 
// to correctly map this into C zero based arrays. The conversion (-1 from 
// the index) is only done at the point of array lookup.

// PROGMEM used to put large string and structures into flash memory.
#include <avr/pgmspace.h>

// Set up push buttons.
const uint8_t key_left = 5;
const uint8_t key_right = 6;
const uint8_t key_sel = 7;

// Debounce objects.
Bounce debouncerLeft = Bounce(); 
Bounce debouncerRight = Bounce(); 
Bounce debouncerSelect = Bounce(); 
  
// Constructor for graphics library.
// Pins are: sck, mosi, cs, a0 (d/c), reset
U8GLIB_SSD1306_128X64 u8g(15, 16, 8, 9, 4);

const u8g_fntpgm_uint8_t frakturcook16[3655] U8G_FONT_SECTION("frakturcook16") = {
  0,25,20,250,252,14,2,52,5,0,32,255,254,15,252,14,
  254,0,0,0,4,0,1,3,12,12,3,0,0,64,192,64,
  64,64,64,64,64,0,0,96,64,4,4,4,5,0,7,80,
  80,0,160,6,9,9,8,1,1,40,40,8,252,0,64,252,
  80,80,6,13,13,7,1,255,32,112,248,200,192,224,120,56,
  12,140,248,120,32,10,12,24,12,1,255,65,0,226,0,50,
  0,52,0,151,128,152,128,232,192,86,64,18,64,35,128,1,
  0,64,0,8,11,11,8,0,0,24,60,44,108,56,56,123,
  94,206,127,59,1,4,4,3,1,9,128,128,128,128,3,14,
  14,4,0,254,32,224,192,192,192,192,192,192,192,192,64,64,
  64,32,4,15,15,4,0,254,64,224,112,48,48,48,16,16,
  16,16,16,32,32,0,64,8,9,9,8,0,1,8,24,60,
  90,103,194,24,28,8,9,9,18,10,1,0,24,0,24,0,
  24,0,255,128,255,0,24,0,24,0,24,0,16,0,3,6,
  6,3,0,253,96,96,96,64,128,128,5,2,2,6,1,4,
  120,240,3,3,3,3,0,0,96,224,64,9,9,18,10,0,
  1,0,128,1,0,2,0,2,0,12,0,24,0,112,0,96,
  0,192,0,6,12,12,8,1,0,16,56,92,140,132,132,132,
  196,196,104,112,32,3,12,12,6,1,0,32,224,32,32,32,
  32,32,32,32,32,96,128,8,12,12,9,0,0,28,62,102,
  99,114,34,4,8,16,49,126,198,7,12,12,8,0,0,96,
  126,4,8,8,28,6,6,66,194,116,48,7,12,12,8,1,
  0,24,16,52,40,72,72,136,254,8,8,24,16,7,12,12,
  8,0,0,62,60,32,32,60,30,6,2,0,196,248,48,7,
  12,12,8,1,0,28,48,96,64,220,204,134,198,196,68,120,
  16,7,12,12,7,1,0,254,252,8,8,16,16,16,32,32,
  96,96,96,7,12,12,8,1,0,56,76,68,196,104,56,76,
  134,196,228,120,48,7,12,12,8,1,0,16,60,76,134,198,
  198,236,116,4,8,16,96,3,9,9,3,0,0,64,224,64,
  0,0,0,96,224,64,3,9,9,3,0,255,96,224,64,0,
  0,96,96,96,64,4,7,7,6,1,1,16,32,64,128,64,
  32,16,5,4,4,6,1,3,120,240,0,224,5,7,7,6,
  0,1,192,32,16,8,16,32,64,3,13,13,3,0,0,64,
  224,96,32,32,0,64,64,192,0,64,224,64,9,9,18,10,
  1,0,28,0,35,0,93,0,164,128,164,128,173,0,183,0,
  128,0,100,0,9,14,28,10,0,254,60,0,71,128,129,128,
  49,128,49,128,63,128,49,128,49,128,33,128,33,128,33,128,
  64,128,64,0,128,0,9,11,22,10,1,1,12,0,63,0,
  98,0,100,0,126,0,99,0,97,128,97,0,65,0,242,0,
  60,0,8,12,12,9,1,0,6,27,200,200,200,200,200,208,
  96,120,30,4,8,11,11,10,1,1,112,252,30,102,99,99,
  99,99,66,228,120,8,12,12,9,1,0,12,31,120,216,159,
  152,216,240,96,120,30,4,7,14,14,8,0,254,12,30,34,
  96,96,62,48,24,12,12,68,232,120,48,10,12,24,11,1,
  0,12,0,55,128,81,0,82,0,151,0,145,128,216,192,208,
  128,97,0,113,0,62,0,8,0,8,15,15,10,1,253,12,
  62,192,64,76,94,99,67,65,99,114,2,4,24,16,7,11,
  11,8,0,1,126,124,4,4,4,6,6,6,196,232,112,8,
  15,15,8,255,253,63,63,67,3,3,31,3,2,6,4,4,
  8,16,224,64,10,12,24,10,0,0,113,0,243,0,51,128,
  53,128,56,192,51,0,62,0,51,0,35,0,33,128,121,192,
  16,0,7,12,12,8,1,0,6,60,204,204,76,108,104,80,
  96,224,254,12,11,12,24,13,1,0,32,192,79,224,214,96,
  102,32,102,96,102,96,38,96,38,96,70,96,70,96,230,96,
  32,32,8,12,12,10,1,0,36,79,211,98,98,98,34,34,
  66,66,227,32,11,12,24,12,1,0,13,0,63,0,77,128,
  76,192,204,224,204,96,204,64,204,64,104,64,112,128,59,0,
  12,0,8,14,14,10,1,254,196,238,118,99,99,97,97,97,
  98,250,124,100,96,64,18,15,45,12,1,253,13,0,0,63,
  0,0,79,128,0,76,192,0,204,224,0,204,96,0,204,64,
  0,204,64,0,104,64,0,112,128,0,59,128,0,13,192,0,
  0,224,0,0,56,0,0,15,192,8,12,12,10,1,0,68,
  238,115,97,102,108,108,70,70,195,243,32,10,12,24,11,1,
  0,30,64,63,128,64,0,192,0,199,0,255,128,152,192,192,
  64,224,0,112,128,63,0,4,0,9,12,24,10,1,0,127,
  128,127,0,28,0,44,0,76,0,204,0,204,0,204,0,104,
  0,112,0,63,128,12,0,8,12,12,10,1,1,64,130,135,
  194,226,98,34,34,66,71,251,247,8,11,11,9,1,1,132,
  204,246,102,99,99,99,98,96,116,120,11,12,24,13,1,0,
  72,0,204,192,215,192,230,96,102,96,102,32,38,32,38,32,
  70,64,70,64,251,128,17,0,9,12,24,9,0,0,35,0,
  99,128,52,0,56,0,24,0,28,0,44,0,38,0,70,0,
  67,128,195,0,128,0,8,16,16,10,1,253,64,130,135,194,
  226,98,34,34,66,71,251,115,1,66,252,56,8,14,14,8,
  0,254,127,126,4,8,24,12,14,27,99,67,131,130,196,248,
  2,13,13,3,1,255,128,128,128,128,128,128,128,128,128,128,
  128,128,192,5,12,12,6,0,0,128,0,64,64,32,32,0,
  16,16,0,8,8,3,13,13,3,0,255,96,96,96,96,96,
  96,96,96,96,96,96,96,224,4,3,3,4,0,9,96,96,
  144,9,1,2,9,0,253,255,128,1,1,1,4,2,12,128,
  5,8,8,7,1,0,112,144,144,144,144,144,248,64,4,12,
  12,6,1,0,64,128,128,128,224,176,144,144,144,144,224,32,
  5,8,8,5,0,0,48,80,0,192,192,72,112,32,4,10,
  10,6,1,1,64,96,96,112,176,144,144,144,144,224,4,8,
  8,5,1,0,224,176,160,128,192,192,224,32,5,14,14,4,
  0,253,56,64,64,112,64,64,64,64,64,64,64,0,128,128,
  5,10,10,7,1,254,112,144,144,144,144,144,216,88,144,224,
  5,14,14,6,1,254,64,128,128,128,160,176,144,152,152,144,
  208,144,16,32,3,11,11,3,0,0,96,0,0,224,64,64,
  64,64,64,96,64,3,14,14,3,0,253,96,64,0,96,64,
  64,64,64,64,64,64,0,128,128,4,11,11,5,1,0,160,
  240,160,128,224,128,128,128,128,192,128,3,12,12,3,0,0,
  64,192,64,64,64,64,64,64,64,64,224,64,9,8,16,9,
  0,0,254,0,91,0,73,0,73,0,73,0,73,0,109,128,
  73,0,6,8,8,6,0,0,248,88,72,72,72,72,108,72,
  4,8,8,6,1,0,96,176,144,144,144,144,224,64,6,11,
  11,6,0,253,240,88,72,76,72,72,112,64,64,64,128,5,
  11,11,6,0,253,48,216,88,88,88,88,248,24,24,8,8,
  4,8,8,5,1,0,224,176,128,128,128,128,192,128,5,8,
  8,6,1,0,112,144,176,240,152,208,96,32,4,9,9,4,
  0,1,32,64,240,64,64,64,64,64,96,5,8,8,7,1,
  0,216,152,144,144,144,144,248,64,5,7,7,6,0,1,216,
  88,72,72,72,72,240,7,7,7,9,1,1,180,246,146,146,
  146,146,252,6,7,7,6,0,1,204,112,32,48,112,24,152,
  6,11,11,6,0,253,108,72,72,72,72,64,240,16,32,64,
  192,4,11,11,5,0,254,128,240,32,64,96,48,80,144,16,
  144,224,4,13,13,3,0,255,32,64,64,64,64,64,192,64,
  64,64,64,64,48,1,12,12,1,0,0,128,128,128,128,128,
  128,128,128,128,128,128,128,4,13,13,4,0,255,64,32,32,
  32,32,32,48,32,32,32,32,32,192,4,2,2,4,0,6,
  64,176,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,0,0,0,3,0,1,2,11,11,3,0,253,192,
  64,0,64,64,64,64,64,64,64,64,5,8,8,5,0,0,
  48,80,0,192,192,72,112,32,7,12,12,9,1,0,2,24,
  224,192,192,64,96,240,64,192,254,12,8,7,7,8,0,2,
  195,126,98,66,66,126,195,7,11,11,8,1,0,230,68,100,
  96,56,48,16,254,16,16,56,1,14,14,7,3,253,128,128,
  128,128,128,128,0,0,0,128,128,128,128,128,6,13,13,8,
  1,254,48,200,204,224,112,156,140,196,224,56,28,204,72,4,
  2,2,7,2,9,208,128,8,8,8,10,1,1,32,12,161,
  160,160,53,66,24,4,5,5,5,0,7,32,144,16,16,240,
  9,7,14,9,0,1,25,128,51,0,102,0,204,0,102,0,
  51,0,25,128,1,1,1,5,4,4,128,0,0,0,5,4,
  6,8,7,7,10,1,2,64,164,36,36,40,175,66,4,1,
  1,5,1,10,240,5,7,7,5,0,5,32,112,16,152,200,
  64,112,9,9,18,10,1,0,8,0,24,0,24,0,255,128,
  255,0,24,0,16,0,0,0,255,0,4,7,7,5,1,5,
  96,32,176,160,0,16,240,5,7,7,5,0,5,112,16,48,
  48,8,192,96,1,2,2,11,9,11,128,128,6,11,11,7,
  0,253,108,76,72,72,72,72,124,96,64,192,128,11,12,24,
  11,0,0,2,128,15,224,50,128,98,128,226,128,226,128,226,
  128,226,128,98,128,114,128,31,224,0,128,3,3,3,3,0,
  6,64,224,64,3,3,3,8,3,253,64,224,32,1,7,7,
  3,2,5,128,128,128,128,128,128,128,3,5,5,4,1,7,
  64,192,160,160,192,9,7,14,9,0,1,204,0,102,0,51,
  0,25,128,51,0,102,0,204,0,8,12,12,10,2,0,192,
  196,192,200,194,212,194,42,2,95,6,134,8,12,12,11,2,
  0,192,196,192,200,192,214,203,43,10,64,7,143,9,13,26,
  11,1,255,240,0,1,0,34,0,48,0,21,0,145,0,234,
  0,1,0,21,0,7,128,33,0,1,0,64,0,3,13,13,
  3,0,252,64,224,64,0,96,96,64,64,128,128,128,224,64,
  9,15,30,11,0,254,32,0,2,0,7,0,17,128,49,128,
  49,128,63,128,49,128,49,128,33,128,33,128,33,128,64,128,
  64,0,128,0,10,16,32,11,0,254,0,192,0,128,60,0,
  71,128,129,128,49,128,49,128,63,128,49,128,49,128,33,128,
  33,128,33,128,64,128,64,0,128,0,9,16,32,11,0,254,
  12,0,16,0,60,0,71,128,129,128,49,128,49,128,63,128,
  49,128,49,128,33,128,33,128,33,128,64,128,64,0,128,0,
  10,17,34,11,0,253,30,64,17,192,2,0,7,128,9,192,
  24,128,24,128,31,128,16,128,16,128,16,128,16,192,48,192,
  32,0,0,0,64,0,128,0,10,15,30,11,0,253,10,192,
  7,128,9,192,24,128,48,128,31,128,16,128,16,128,16,128,
  48,192,32,192,32,0,64,0,64,0,128,0,9,16,32,10,
  1,254,12,0,18,0,18,0,10,0,62,0,139,128,17,128,
  49,128,63,128,49,128,49,128,33,128,33,128,64,128,64,0,
  128,0,15,14,28,15,0,254,28,12,127,62,139,240,17,176,
  49,190,63,176,63,144,48,176,48,192,32,112,32,60,64,8,
  64,0,128,0,7,14,14,9,1,254,6,62,216,152,152,152,
  152,240,192,112,60,20,24,8,8,14,14,10,1,0,64,64,
  12,31,120,216,159,152,216,240,96,120,30,4,8,15,15,10,
  1,0,1,1,0,12,31,120,216,159,152,216,240,96,120,30,
  4,8,14,14,10,1,0,12,18,12,31,120,216,159,152,216,
  240,96,120,30,4,8,13,13,9,1,0,64,109,31,120,216,
  159,152,216,240,96,120,30,4,7,14,14,8,0,1,8,8,
  0,126,124,4,4,4,6,6,6,196,232,112,7,14,14,8,
  0,1,4,0,0,126,124,4,4,4,6,6,6,196,232,112,
  7,14,14,8,0,1,8,20,0,126,124,4,4,4,6,6,
  6,196,232,112,7,13,13,8,0,1,20,32,126,124,4,4,
  4,6,6,6,196,232,112,9,11,22,10,0,1,56,0,126,
  0,15,0,51,0,49,128,51,128,253,128,49,128,33,0,114,
  0,60,0,10,14,28,11,1,0,30,64,24,192,36,0,79,
  0,211,0,98,0,98,0,98,0,34,0,34,0,66,0,66,
  0,227,0,32,0,11,15,30,13,1,0,2,0,2,0,0,
  0,13,0,63,0,77,128,76,192,204,224,204,96,204,64,204,
  64,104,64,112,128,59,0,12,0,11,15,30,13,1,0,1,
  0,1,0,0,0,13,0,63,0,77,128,76,192,204,224,204,
  96,204,64,204,64,104,64,112,128,59,0,12,0,11,14,28,
  13,1,0,6,0,0,0,13,0,63,0,77,128,76,192,204,
  224,204,96,204,64,204,64,104,64,112,128,59,0,12,0,11,
  15,30,13,1,0,56,0,119,128,33,128,13,0,63,0,77,
  128,76,192,204,224,204,96,204,64,204,64,104,64,112,128,59,
  0,12,0,11,12,24,13,1,0,237,32,255,96,79,160,76,
  192,204,224,204,96,204,64,204,64,104,64,112,128,59,0,12,
  0,6,7,7,10,2,1,132,204,120,48,120,204,132,11,12,
  24,12,1,0,13,64,63,192,79,128,77,192,207,224,206,96,
  204,64,204,64,120,64,112,128,59,0,108,0,8,13,13,11,
  1,1,24,72,130,135,194,226,98,34,34,66,71,251,247,8,
  13,13,11,1,1,4,64,130,135,194,226,98,34,34,66,71,
  251,247,8,14,14,11,1,1,8,20,64,130,135,194,226,98,
  34,34,66,71,251,247,8,11,11,11,1,1,158,167,195,99,
  115,35,35,35,71,123,115,8,16,16,11,1,253,72,138,135,
  194,226,98,34,34,66,71,251,115,1,66,252,56,8,14,14,
  10,1,254,192,224,100,110,119,99,97,97,97,250,124,100,96,
  64,6,14,14,7,0,253,48,92,64,124,72,72,92,76,68,
  68,68,0,136,176,5,11,11,7,1,0,64,32,0,112,144,
  144,144,144,144,248,64,5,11,11,7,1,0,32,64,0,112,
  144,144,144,144,144,248,64,5,11,11,7,1,0,96,16,0,
  112,144,144,144,144,144,248,64,5,10,10,7,1,0,176,0,
  112,144,144,144,144,144,248,64,5,11,11,7,1,0,88,0,
  0,112,144,144,144,144,144,248,64,5,13,13,7,1,0,32,
  64,64,32,0,112,144,144,144,144,144,248,64,7,8,8,8,
  1,0,252,54,52,24,16,144,252,68,5,11,11,6,0,253,
  48,80,0,192,192,72,112,32,48,16,96,4,11,11,5,1,
  0,64,0,0,224,176,160,128,192,192,224,32,4,11,11,5,
  1,0,96,64,0,224,176,160,128,192,192,224,32,4,11,11,
  5,1,0,96,144,0,224,176,160,128,192,192,224,32,4,11,
  11,5,1,0,208,160,0,224,176,160,128,192,192,224,32,3,
  11,11,3,0,0,64,64,0,224,64,64,64,64,64,96,64,
  3,11,11,3,0,0,64,64,0,224,64,64,64,64,64,96,
  64,3,11,11,3,0,0,64,32,0,224,64,64,64,64,64,
  96,64,4,11,11,3,0,0,176,32,0,224,64,64,64,64,
  64,96,64,4,10,10,7,1,1,80,96,224,112,176,144,144,
  144,144,224,6,10,10,6,0,0,88,0,248,88,72,72,72,
  72,108,72,4,11,11,6,1,0,64,0,0,96,176,144,144,
  144,144,224,64,4,11,11,6,1,0,64,64,0,96,176,144,
  144,144,144,224,64,4,11,11,6,1,0,96,144,0,96,176,
  144,144,144,144,224,64,4,10,10,6,1,0,176,128,96,176,
  144,144,144,144,224,64,4,11,11,6,1,0,80,144,0,96,
  176,144,144,144,144,224,64,255,4,8,8,6,1,0,112,176,
  176,144,208,144,224,64,5,11,11,7,1,0,64,32,0,216,
  152,144,144,144,144,248,64,5,11,11,7,1,0,32,0,0,
  216,152,144,144,144,144,248,64,5,11,11,7,1,0,96,16,
  0,216,152,144,144,144,144,248,64,5,11,11,7,1,0,88,
  80,0,216,152,144,144,144,144,248,64,6,14,14,6,0,253,
  16,32,0,108,72,72,72,72,64,240,16,32,64,192,5,15,
  15,6,0,253,32,64,64,64,112,88,72,72,72,72,112,80,
  64,64,128,6,14,14,6,0,253,52,40,0,108,72,72,72,
  72,64,240,16,32,64,192};
  
// ENIGMA logo.
const uint8_t logo[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xC0, 0x00, 0x00, 0x01, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x0C, 0x07, 0xF8, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x3C, 0x1F, 0xFF, 0x0F, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x7C, 0x3F, 0xFF, 0x8F, 0xE0, 0x0F, 0xFE, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0xFF, 0xC0, 0x00, 0x7C, 0x3F, 0xFF, 0x8F, 0xFE, 0x01, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x07, 0xFE, 0x01, 0xE0, 0x7C, 0x7F, 0x1F, 0xCF, 0xFF, 0xC0, 0x7F, 0xF0, 0x00, 0x00,
0x00, 0x00, 0x1F, 0xF8, 0x0F, 0xF8, 0x7C, 0x7E, 0x0F, 0xCF, 0xFF, 0xF8, 0x0F, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x7F, 0xC0, 0x3F, 0xFC, 0x7C, 0x7E, 0x0F, 0xCF, 0xFF, 0xFC, 0x03, 0xFF, 0x00, 0x00,
0x00, 0x01, 0xFF, 0x80, 0xFF, 0xFC, 0x7C, 0x7E, 0x0F, 0xCF, 0xBF, 0xFC, 0x01, 0xFF, 0x80, 0x00,
0x00, 0x07, 0xFE, 0x01, 0xFF, 0xFC, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0xFE, 0x00, 0x7F, 0xE0, 0x00,
0x00, 0x1F, 0xF8, 0x19, 0xF0, 0xFC, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x7E, 0x03, 0x1F, 0xF8, 0x00,
0x00, 0x3F, 0xF0, 0x79, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x07, 0x87, 0xFC, 0x00,
0x00, 0xFF, 0xC0, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x0F, 0xC3, 0xFF, 0x00,
0x01, 0xFF, 0x83, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x1F, 0xF1, 0xFF, 0x80,
0x03, 0xFE, 0x07, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x1F, 0xF8, 0x7F, 0xC0,
0x07, 0xFC, 0x1F, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x3F, 0xFC, 0x1F, 0xE0,
0x0F, 0xF8, 0x3F, 0x81, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x3E, 0xFE, 0x0F, 0xF0,
0x1F, 0xE0, 0x7E, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x7C, 0x3E, 0x03, 0xF8,
0x3F, 0xE0, 0xFC, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x7C, 0x1E, 0x07, 0xF8,
0x3F, 0xF0, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x78, 0x1E, 0x0F, 0xFC,
0x7F, 0xF8, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x78, 0x1E, 0x1F, 0xFE,
0x78, 0xF8, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x00, 0x0F, 0x8F, 0x1E, 0x78, 0x1E, 0x1F, 0x1E,
0xF0, 0x7C, 0xFF, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x7F, 0xCF, 0x8F, 0x1E, 0x78, 0x1E, 0x3E, 0x0F,
0xF0, 0x7C, 0xFF, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x7F, 0xCF, 0x8F, 0x1E, 0x78, 0x1E, 0x3E, 0x0F,
0xF0, 0x7C, 0xFF, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x7F, 0xCF, 0x8F, 0x1E, 0x78, 0x1E, 0x3E, 0x0F,
0xF8, 0xF8, 0xFF, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x7F, 0xCF, 0x8F, 0x1E, 0x78, 0x3E, 0x1F, 0x1F,
0x7F, 0xF8, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x0F, 0xCF, 0x8F, 0x1E, 0x78, 0xFE, 0x1F, 0xFE,
0x3F, 0xF0, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x7B, 0xFE, 0x0F, 0xFE,
0x3F, 0xF0, 0xF8, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x7F, 0xFE, 0x0F, 0xFC,
0x1F, 0xC0, 0xFE, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x7F, 0xDE, 0x07, 0xF8,
0x0F, 0xE0, 0x7F, 0x81, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x7F, 0x1E, 0x0F, 0xF0,
0x07, 0xF0, 0x3F, 0xE1, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x7C, 0x1C, 0x3F, 0xE0,
0x03, 0xFC, 0x0F, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x78, 0x18, 0x7F, 0xC0,
0x01, 0xFE, 0x07, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x78, 0x11, 0xFF, 0x80,
0x00, 0xFF, 0x81, 0xF9, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x78, 0x07, 0xFF, 0x00,
0x00, 0x3F, 0xE0, 0x39, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x78, 0x0F, 0xFC, 0x00,
0x00, 0x1F, 0xF0, 0x09, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x70, 0x3F, 0xF0, 0x00,
0x00, 0x07, 0xFE, 0x01, 0xF0, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x60, 0xFF, 0xE0, 0x00,
0x00, 0x03, 0xFF, 0x00, 0x70, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x1E, 0x03, 0xFF, 0xC0, 0x00,
0x00, 0x00, 0x7F, 0xC0, 0x00, 0x7C, 0x7C, 0x7E, 0x07, 0xCF, 0x8F, 0x18, 0x0F, 0xFE, 0x00, 0x00,
0x00, 0x00, 0x1F, 0xF8, 0x00, 0x7C, 0x7C, 0x7F, 0x0F, 0xCF, 0x8F, 0x00, 0x7F, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x07, 0xFF, 0x00, 0x1C, 0x7C, 0x3F, 0xFF, 0x8F, 0x8E, 0x01, 0xFF, 0xE0, 0x00, 0x00,
0x00, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x7C, 0x3F, 0xFF, 0x8F, 0x80, 0x0F, 0xFF, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7F, 0xF8, 0x00, 0x3C, 0x1F, 0xFF, 0x0E, 0x00, 0x3F, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0F, 0xFF, 0x80, 0x0C, 0x07, 0xFE, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xC0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xE0, 0x00, 0x00, 0x07, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// Look up tables for entry point, rotor wirings and reflectors.
// Entry point.
//                                   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z
const int ent[26] PROGMEM =       {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 ,22, 23, 24, 25, 26 };

// Rotors. Second to last value is the notch position. Last value is the rotor number.
// Letters.                          E   K   M   F   L   G   D   Q   V   Z   N   T   O   W   Y   H   X   U   S   P   A   I   B   R   C   J
const int rotor_I[28] PROGMEM =   {  5, 11, 13,  6, 12,  7,  4, 17, 22, 26, 14, 20, 15, 23, 25,  8, 24, 21, 19, 16,  1,  9,  2, 18,  3, 10, 17, 1 };
// Letters.                          A   J   D   K   S   I   R   U   X   B   L   H   W   T   M   C   Q   G   Z   N   P   Y   F   V   O   E
const int rotor_II[28] PROGMEM =  {  1, 10,  4, 11, 19,  9, 18, 21, 24,  2, 12,  8, 23, 20, 13,  3, 17,  7, 26, 14, 16, 25,  6, 22, 15,  5,  5, 2 };
// Letters.                          B   D   F   H   J   L   C   P   R   T   X   V   Z   N   Y   E   I   W   G   A   K   M   U   S   Q   O
const int rotor_III[28] PROGMEM = {  2,  4,  6,  8, 10, 12,  3, 16, 18, 20, 24, 22, 26, 14, 25,  5,  9, 23,  7,  1, 11, 13, 21, 19, 17, 15, 22, 3 };
// Letters.                          E   S   O   V   P   Z   J   A   Y   Q   U   I   R   H   X   L   N   F   T   G   K   D   C   M   W   B
const int rotor_IV[28] PROGMEM =  {  5, 19, 15, 22, 16, 26, 10,  1, 25, 17, 21,  9, 18,  8, 24, 12, 14,  6, 20,  7, 11,  4,  3, 13, 23,  2, 10, 4 };
// Letters.                          V   Z   B   R   G   I   T   Y   U   P   S   D   N   H   L   X   A  W   M   J   Q    O   F   E   C   K
const int rotor_V[28] PROGMEM =   { 22, 26,  2, 18,  7,  9, 20, 25, 21, 16, 19,  4, 14,  8, 12, 24,  1, 23, 13, 10, 17, 15,  6,  5,  3, 11, 26, 5};
// Reflectors.
// Letters.                          Y   R   U   H   Q   S   L   D   P   X   N   G   O   K   M   I   E   B   F   Z   C   W   V   J   A   T
const int ref_B[26] PROGMEM =     { 25, 18, 21,  8, 17, 19, 12,  4, 16, 24, 14,  7, 15, 11, 13,  9,  5,  2,  6, 26,  3, 23, 22, 10,  1, 20 }; 
// Letters                           F   V   P   J   I   A   O   Y   E   D   R   Z   X   W   G   C   T   K   U   Q   S   B   N   M   H   L
const int ref_C[26] PROGMEM =     {  6, 22, 16, 10,  9,  1, 15, 25,  5,  4, 18, 26, 24, 23,  7,  3, 20, 11, 21, 17, 19,  2, 14, 13,  8, 12 };

// Plugboard.
// Letters.            A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z
int plugboard[26] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 ,22, 23, 24, 25, 26 }; 
// Plug board steckered character display lines.
char pb_line_1[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char pb_line_2[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// Which plugboard pair we are entering.
int pairs = 0;
// What are the letters in the pair.
int first_of_pair = 0;
int second_of_pair = 0;

// Rotors.
const int *left_rotor = &rotor_I[0];  
const int *mid_rotor = &rotor_II[0]; 
const int *right_rotor = &rotor_III[0];

// Reflector.
const int *reflector;
bool reflector_B = true;

// Rotor positions.
int left_rotor_position = 26;
int mid_rotor_position = 26;
int right_rotor_position = 26;

// Rotor notch positions.
int left_rotor_notch = 0;
int mid_rotor_notch = 0;
int right_rotor_notch = 0;

// Rotor ring positions.
int left_rotor_ring = 1;
int mid_rotor_ring = 1;
int right_rotor_ring = 1;

// Rotor selection.
char* rotor_names[] = { "I", "II", "III", "IV", "V" };
int rotor_selection[5] = { 1, 2, 3, 4, 5 };
int rotor_selection_index = 0;

// Screens. This is used to keep track of the screen we are currently
// viewing/editing.
int screen = 0;

// Toggles for the UI.
bool edit_steckerverbindungen = false;
bool edit_Walzenlage = false;
bool edit_Ringstellung = false;
bool edit_Grundstellung = false;

// Input/output letters and ticker output array.
int input_letter = 1;
int output_letter = 1;
char output_ticker[16];
int output_ticker_counter = 0;

// UI setting constants.
const int line_1_y = 15;
const int line_2_y = 33;
const int time_y = 51;
  
// Current time settings.
int seconds = 0;
int minutes = 0;
char minutesString[3] = { 0, 0, 0 };
int hours = 0;
char hoursString[3] = { 0, 0, 0 };
elapsedMillis secondCounter = 0;
const int onesecond = 1000;
boolean RTC_read = false;
boolean timeUpdated = false;

//static const uint8_t M41T62_100THS_SEC = 0;
static const uint8_t M41T62_SEC = 1;
static const uint8_t M41T62_MIN = 2;
static const uint8_t M41T62_HOUR = 3;
//static const uint8_t M41T62_DOW = 4;
//static const uint8_t M41T62_DATE = 5;
//static const uint8_t M41T62_CENT_MONTH = 6;
//static const uint8_t M41T62_YEAR = 7;
//static const uint8_t M41T62_CALIB = 8;
//static const uint8_t M41T62_WDMB = 9;
//static const uint8_t M41T62_ALARM_MONTH = 10;
//static const uint8_t M41T62_ALARM_DATE = 11;
//static const uint8_t M41T62_ALARM_HOUR = 12;
//static const uint8_t M41T62_ALARM_NIM = 13;
//static const uint8_t M41T62_ALARM_SEC = 14;
//static const uint8_t M41T62_WDFLAG = 15;

static const uint8_t MT41T62_SLAVEADDR = 0b1101000;
static const uint8_t BITS_SEC = 0b01111111;
static const uint8_t BITS_STOPBIT = 1<<7;
static const uint8_t BITS_MIN = 0b01111111;
static const uint8_t BITS_OFIE = 1<<7;
static const uint8_t BITS_HR = 0b00111111;
//static const uint8_t BITS_DOW = 0b00000111;
//static const uint8_t BITS_RS = 0b11110000;
//static const uint8_t BITS_DATE = 0b00111111;
//static const uint8_t BITS_MTH = 0b00011111;
//static const uint8_t BITS_CENT = 0b11000000;
//static const uint8_t BITS_YR = 0b11111111;


//long time;

////////////////////////////////////////////////////////////////////////////////
// Read from the given register.
////////////////////////////////////////////////////////////////////////////////
void RTC_readRegisters(uint8_t addr, uint8_t *regvals, uint8_t num) 
{
  Wire.beginTransmission(MT41T62_SLAVEADDR);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)MT41T62_SLAVEADDR, num);
  for(int i = 0; i < num; i++) {
    *regvals++ = Wire.read(); 
  }
}

////////////////////////////////////////////////////////////////////////////////
// Write to the given register.
////////////////////////////////////////////////////////////////////////////////
void RTC_writeRegisters(uint8_t addr, uint8_t *regvals, uint8_t num)
{
  Wire.beginTransmission(MT41T62_SLAVEADDR);
  Wire.write(addr); 
  for(int i = 0; i < num; i++) {
    Wire.write(*regvals++);
  }
  Wire.endTransmission();
}

////////////////////////////////////////////////////////////////////////////////
// RTC begin.
////////////////////////////////////////////////////////////////////////////////
void RTC_begin(void) 
{
  // As per the datasheet set stop bit to 1 then immediately clear.
  RTC_stop();
  RTC_start();
}

////////////////////////////////////////////////////////////////////////////////
// RTC start.
////////////////////////////////////////////////////////////////////////////////
void RTC_start(void) 
{
  // Clear the stop bit high to start the RTC.
  uint8_t r;
  RTC_readRegisters((uint8_t) M41T62_SEC, &r, 1);
  r &= 0b01111111;
  RTC_writeRegisters(M41T62_SEC, &r, 1);
}

////////////////////////////////////////////////////////////////////////////////
// RTC stop.
////////////////////////////////////////////////////////////////////////////////
void RTC_stop(void)
{
  // Set the stop bit high to stop the RTC.
  uint8_t r;
  RTC_readRegisters((uint8_t) M41T62_SEC, &r, 1);
  r |= 0b10000000; 
  RTC_writeRegisters(M41T62_SEC, &r, 1);
}

////////////////////////////////////////////////////////////////////////////////
// Set time.
////////////////////////////////////////////////////////////////////////////////
//void RTC_setTime(const long & p) 
//{
//  RTC_writeRegisters((byte) M41T62_SEC, (byte *) &p, 3);
//}

////////////////////////////////////////////////////////////////////////////////
// Get time.
////////////////////////////////////////////////////////////////////////////////
//long RTC_getTime() 
//{
//  long timeGot = 0;
//  RTC_readRegisters((byte) M41T62_SEC, (byte *) &timeGot, 3);
//  timeGot &= ((unsigned long)BITS_HR<<16 | BITS_MIN<<8 | BITS_SEC);
//  return timeGot;
//}

////////////////////////////////////////////////////////////////////////////////
// Get hours.
////////////////////////////////////////////////////////////////////////////////
byte RTC_getHours() 
{
  byte data;
  RTC_readRegisters((byte) M41T62_HOUR, &data, 1);
  return (convertFromBcd(data));
}

////////////////////////////////////////////////////////////////////////////////
// Get minutes.
////////////////////////////////////////////////////////////////////////////////
byte RTC_getMinutes() 
{
  byte data;
  RTC_readRegisters((byte) M41T62_MIN, &data, 1);
  return (convertFromBcd(data));
}

////////////////////////////////////////////////////////////////////////////////
// Get seconds.
////////////////////////////////////////////////////////////////////////////////
byte RTC_getSeconds() 
{
  byte data;
  RTC_readRegisters((byte) M41T62_SEC, &data, 1);
  return (convertFromBcd(data));
}

////////////////////////////////////////////////////////////////////////////////
// Set hours minutes and seconds.
////////////////////////////////////////////////////////////////////////////////
void RTC_setHoursMinutesAndSeconds( byte hours, byte minutes, byte seconds) 
{
  byte hoursBCD = convertToBcd(hours);
  byte minutesBCD = convertToBcd(minutes);
  byte secondsBCD = convertToBcd(seconds);
  RTC_writeRegisters((byte) M41T62_HOUR, &hoursBCD, 1);  
  RTC_writeRegisters((byte) M41T62_MIN, &minutesBCD, 1);
  RTC_writeRegisters((byte) M41T62_SEC, &secondsBCD, 1);
}

////////////////////////////////////////////////////////////////////////////////
// Convert decimal to BCD.
////////////////////////////////////////////////////////////////////////////////
byte convertToBcd(byte byteDecimal) 
{
  return ( (byteDecimal/10*16) + (byteDecimal%10) );
}

////////////////////////////////////////////////////////////////////////////////
// Convert BCD to decimal.
////////////////////////////////////////////////////////////////////////////////
byte convertFromBcd(byte byteBCD) 
{ 
  return ( (byteBCD/16*10) + (byteBCD%16) );
}

////////////////////////////////////////////////////////////////////////////////
// Get time.
////////////////////////////////////////////////////////////////////////////////
void GetTime() 
{
  //Serial.println("Getting time....");
  //Serial.println(RTC_getTime(), HEX);
  
  hours = RTC_getHours();
  minutes = RTC_getMinutes();
  seconds = RTC_getSeconds();
  
//  Serial.print("Hours: ");
//  Serial.println( hours );
//  Serial.print("Minutes: ");
//  Serial.println( minutes );
//  Serial.print("Seconds: ");
//  Serial.println( seconds ); 
}

////////////////////////////////////////////////////////////////////////////////
// Store time.
////////////////////////////////////////////////////////////////////////////////
void StoreTime() 
{
//  Serial.println("Storing time...");
//  Serial.print("Hours: ");
//  Serial.println( hours );
//  Serial.print("Minutes: ");
//  Serial.println( minutes );
//  Serial.print("Seconds: ");
//  Serial.println( seconds ); 
  RTC_setHoursMinutesAndSeconds(hours, minutes, seconds);
}

////////////////////////////////////////////////////////////////////////////////
// Increment seconds.
////////////////////////////////////////////////////////////////////////////////
void IncrementSeconds() 
{
  seconds = seconds + 1;
  if ( seconds > 59 ){
    seconds = 0; 
    IncrementMinutes(true);   
  }
}

////////////////////////////////////////////////////////////////////////////////
// Increment minutes.
////////////////////////////////////////////////////////////////////////////////
void IncrementMinutes(bool rollover) 
{
  minutes = minutes + 1;
  if ( minutes > 59 ){
    minutes = 0;
    if ( rollover == true )
    {
      IncrementHours();
    }
  }
  BuildMinutesString();
  //Serial.print("Minutes: ");
  //Serial.println(minutes);
}

////////////////////////////////////////////////////////////////////////////////
// Build minutes string.
////////////////////////////////////////////////////////////////////////////////
void BuildMinutesString()
{
  // minutesString[2] = 0;
  minutesString[1] = 48 + (minutes % 10);
  minutesString[0] = 48 + ((minutes/10) % 10); 
}

////////////////////////////////////////////////////////////////////////////////
// Increment hours.
////////////////////////////////////////////////////////////////////////////////
void IncrementHours() 
{
  hours = hours + 1;
  if ( hours > 23 ){
    hours = 0;
  }
  BuildHoursString();
  //Serial.print("Hours: ");
  //Serial.println(hours);
}

////////////////////////////////////////////////////////////////////////////////
// Build hours string.
////////////////////////////////////////////////////////////////////////////////
void BuildHoursString()
{
  // hoursString[2] = 0;
  hoursString[1] = 48 + (hours % 10);
  hoursString[0] = 48 + ((hours/10) % 10); 
}

////////////////////////////////////////////////////////////////////////////////
// Setup routine.
////////////////////////////////////////////////////////////////////////////////
void setup() 
{  
  // Start the RTC.
  Wire.begin();
  RTC_start;
    
  // Set up push buttons.
  pinMode( key_left, INPUT_PULLUP );  // Left.
  pinMode( key_right, INPUT_PULLUP ); // Right.
  pinMode( key_sel, INPUT_PULLUP );   // Select.
  
  // Debouncers.
  debouncerLeft.attach(key_left);
  debouncerLeft.interval(5); 
  debouncerRight.attach(key_right);
  debouncerRight.interval(5); 
  debouncerSelect.attach(key_sel);
  debouncerSelect.interval(5); 

  // Start the serial port.
  Serial.begin( 9600 ); 
  //Serial.println("ENIGMA");

  // Set the main font.
  u8g.setFont( frakturcook16 );

  // Initialise the output ticker.
  ResetOutputTicker();

}

////////////////////////////////////////////////////////////////////////////////
// Reset the output ticker.
// Set the output ticker array to be all blank spaces and a null terminator.
////////////////////////////////////////////////////////////////////////////////
void  ResetOutputTicker ( void )
{
  // Loop though the array making each character a space.
  // We can skip the last one since we null terminate that.
  for (int i = 0; i < (sizeof(output_ticker) - 1); i++ )
  {
   output_ticker[i] = ' ';
  }
  // Null terminate!
  output_ticker[sizeof(output_ticker) - 1] = 0; 
  // Reset the counter too.
  output_ticker_counter = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Append the given letter to the output ticker.
// The ticker is a char array where characters are added to the right and 
// all other characters are shuffled one place to the right. 
////////////////////////////////////////////////////////////////////////////////
void  AppendOutputTicker ( char letter )
{  
  // Increment the counter.
  output_ticker_counter++;
  
  // If we have less than 5 characters just shift everything to the left
  // one space and add the new character to the end.
  if ( output_ticker_counter <= 5 )
  {
    // For every item in the array (except the null terminator).
    for (int i = 0; i < (sizeof(output_ticker)) - 1; i++ )
    {
     // Shuffle all letters left one place. 
     output_ticker[i] = output_ticker[i + 1];     
    }
  }
  else
  {
    // We have a group of 5 characters.
    // Rest the counter.
    output_ticker_counter = 1;
    
    // Shuffle everything but the null terminator 2 places to the left.
    for (int i = 0; i < (sizeof(output_ticker)) - 2; i++ )
    {
     // Shuffle all letters left two places. 
     output_ticker[i] = output_ticker[i + 2];     
    }
    // Add a space now.
    output_ticker[sizeof(output_ticker) - 3] = ' ';    
  }
  
  // Put the new letter on the end.
  output_ticker[sizeof(output_ticker) - 2] = letter;
  // Null terminate (just to be sure!).
  output_ticker[sizeof(output_ticker) - 1] = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 1 from keyboard through plugboard.
// In here we handle the steckering.
////////////////////////////////////////////////////////////////////////////////
int Transition_T1 ( int input )
{
  int output = 0;
  
  output = PlugBoardIn(input);
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 2 from plugboard to entry wheel.
// Entry wheel is in simple alphabetic order starting with A so input = output.
////////////////////////////////////////////////////////////////////////////////
int Transition_T2 ( int input )
{
  int output = 0;
  
  output = input;
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 3 through right rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T3 ( int input )
{
  int output = 0;
  
  output =  RotorIn ( input, right_rotor, right_rotor_position, right_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 4  through middle rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T4 ( int input )
{
  int output = 0;
  
  output =  RotorIn ( input, mid_rotor, mid_rotor_position, mid_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 5 through left rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T5 ( int input )
{
  int output = 0;
  
  output =  RotorIn ( input, left_rotor, left_rotor_position, left_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 6 through reflector.
////////////////////////////////////////////////////////////////////////////////
int Transition_T6 ( int input, const int* reflector )
{
  int output = 0;
  
  // Subtract 1 from the input to handle zero based array indexing.
  output = pgm_read_byte_near( &reflector[ input-1 ] );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 7 back through left rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T7 ( int input)
{
  int output = 0;
  
  output =  RotorOut ( input, left_rotor, left_rotor_position, left_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 8 back through middle rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T8 ( int input )
{
  int output = 0;
  
  output =  RotorOut ( input, mid_rotor, mid_rotor_position, mid_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 9 back through right rotor.
////////////////////////////////////////////////////////////////////////////////
int Transition_T9 ( int input )
{
  int output = 0;
  
  output =  RotorOut ( input, right_rotor, right_rotor_position, right_rotor_ring );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 10 back through entry wheel to plugboard.
////////////////////////////////////////////////////////////////////////////////
int Transition_T10 ( int input )
{
  int output = 0;
  
  output = input;
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// Transition 11 through plugboard to lightboard.
// In here handle the steckering.
////////////////////////////////////////////////////////////////////////////////
int Transition_T11 ( int input )
{
  int output = 0;
  
  output = PlugBoardOut( input );
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// 1st pass through the plugboard.
// Take the entry index and find what it is steckered to.
////////////////////////////////////////////////////////////////////////////////
int PlugBoardIn ( int input )
{ 
  int output = 0;
  
  // Find the value based on the input. Subtract 1 to account for
  // zero based array indexing.
  output = plugboard[input - 1];
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// 2nd pass through the plugboard.
// Search the array for the value and return the position.
////////////////////////////////////////////////////////////////////////////////
int PlugBoardOut ( int input )
{ 
  int output = 0;
  
  // Loop through the array.
  for ( int i = 0; i < 26; i++ )
  {
    // If the value here matches return the 
    // matching index.
    if ( plugboard[i] == input )
    { 
      output = i;
    }
  }
  // Add 1 to get from array index to actual value.
  output = output + 1;
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// 1st pass through each rotor.
// Take the entry index, take into account our current position and the 
// ring offset.
////////////////////////////////////////////////////////////////////////////////
int RotorIn ( int input, const int* rotor, int pos, int ring )
{
  int index = 0;  
  int output = 0;
  
  // Work out the offset given position and ring setting for this rotor.
  // We subract 1 from the position and ring values as these are not
  // indexes rather absolute positions. So a position or ring setting of 
  // 1 means no offset. We must handle wrapping of the value also.
  index = ( input ) + ( pos - 1 ) - ( ring - 1 );
  index = Wrap ( index ); 
  
  // Get the actual value fomr the array. We subtract 1 to account for zero 
  // based array indexing.
  output = pgm_read_byte_near( &rotor[ index - 1 ]);
  
  // The output must similarily account for position and ring setting.
  output = output - ( pos - 1 ) + ( ring - 1 );
  output = Wrap ( output ); 
  
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// 2st pass through each rotor.
// Take the entry index, take into account our current position and the 
// ring offset.
////////////////////////////////////////////////////////////////////////////////
int RotorOut ( int input, const int* rotor, int pos, int ring )
{
  int index = 0;  
  int output = 0;
  
  // Work out the offset given position and ring setting for this rotor.
  // We subract 1 from the position and ring values as these are not
  // indexes rather absolute positions. So a position or ring setting of 
  // 1 means no offset. We must handle wrapping of the value also.
  index = input + ( pos - 1 ) - ( ring - 1 );
  index = Wrap ( index ); 
  
  // Find the matching value.
  for ( int i = 1; i < 27; i++ )
  {
    // Subtract 1 to account for zero based array indexing.
    if ( pgm_read_byte_near( &rotor[i - 1] ) == index )
    { 
      output = i;
    }
  }
  // Get the output accounting for position and ring setting. And
  // Handle any wrapping needed.
  output =  output - ( pos - 1 ) + ( ring - 1 ); 
  output = Wrap ( output );
  return output;  
}

////////////////////////////////////////////////////////////////////////////////
// Make sure the wheel index is in the range of 1..26.
// The rotors are circular so we just wrap around.
////////////////////////////////////////////////////////////////////////////////
int Wrap ( int index )
{
  int i = index;
  
  // If positive see if we are over 26.
  if ( index > 0 )
  {  
    // If over 26 keep subrtacting 26 until we 
    // are back in the 1..26 range.
    while ( i > 26 )
    {
      i = i - 26;   
    }
  }
  else
  // If we are less than 1.
  {
    // We keep adding 26 until we are back in the
    // 1..26 range.
    while ( i < 1 )
    {
      i = i + 26;   
    }    
  }
  
  return i; 
}

////////////////////////////////////////////////////////////////////////////////
// Clear all steckers.
////////////////////////////////////////////////////////////////////////////////
void ClearSteckers ( void )
{
  // Iterate the plugboard array.
  for ( int i = 0; i < 26; i++ )
  { 
    plugboard[i] = i + 1;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Given a pair of letters swap them on the plug board.
// This returns the number of pairings made which is the number of old pairings
// broken minus the number of new pairings.
////////////////////////////////////////////////////////////////////////////////
int Stecker ( int letter_1, int letter_2 )
{
  int temp;
  int partner = 0;
  int pairings = 0;
  
  // Check if the first letter is already steckered.
  partner = IsSteckered ( letter_1 );
  // It is steckered.
  if ( partner ) 
  {
    // Unstecker it!
    plugboard[letter_1 - 1] = letter_1;
    plugboard[partner - 1] = partner;
    pairings--;
  }
  
  // Check if the second letter is already steckered.
  partner = IsSteckered ( letter_2 );
  // It is steckered.
  if ( partner ) 
  {
    // Unstecker it!
    plugboard[letter_2 - 1] = letter_2;
    plugboard[partner - 1] = partner;
    pairings--;
  }
  
  // If we are self steckering we don't need to do anything
  // else here else make the pairing.
  if ( letter_1 != letter_2 )
  {  
    // Now we swap the two value over in the plug board array.
    // We subtract 1 to handle the zero based array indexing.
    temp = plugboard[letter_1 - 1];
    plugboard[letter_1 - 1] = letter_2;
    plugboard[letter_2 - 1] = temp;
    pairings++;
  }
  return pairings;
}

////////////////////////////////////////////////////////////////////////////////
// Given a letter check if it's steckered. Return the index of it's pair if
// it is steckered or 0 if it is not.
////////////////////////////////////////////////////////////////////////////////
int IsSteckered ( int letter )
{
  // Look up this letter in the array based on it's index. Subtract 1 to 
  // account for zero based array indexing.
  if ( plugboard[letter - 1] == letter )
  { 
    // The letter is in it's own place so it is not steckered.
    return 0;
  }
  else
  {
    // We found another letter in it's place so it is steckered.
    // Return what it is.
    return plugboard[letter - 1];
  }
}

////////////////////////////////////////////////////////////////////////////////
// Move the rotors.
// In the ENIGMA machine all rotors try to move at once BEFORE each letter is 
// encoded. 
// The right most rotor moves every keypress. 
// The middle and left rotors can only move when the rotor to it's right is in 
// it's notch position. Or when the rotor to it's right is moving from it's 
// notch position.
// This explains the 'double stepping' anomaly. If the right rotor steps and 
// is in it's notch position the middle rotor will move. The middle rotor is
// also moving the right rotor but as that moves anyway we don't see anything
// unusual. If the middle rotor is now in it's notch position then next move
// will move the left most rotor. The left most one moving also moves the 
// middle rotor again so it has now stepped twice in two keypresses.
////////////////////////////////////////////////////////////////////////////////
void MoveRotors( void )
{
  
  // Do we move the left rotor? Check if we are in the mid rotors
  // notch.
  if ( mid_rotor_position == mid_rotor_notch ) 
  {
    left_rotor_position++;
    // Handle wrap around. 
    left_rotor_position = Wrap ( left_rotor_position );
   
    // Handle double stepping. Since the left rotor moved it
    // moves the middle one with it.
    mid_rotor_position++;
    // Handle wrap around. 
    mid_rotor_position = Wrap ( mid_rotor_position );
  }
  
  // Do we move the middle rotor.
  if ( right_rotor_position == right_rotor_notch )
  {
    mid_rotor_position++;
    // Handle wraparound. 
    mid_rotor_position = Wrap ( mid_rotor_position );    
    // Here since the middle rotor moved we should also move the
    // right most rotor. But as it is moving anyway we don't need to!    
  }
  
  // Right rotor. It always moves.
  right_rotor_position++; 
  // Handle wrap around.
  right_rotor_position = Wrap ( right_rotor_position );
}

////////////////////////////////////////////////////////////////////////////////
// Encode function. Simply transition the letter through the machine and
// back out again. Extra variables for clarity only.
////////////////////////////////////////////////////////////////////////////////
int Encode ( int input_letter )
{
  int letter;
  int encoded_letter;
  
  letter = Transition_T1 ( input_letter );        // Plugboard.  
  letter = Transition_T2 ( letter );              // Entry wheel.
  letter = Transition_T3 ( letter );              // Rotor 1.
  letter = Transition_T4 ( letter );              // Rotor 2.
  letter = Transition_T5 ( letter );              // Rotor 3.
  letter = Transition_T6 ( letter, reflector );   // Reflector.   
  letter = Transition_T7 ( letter );              // Rotor 3. 
  letter = Transition_T8 ( letter );              // Rotor 2.
  letter = Transition_T9 ( letter );              // Rotor 1.
  letter = Transition_T10 ( letter );             // Entry wheel.
  encoded_letter = Transition_T11 ( letter );     // Plugboard. 
  
  return encoded_letter;
}
   
////////////////////////////////////////////////////////////////////////////////
// Shows the steckered letters on the plugboard. Steckered letters are swapped 
// so are not at their own index. There can be 10 steckered letters for an M3
// ENIGMA machine. The pairs are shown split across two lines for displaying.
////////////////////////////////////////////////////////////////////////////////
void Plugboard_Selection_Show ( void ) 
{
  int line_index = 0;
  int pairs = 0;
  int pairs_array[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  bool bPaired = false; 
 
  // Make sure the strings are empty. 
  pb_line_1[0] = 0;
  pb_line_2[0] = 0;
  
  // Iterate the plugboard array.
  for ( int i = 0; i < 26; i++ )
  { 
    // We only need to find the first of a pair so 
    // we can use the pairs array to skip over already
    // known pairings.
    bPaired = false;
    for ( int j = 0; j < 10; j++ )
    {
      if ( pairs_array[j] == (i + 1) )
      {
        bPaired = true;
      }  
    }
    
    // If this letter isn't already paired and it is
    // steckered we need to store it.
    if ( !bPaired && ( plugboard[i] != (i + 1) ) )
    {
      // Put it in the pairs array so we know we found it.
      pairs_array[pairs++] = plugboard[i];
      
      // If we have exactly 6 pairs move to the next line
      // and put the new one there. 
      if ( pairs == 6 ) 
      {
        line_index = 0; // Reset the index.       
      }
      
      // The first 5 pairs go on the first line.
      if ( pairs < 6 ) 
      {
        // Space between pairs except before the first one.
        if ( pairs != 1 )
        {
          pb_line_1[line_index++] = ' '; 
        }  
        pb_line_1[line_index++] = (i + 1) + 64;
        pb_line_1[line_index++] = plugboard[i] + 64;
        pb_line_1[line_index] = 0;      
      }
      else 
      {
        // Space between pairs except before the first one.
        if ( pairs != 6 )
        {
          pb_line_2[line_index++] = ' '; 
        }
        // The next 5 pairs go on the second line.
        pb_line_2[line_index++] = (i + 1) + 64;
        pb_line_2[line_index++] = plugboard[i] + 64;
        pb_line_2[line_index] = 0;
      }        
    }     
  }  
}

////////////////////////////////////////////////////////////////////////////////
// After selecting the rotor we mark the current rotor as selected in the 
// rotor_selection array by setting it's value to 0. We then reset the index to 
// the next lowest available one.
////////////////////////////////////////////////////////////////////////////////
void Rotor_Selection_Done ( void ) 
{
  // Mark it as selected by putting a 0 in the array.
  rotor_selection[rotor_selection_index] = 0;
  
  // Reset the index so it shows the next available rotor.
  rotor_selection_index = 0;
  
  // Now loop through looking for the next non selected one and
  // set the index to that one.
  while ( rotor_selection[rotor_selection_index] == 0 )
  {
    rotor_selection_index++;
    if ( rotor_selection_index > 4 )
    {
      rotor_selection_index = 0;
    }      
  }  
}

////////////////////////////////////////////////////////////////////////////////
// We reset the rotor selection here. Loop through the selection array and put
// each rotor back into place.
////////////////////////////////////////////////////////////////////////////////
void Rotor_Selection_Reset ( void ) 
{
  // Reset the selections. 
  for ( int i = 0; i < 5; i++ )
  {
    rotor_selection[i] = i + 1;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Show the next selected rotor based on dir(ection). 0 means look left while
// 1 means look right. As rotors are selected they are marked as so by a 0 in 
// the rotor_selection array and we can skip over showing them.
////////////////////////////////////////////////////////////////////////////////
void Rotor_Selection_Show ( int dir ) 
{ 
  // Get the previous.
  if ( dir == 0 ) 
  { 
    // Get the previous one.
    rotor_selection_index--;
    // Handle wrapping.
    if ( rotor_selection_index < 0 )
    {
      rotor_selection_index = 4;
    }
    // Skip over any already selected.    
    while ( rotor_selection[rotor_selection_index] == 0 )
    {
      rotor_selection_index--;
      // Handle wrapping.
      if ( rotor_selection_index < 0 )
      {
        rotor_selection_index = 4;
      }      
    } 
  }    
  else  
  {
    // Get the next one.
    rotor_selection_index++;
    // Handle wrapping.
    if ( rotor_selection_index > 4 )
    {
      rotor_selection_index = 0;
    }     
    // Skip over any already selected.
    while ( rotor_selection[rotor_selection_index] == 0 )
    {
      rotor_selection_index++;
      // Handle wrapping.
      if ( rotor_selection_index > 4 )
      {
        rotor_selection_index = 0;
      }      
    }              
  }    
}

////////////////////////////////////////////////////////////////////////////////
// Get the correct rotor pointer from an index given.
////////////////////////////////////////////////////////////////////////////////
const int* GetRotor ( int rotor )
{
  const int* rotor_array = 0;
  
  switch ( rotor )
  {
    case 0:
      rotor_array = &rotor_I[0];
      break;
    case 1:
      rotor_array = &rotor_II[0];
      break;
    case 2:
      rotor_array = &rotor_III[0];
      break;
    case 3:
      rotor_array = &rotor_IV[0];
      break;
    case 4:
      rotor_array = &rotor_V[0];
      break;
    default:
      rotor_array = &rotor_I[0];
  }
  return rotor_array;
}

////////////////////////////////////////////////////////////////////////////////
// drawStrC
// Use the drawSrt function but draw the text centred horizontally based on the
// string width.
////////////////////////////////////////////////////////////////////////////////
void drawStrC( int vert, char* string ) 
{
   int width = u8g.getStrWidth(string);
   u8g.drawStr(64 - (width/2), vert, string);
}

////////////////////////////////////////////////////////////////////////////////
// drawStrC_sel
// Use the drawSrt function but draw the text centred horizontally with 
// selection indication triangles either side of the string.
////////////////////////////////////////////////////////////////////////////////
void drawStrC_sel( int vert, char* string ) 
{
   u8g.setFontRefHeightAll();
   int width = u8g.getStrWidth(string);   
   int height = u8g.getFontAscent();
   
   // Draw the string.
   u8g.drawStr(64 - (width/2), vert, string);
   
// Left triangle.
   u8g.drawTriangle ( 64 - (width/2) - 20, vert - (height/2) + 2,
                      64 - (width/2) - 10, vert - height + 4,
                      64 - (width/2) - 10, vert );
   
   // Right triangle.
   u8g.drawTriangle ( 64 + (width/2) + 20, vert - (height/2) + 2,
                      64 + (width/2) + 10, vert - height + 4,
                      64 + (width/2) + 10, vert );
}

////////////////////////////////////////////////////////////////////////////////
// drawStrL
// Use the drawSrt function but draw the text on the far left.
////////////////////////////////////////////////////////////////////////////////
void drawStrL( int vert, char* string ) 
{   
   u8g.drawStr180(15, vert, string);
}

////////////////////////////////////////////////////////////////////////////////
// drawStrR
// Use the drawSrt function but draw the text spaced from the right based on the
// string width.
////////////////////////////////////////////////////////////////////////////////
void drawStrR( int vert, char* string ) 
{
   int width = u8g.getStrWidth(string);
   u8g.drawStr180(110 + width, vert, string);
}

////////////////////////////////////////////////////////////////////////////////
// Draw
// U8GLIB draw function.
////////////////////////////////////////////////////////////////////////////////
void draw(void) 
{  
  // Graphic commands to redraw the complete screen should be placed here.     
  switch ( screen ) 
  {
    // Splash bitmap.
    case 0:     
      DisplaySplash();      
    break;
  
    // Rotor selection and order summary.
    case 1:
      RotorSelection();
    break;    
    
    // Left rotor selection.
    case 2:
      LeftRotorSelect();
    break;

    // Middle rotor selection.      
    case 3:
      MiddleRotorSelect();    
    break;
      
    // Right rotor selection.
    case 4:
      RightRotorSelect();
    break;
    
    // Ring settings.
    case 5:
      RingSelection();
    break;  
                   
    // Left rotor ring selection.  
    case 6:
      LeftRingSelect();
    break;
 
     // Middle rotor ring selection.     
    case 7:
      MiddleRingSelect();
    break;
    
    // Right rotor ring selection.
    case 8:
      RightRingSelect();
    break;

    // Plugboard settings.
    case 9:  
      PlugboardSelection();
    break;
    
    // Plugboard editing.
    case 10:  
      PlugboardSelect();
    break;

    // Rotor starting positions.
    case 11:
      StartPositionSelection();
    break;  
    
    // Left rotor start position.
    case 12:
      LeftStartPositionSelect();  
    break;

    // Middle rotor start position.
    case 13:
      MiddleStartPositionSelect();  
    break;
    
    // Right rotor start position.
    case 14:
      RightStartPositionSelect();  
    break;
    
    // Reflector selection.
    case 15:
      ReflectorSelection();  
    break;
            
    // Main encoding/decoding screen.
    case 16:
      CodingScreen();    
    break;
      
    default:      
      u8g.drawBitmap(0, 0, 16, 64, logo );
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display the ENIGMA splash screen.
////////////////////////////////////////////////////////////////////////////////
void DisplaySplash ( void )
{
  // Draw the logo.
  u8g.drawBitmapP(0, 0, 16, 64, logo );

  if ( RTC_read == true )
  {
    // Update time.
    if ( secondCounter > onesecond )
    {
      secondCounter = 0;
      IncrementSeconds();    
      //Serial.print( hoursString );
      //Serial.print(":");
      //Serial.print( minutesString );
      //Serial.print(":");
      //Serial.println( seconds );
    }

    // Draw the time.
    BuildHoursString();
    BuildMinutesString();
    drawStrR(time_y, hoursString );
    drawStrL(time_y, minutesString );
  }
    
  // Do we exit the splash screen?
  if ( !LeftKeyDown() && !RightKeyDown() && SelKeyPressed() )
  {
    // Store the time if it was updated.
    if ( timeUpdated == true )
    {
      StoreTime();
    }
    // Go to the next screen.
    screen = 1;      
  }
  
  // Do we increment the hours?
  if ( RightKeyDown() && SelKeyPressed() )
  {
    timeUpdated = true;
    IncrementHours();         
  }

  // Do we increment the minutes?
  if ( LeftKeyDown() && SelKeyPressed() )
  {
    timeUpdated = true;
    IncrementMinutes(false);         
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display the rotor selection screen.
////////////////////////////////////////////////////////////////////////////////
void RotorSelection ( void )
{
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Walzenlage" );
  if ( edit_Walzenlage )
  {
    drawStrC_sel( line_2_y, "Ändern" );
  }
  else    
  {
   drawStrC_sel( line_2_y, "Beibehalten" );
  }
  
  char selected_rotors[10] = {0};
  strcat( selected_rotors, rotor_names[pgm_read_byte_near(&left_rotor[27]) - 1] );
  strcat( selected_rotors, " ");
  strcat( selected_rotors, rotor_names[pgm_read_byte_near(&mid_rotor[27]) - 1] );
  strcat( selected_rotors, " ");
  strcat( selected_rotors, rotor_names[pgm_read_byte_near(&right_rotor[27]) - 1] );
  u8g.setFont(u8g_font_9x18);
  drawStrC ( 60, selected_rotors );
  
  // Set up the notch positions.
  left_rotor_notch = pgm_read_byte_near(&left_rotor[26]);  
  mid_rotor_notch = pgm_read_byte_near(&mid_rotor[26]);
  right_rotor_notch = pgm_read_byte_near(&right_rotor[26]);
  
  // If the left ro right keys are pressed toggle the selection.
  if ( LeftKeyPressed() || RightKeyPressed() )
  {
    edit_Walzenlage = !edit_Walzenlage;
  }

  // If the select key is pressed go to the next screen based on selection.
  if ( SelKeyPressed() )
  {
    if ( edit_Walzenlage )
    {
      // Go to the left rotor selection screen.
      screen = 2;
    }
     else
    {
      // Go to the ring position selection screen.
      screen = 5;
    }       
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display the left rotor selection screen.
////////////////////////////////////////////////////////////////////////////////
void LeftRotorSelect ( void )
{      
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Walzenlage" );
  drawStrC( line_2_y, "Links Walzen" );        
  
  // Show left selection.
  if ( LeftKeyPressed() )
  {
    Rotor_Selection_Reset();
    Rotor_Selection_Show ( 0 );      
  }

  // Show right selection.
  if ( RightKeyPressed() )
  {
    Rotor_Selection_Reset();
    Rotor_Selection_Show ( 1 );            
  }
  
  // Select key pressed.
  if ( SelKeyPressed() )
  {
    left_rotor = GetRotor ( rotor_selection_index );
    Rotor_Selection_Done();
    screen++;         
  }
  
  // Display the selected rotor.
  u8g.setFont(u8g_font_9x18);
  drawStrC_sel( 60, rotor_names[rotor_selection_index] );        
}

////////////////////////////////////////////////////////////////////////////////
// Display the middle rotor selection screen.
////////////////////////////////////////////////////////////////////////////////
void MiddleRotorSelect ( void )
{      
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Walzenlage" );
  drawStrC( line_2_y, "Mitte Walzen" );        
  
  // Show left selection.
  if ( LeftKeyPressed() )
  {
    Rotor_Selection_Show ( 0 );      
  }

  // Show right selection.
  if ( RightKeyPressed() )
  {
    Rotor_Selection_Show ( 1 );            
  }
  
  // Select key pressed.
  if ( SelKeyPressed() )
  {
    mid_rotor = GetRotor ( rotor_selection_index ); 
    Rotor_Selection_Done(); 
    screen++;    
  }
  
  // Display the selected rotor.
  u8g.setFont(u8g_font_9x18);
  drawStrC_sel( 60, rotor_names[rotor_selection_index] );      
}

////////////////////////////////////////////////////////////////////////////////
// Display the right rotor selection screen.
////////////////////////////////////////////////////////////////////////////////
void RightRotorSelect ( void )
{      
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Walzenlage" );
  drawStrC( line_2_y, "Recht Walzen" );        
  
   // Show left selection.
  if ( LeftKeyPressed() )
  {
    Rotor_Selection_Show ( 0 );      
  }

  // Show right selection.
  if ( RightKeyPressed() )
  {
    Rotor_Selection_Show ( 1 );            
  }
  
  // Select key pressed.
  if ( SelKeyPressed() )
  {
    right_rotor = GetRotor ( rotor_selection_index);        
    Rotor_Selection_Done();
    edit_Walzenlage = false;
    // Go back to the summary screen.
    screen = 1;   
  }
  
  // Display the selected rotor.
  u8g.setFont(u8g_font_9x18);
  drawStrC_sel( 60, rotor_names[rotor_selection_index] );       
}

////////////////////////////////////////////////////////////////////////////////
// Display the ring selection screen.
////////////////////////////////////////////////////////////////////////////////
void RingSelection ( void )
{
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Ringstellung" );
  if ( edit_Ringstellung )
  {
    drawStrC_sel( line_2_y, "Ändern" );
  }
  else    
  {
   drawStrC_sel( line_2_y, "Beibehalten" );
  }
  
  char selected_rings[10] = {0};
  char buf[2] = { 0, 0 };
  itoa( left_rotor_ring, buf, 10 );
  strcat( selected_rings, buf );
  strcat( selected_rings, " ");
  itoa( mid_rotor_ring, buf, 10 );
  strcat( selected_rings, buf );
  strcat( selected_rings, " ");
  itoa( right_rotor_ring, buf, 10 );
  strcat( selected_rings, buf );
  u8g.setFont(u8g_font_9x18);
  drawStrC ( 60, selected_rings );
  
  // If left or right key pressed toggle selection.
  if ( LeftKeyPressed() || RightKeyPressed() )
  {
    edit_Ringstellung = !edit_Ringstellung;
  }
  
  // If the select key is pressed go to the next screen based on selection.
  if ( SelKeyPressed() )
  {
    if ( edit_Ringstellung )
    {
      // Go to the select left ring screen.
      screen = 6;
    }
     else
    {
      // Go to the plugboard settings screen.
      screen = 9;
    }       
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display the left ring selection screen.
////////////////////////////////////////////////////////////////////////////////
void LeftRingSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Ringstellung" );
  drawStrC( line_2_y, "Links" );        
  
  if ( LeftKeyPressed() )
  {
    left_rotor_ring--;
    left_rotor_ring = Wrap ( left_rotor_ring );
  }

  if ( RightKeyPressed() )
  {
    left_rotor_ring++;
    left_rotor_ring = Wrap ( left_rotor_ring );        
  }
  
  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    screen++;         
  }
             
  // Display the ring setting.
  u8g.setFont(u8g_font_9x18);
  char buf[2] = {0, 0};
  itoa( left_rotor_ring, buf, 10 );
  drawStrC_sel( 60, buf ); 
}

////////////////////////////////////////////////////////////////////////////////
// Display the middle ring selection screen.
////////////////////////////////////////////////////////////////////////////////
void MiddleRingSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Ringstellung" );
  drawStrC( line_2_y, "Mitte" );        
  
  if ( LeftKeyPressed() )
  {
    mid_rotor_ring--;
    mid_rotor_ring = Wrap ( mid_rotor_ring );
  }

  if ( RightKeyPressed() )
  {
    mid_rotor_ring++;
    mid_rotor_ring = Wrap ( mid_rotor_ring );        
  }
  
  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    screen++;         
  }
             
  // Display the ring setting.
  u8g.setFont(u8g_font_9x18);
  char buf[2] = {0, 0};
  itoa( mid_rotor_ring, buf, 10 );
  drawStrC_sel( 60, buf ); 
}   

////////////////////////////////////////////////////////////////////////////////
// Display the right ring selection screen.
////////////////////////////////////////////////////////////////////////////////
void RightRingSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Ringstellung" );
  drawStrC( line_2_y, "Recht" );        
  
  if ( LeftKeyPressed() )
  {
    right_rotor_ring--;
    right_rotor_ring = Wrap ( right_rotor_ring );
  }

  if ( RightKeyPressed() )
  {
    right_rotor_ring++;
    right_rotor_ring = Wrap ( right_rotor_ring );        
  }

  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    edit_Ringstellung = false;
    // Go back to the summary screen.
    screen = 5;          
  } 
             
  // Display the ring setting.
  u8g.setFont(u8g_font_9x18);
  char buf[2] = {0, 0};
  itoa( right_rotor_ring, buf, 10 );
  drawStrC_sel( 60, buf ); 
}

////////////////////////////////////////////////////////////////////////////////
// Display the plugboard selection screen.
////////////////////////////////////////////////////////////////////////////////
void PlugboardSelection ( void )
{
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Steckerverbindungen" );
  if ( edit_steckerverbindungen )
  {
    drawStrC_sel( line_2_y, "Ändern" );
  }
  else    
  {
   drawStrC_sel( line_2_y, "Beibehalten" );
  }
  
  // If left or right key pressed toggle selection.
  if ( LeftKeyPressed() || RightKeyPressed() )
  {
    edit_steckerverbindungen = !edit_steckerverbindungen;
  }
  
  // Select key pressed. 
  if ( SelKeyPressed())
  {
    if ( edit_steckerverbindungen )
    {      
      // Clear the plugboard and go to the plug board setting screen.        
      ClearSteckers();
      first_of_pair = 0;
      second_of_pair = 0;
      pairs = 0;      
      screen = 10;
    }
     else
    {
      // Go to the rotor start position screen.
      screen = 11;
    }
    edit_steckerverbindungen = false;    
  }

  Plugboard_Selection_Show();
  u8g.setFont(u8g_font_6x13);
  drawStrC( 48, pb_line_1 );
  drawStrC( 60, pb_line_2 );
}

////////////////////////////////////////////////////////////////////////////////
// Choose the plugboard connections.
////////////////////////////////////////////////////////////////////////////////
void PlugboardSelect ( void )
{
  char text[12] = {0};
  char ch[3] = {0, 0};

  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Steckerverbindungen" );  

  text[0] = 0;
  strcat( text, "Paare: " );
  itoa( pairs, ch, 10 );
  strcat( text, ch );
  drawStrC( line_2_y, text );
  
// Show left selection.
  if ( LeftKeyPressed() )
  {
    first_of_pair++;
    // Handle wrap around.
    if ( first_of_pair > 26 )
    {
      first_of_pair = 0;
    }    
  }

  // Show right selection.
  if ( RightKeyPressed() )
  {
    second_of_pair++;
    // Handle wrap around.
    if ( second_of_pair > 26 )
    {
      second_of_pair = 0;
    }     
  }
  
  // Select key pressed.
  if ( SelKeyPressed() )
  {
    // If either of the two values are both blank we are done. 
    if ( ( first_of_pair == 0 ) || ( second_of_pair == 0 ) )
    {
      // Go back to the plug board selections screen.
      screen = 9;
    }
    else
    {
      // Stecker these two values. If already steckered we break those
      // connection to make new ones.
      int newPairs = Stecker( first_of_pair, second_of_pair );
      pairs = pairs + newPairs;
      // If we have 10 go back to the plug board selections screen.
      if  ( pairs >= 10 ) 
      {      
        screen = 9;
      }
    }
  }
  
  // Display selections.
  text[0] = 0;
  if ( first_of_pair == 0 )
  {
    ch[0] = ' '; 
  }
  else
  {
    ch[0] = first_of_pair + 64;
  }  
  ch[1] = 0;
  strcat( text, ch );
  strcat( text, "-" );
  if ( second_of_pair == 0 )
  {
    ch[0] = ' '; 
  }
  else
  {
    ch[0] = second_of_pair + 64;
  } 
  ch[1] = 0;
  strcat( text, ch );
  
  u8g.setFont(u8g_font_9x18);  
  drawStrC_sel ( 64, text );    
}

////////////////////////////////////////////////////////////////////////////////
// Display the rotor start position selection screen.
////////////////////////////////////////////////////////////////////////////////
void StartPositionSelection ( void )
{
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Grundstellung" );
  if ( edit_Grundstellung )
  {
    drawStrC_sel( line_2_y, "Ändern" );
  }
  else    
  {
   drawStrC_sel( line_2_y, "Beibehalten" );
  }
  
  char star_positions[10] = {0};
  char letter[2] = {0, 0};
  letter[0] = left_rotor_position + 64;
  strcat( star_positions, letter );
  strcat( star_positions, " ");
  letter[0] = mid_rotor_position + 64;
  strcat( star_positions, letter );
  strcat( star_positions, " ");
  letter[0] = right_rotor_position + 64;
  strcat( star_positions, letter );
  u8g.setFont(u8g_font_9x18);
  drawStrC ( 60, star_positions );
  
  // If left or right key pressed toggle selection.
  if ( LeftKeyPressed() || RightKeyPressed() )
  {
    edit_Grundstellung = !edit_Grundstellung;
  }
 
  // If the selection key is pressed go to the left position select screen.
  if ( SelKeyPressed() )
  {
    if ( edit_Grundstellung )
    {
      // Left rotor start position selection.
      screen = 12;
    }
     else
    {
      // Reflector selection screen.
      screen = 15;
    }       
  }
}

////////////////////////////////////////////////////////////////////////////////
// Display the left rotor start position selection screen.
////////////////////////////////////////////////////////////////////////////////
void LeftStartPositionSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Grundstellung" );
  drawStrC( line_2_y, "Links" );        
  
  if ( LeftKeyPressed() )
  {
    left_rotor_position--;
    left_rotor_position = Wrap ( left_rotor_position );
  }

  if ( RightKeyPressed() )
  {
    left_rotor_position++;
    left_rotor_position = Wrap ( left_rotor_position );        
  }
  
  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    screen++;         
  }
         
  // Display the start position.
  u8g.setFont(u8g_font_9x18);      
  char letter[2] = {0, 0};
  letter[0] = left_rotor_position + 64;
  drawStrC_sel( 60, letter ); 
}

////////////////////////////////////////////////////////////////////////////////
// Display the middle rotor start position selection screen.
////////////////////////////////////////////////////////////////////////////////
void MiddleStartPositionSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Grundstellung" );
  drawStrC( line_2_y, "Mitte" );        
  
  if ( LeftKeyPressed() )
  {
    mid_rotor_position--;
    mid_rotor_position = Wrap ( mid_rotor_position );
  }

  if ( RightKeyPressed() )
  {
    mid_rotor_position++;
    mid_rotor_position = Wrap ( mid_rotor_position );        
  }
  
  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    screen++;         
  }
             
  // Display the start position.

  u8g.setFont(u8g_font_9x18);
  char letter[2] = {0, 0};
  letter[0] = mid_rotor_position + 64;
  drawStrC_sel( 60, letter ); 
}

////////////////////////////////////////////////////////////////////////////////
// Display the right rotor start position selection screen.
////////////////////////////////////////////////////////////////////////////////
void RightStartPositionSelect ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Grundstellung" );
  drawStrC( line_2_y, "Recht" );        
  
  if ( LeftKeyPressed() )
  {
    right_rotor_position--;
    right_rotor_position = Wrap ( right_rotor_position );
  }

  if ( RightKeyPressed() )
  {
    right_rotor_position++;
    right_rotor_position = Wrap ( right_rotor_position );        
  }
  
  // Sel key pressed, go to the next screen.
  if ( SelKeyPressed() )
  {
    edit_Grundstellung = false;
    // Go back to the summary screen.
    screen = 11;             
  }
  
  // Display the start position.
  u8g.setFont(u8g_font_9x18);
  char letter[2] = {0, 0};
  letter[0] = right_rotor_position + 64;
  drawStrC_sel( 60, letter ); 
}

////////////////////////////////////////////////////////////////////////////////
// Display the reflector selection screen.
////////////////////////////////////////////////////////////////////////////////
void ReflectorSelection ( void )
{    
  u8g.setFont( frakturcook16 );
  drawStrC( line_1_y, "Umkehrwalze" );          
  
  // If left or right key pressed toggle selection.
  if ( LeftKeyPressed() || RightKeyPressed() )
  {
    reflector_B = !reflector_B;
  }
    
  // Selection key pressed. Select reflector.
  if ( SelKeyPressed() )
  {
   if ( reflector_B )
    {
      reflector = &ref_B[0];
    }
     else
    {
      reflector = &ref_C[0];        
    }
    // Go to the encoding screen.
    screen = 16; 
    // Always start coding on A.
    input_letter = 1;    
  }
             
  // Display the reflector.
  u8g.setFont(u8g_font_9x18);
  if ( reflector_B )
  {
    drawStrC_sel( 60, "B" );
  }
   else
  {
    drawStrC_sel( 60, "C" );
  }      
}

////////////////////////////////////////////////////////////////////////////////
// Main coding/decoding screen.
////////////////////////////////////////////////////////////////////////////////
void CodingScreen ( void )
{   
  int h;
  int w; 
  char star_positions[10] = {0};
  char letter[2] = {0, 0};
  
  // Work out text height and width.
  u8g.setFont( u8g_font_9x18 );
  u8g.setFontRefHeightText();            
  h = u8g.getFontAscent() - u8g.getFontDescent();
  w = u8g.getStrWidth("A");
  
  // Draw each rotor and it's surrounding box.
  letter[0] = left_rotor_position + 64;
  u8g.drawStr ( 48, 20, letter );
  u8g.drawFrame( 46, 20 - h - 2, w + 4, h + 7 );
  
  letter[0] = mid_rotor_position + 64;
  u8g.drawStr ( 60, 20, letter );
  u8g.drawFrame( 58, 20 - h - 2, w + 4, h + 7 );
  
  letter[0] = right_rotor_position + 64;
  u8g.drawStr ( 72, 20, letter );
  u8g.drawFrame( 70, 20 - h - 2, w + 4, h + 7 );
  
  // Select letter.
  if ( LeftKeyPressed() )
  {
    input_letter--;    
    // We have two extra positions here to 
    // give us a way to clear the output or exit.
    if ( input_letter < 1 )
    {  
      input_letter = 28;
    }
  }
  
  // Select letter.
  if ( RightKeyPressed() )
  {
    input_letter++;    
    // We have two extra positions here to 
    // give us a way to clear the output or exit.
    if ( input_letter > 28 )
    {  
      input_letter = 1;
    }    
  }
  
  // Sel key pressed. Check if we wish to exit. If so go back to
  // the setting screen. If not move rotors and encode letter.
  if ( SelKeyPressed() )
  {
    if ( input_letter == 27 )
    {
      // Go back to the main settings page.
      screen = 1;
    }
    else if ( input_letter == 28 )
    {
      // Clear the output ticker.
      ResetOutputTicker();
    }
    else
    {
      MoveRotors();
      output_letter = Encode( input_letter );         
      AppendOutputTicker ( output_letter + 64 );         
      
      //Serial.print( char(input_letter + 64) );
      //Serial.print("-");
      //Serial.println( char(output_letter + 64) );
  
    }
  }
  
  // Show the input letter.
  if ( input_letter == 27 )
  {  
    drawStrC_sel ( 60, "Ausgang" );  
  }
  else if ( input_letter == 28 )
  {
    drawStrC_sel ( 60, "Klar" );
  }
  else
  {
    letter[0] = input_letter + 64;
    drawStrC_sel( 60, letter );  
  }
  
  // Show the output letter ticker.
  u8g.setFont(u8g_font_6x13);
  drawStrC( 42, output_ticker );

}

////////////////////////////////////////////////////////////////////////////////
// Was the left key pressed?
////////////////////////////////////////////////////////////////////////////////
bool LeftKeyPressed ( void )
{  
  int value = debouncerLeft.fell();
    
  if ( value == 1 ) // Left key pressed. 
  {
    return true;
  } 
  else
  {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Is the left key currently pressed?
////////////////////////////////////////////////////////////////////////////////
bool LeftKeyDown ( void )
{  
  int value = debouncerLeft.read();
    
  if ( value == 0 ) // Left key down. 
  {
    return true;
  } 
  else
  {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Was the right key pressed?
////////////////////////////////////////////////////////////////////////////////
bool RightKeyPressed ( void )
{    
   int value = debouncerRight.fell();
    
  if ( value == 1 ) // Right key pressed. 
  {
    return true;
  } 
  else
  {
    return false;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Is the right key down?
////////////////////////////////////////////////////////////////////////////////
bool RightKeyDown ( void )
{    
   int value = debouncerRight.read();
    
  if ( value == 0 ) // Right key down. 
  {
    return true;
  } 
  else
  {
    return false;
  }
}
////////////////////////////////////////////////////////////////////////////////
// Is the select key pressed?
////////////////////////////////////////////////////////////////////////////////
bool SelKeyPressed ( void )
{    
  int value = debouncerSelect.fell();
    
  if ( value == 1 ) // Select key pressed. 
  {
    return true;
  } 
  else
  {
    return false;
  }
}
    
////////////////////////////////////////////////////////////////////////////////
// Main loop.
////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Picture loop.
  u8g.firstPage();  
  do 
  {
    debouncerLeft.update();
    debouncerRight.update();
    debouncerSelect.update();
    draw();    
  } 
  while( u8g.nextPage() ); 
  
  // Read the time once when powered up.
  if ( RTC_read == false )
  {

    // Get the time.
    GetTime();

    // Set the flag so we don't do this again.
    RTC_read = true;
  } 
}

