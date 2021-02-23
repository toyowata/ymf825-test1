#include "mbed.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);
SPI _spi(p5, p6, p7); // mosi, miso, sclk
DigitalOut _cs(p8);
DigitalOut _reset(p9);

#define OUTPUT_power 0

void if_write(int addr,int* data,int num) {
    int i;
    _cs = 0;
    _spi.write(addr);
    for(i = 0; i < num; i++){
      _spi.write(data[i]);    
    }
    _cs = 1;
}

void if_s_write(int addr,int data){
    if_write(addr, &data, 1);
}

void init_825(void) {
    _reset = 0;
    wait_ms(1);
    _reset = 1;

    if_s_write( 0x1D, OUTPUT_power );
    if_s_write( 0x02, 0x0E );
    wait_ms(1);
    if_s_write( 0x00, 0x01 ); //CLKEN
    if_s_write( 0x01, 0x00 ); //AKRST
    if_s_write( 0x1A, 0xA3 );
    wait_ms(1);
    if_s_write( 0x1A, 0x00 );
    wait_ms(30);
    if_s_write( 0x02, 0x04 ); //AP1,AP3
    wait_ms(1);
    if_s_write( 0x02, 0x00 );
    //add
    if_s_write( 0x19, 0xF0 ); //MASTER VOL
    if_s_write( 0x1B, 0x3F ); //interpolation
    if_s_write( 0x14, 0x00 ); //interpolation
    if_s_write( 0x03, 0x01 ); //Analog Gain
   
    if_s_write( 0x08, 0xF6 );
    wait_ms(21);
    if_s_write( 0x08, 0x00 );
    if_s_write( 0x09, 0xF8 );
    if_s_write( 0x0A, 0x00 );
   
    if_s_write( 0x17, 0x40 ); //MS_S
    if_s_write( 0x18, 0x00 );
}


void set_tone(void){
    int tone_data[35] ={
    0x81,//header
    //T_ADR 0
    0x01,0x85,
    0x00,0x7F,0xF4,0xBB,0x00,0x10,0x40,
    0x00,0xAF,0xA0,0x0E,0x03,0x10,0x40,
    0x00,0x2F,0xF3,0x9B,0x00,0x20,0x41,
    0x00,0xAF,0xA0,0x0E,0x01,0x10,0x40,
    0x80,0x03,0x81,0x80,
    };
  
    if_s_write( 0x08, 0xF6 );
    wait_ms(1);
    if_s_write( 0x08, 0x00 );
  
    if_write( 0x07, &tone_data[0], 35 );//write to FIFO
}

void set_ch(void){
    if_s_write( 0x0F, 0x30 );// keyon = 0
    if_s_write( 0x10, 0x71 );// chvol
    if_s_write( 0x11, 0x00 );// XVB
    if_s_write( 0x12, 0x08 );// FRAC
    if_s_write( 0x13, 0x00 );// FRAC  
}

void keyon(unsigned char fnumh, unsigned char fnuml){
    if_s_write( 0x0B, 0x00 );//voice num
    if_s_write( 0x0C, 0x54 );//vovol
    if_s_write( 0x0D, fnumh );//fnum
    if_s_write( 0x0E, fnuml );//fnum
    if_s_write( 0x0F, 0x40 );//keyon = 1  
}

void keyoff(void){
    if_s_write( 0x0F, 0x00 );//keyon = 0
}

// main() runs in its own thread in the OS
int main() {
    pc.baud(115200);
    pc.printf("\nYMF826 test program\n");
    _cs = 1;
    _spi.format(8, 3);
    _spi.frequency(12000000);

    init_825();
    set_tone();
    set_ch();
  
    while (true) {
        led1 = !led1;
        keyon(0x14,0x65);
        wait_ms(500);
        keyoff();
        wait_ms(200);
        keyon(0x1c,0x11);
        wait_ms(500);
        keyoff();
        wait_ms(200);
        keyon(0x1c,0x42);
        wait_ms(500);
        keyoff();
        wait_ms(200);
        keyon(0x1c,0x5d);
        wait_ms(500);
        keyoff();
        wait_ms(200);
        keyon(0x24,0x17);
        wait_ms(500);
        keyoff();
        wait_ms(200);
    }
}

