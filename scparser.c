#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef unsigned char byte;

#define _9_BIT_PRECISION 0
#define _10_BIT_PRECISION 32
#define _11_BIT_PRECISION 64
#define _12_BIT_PRECISION 96

byte precision = -1;

int c;
FILE *file;

byte temp_msb;
byte temp_lsb;

byte configuration;

byte *scratchpad;
int indx = 0;

void read_byte_from_file(FILE *file, char *byte_chars) {
	byte_chars[0] = getc(file);
	byte_chars[1] = getc(file);
        byte_chars[2] = '\0';
	getc(file); // skip space char

        byte tmp1;
        byte tmp2;
        
        int number = (int)strtol(byte_chars, 0, 16);       
        
	sscanf(byte_chars, "%2hx", &(scratchpad[indx]));
	indx++;
}

void print_byte(byte byte_data) {
	byte x = 128;
	int i = 0;
	for(i=0; i<8; i++) {
		if((byte_data & x) > 0) {
			putchar('1');
		} else {
			putchar('0');
		}
		x >>= 1;
	}
	putchar(' ');
}

byte get_precision(byte configuration) {
	byte template = 96;
	byte precision = configuration & template;
	printf("precision: %d\n", precision);
	return precision;
}

float parse_temperature() {
	float result = 0.0f;

	int i;
	byte increment = 16;

	for(i=0; i<3; i++) {
//		printf("temp_msb: %d | increment: %d\n", temp_msb, increment);
		if((temp_msb & 1) == 1) {
			result += increment;
		}
		increment <<= 1;
		temp_msb >>= 1;
	}

	increment = 8;
	for(i=0; i<4; i++) {		
//		printf("temp_lsb: %d | increment: %d\n", temp_lsb, increment);
		if ((temp_lsb & 128) == 128) {
			result += increment;
		}
		increment >>= 1;
		temp_lsb <<= 1;
	}

	if (_9_BIT_PRECISION <= precision) {
		if((temp_lsb & 128) == 128) {
			result += 0.5f;
		}
		temp_lsb <<= 1;
	}
	if (_10_BIT_PRECISION <= precision) {
		if((temp_lsb & 128) == 128) {
			result += 0.25f;
		}
		temp_lsb <<= 1;
	}
	if (_11_BIT_PRECISION <= precision) {
		if((temp_lsb & 128) == 128) {
			result += 0.125f;
		}
		temp_lsb <<= 1;
	}
	if (_12_BIT_PRECISION <= precision) {
		if((temp_lsb & 128) == 128) {
			result += 0.0625f;
		}
		temp_lsb <<= 1;
	}

	return result;
}

void print_scratchpad() {
	int i;
	for(i=0; i<8; i++) {
		print_byte(scratchpad[i]);
	}
	putchar('\n');
}

void computeCRC() {
    //print_scratchpad();
    byte shift_r = 0;
    
    byte current_byte;
    
    int i, j;
    byte input_bit, fb;
    
    //for(i=7; i>=1; i--) { // rom crc input3.txt
    for(i=0; i<8; i++) { // scratchpad crc
        current_byte = scratchpad[i];
        //printf("\n== %x ==\n\n", current_byte);
        for(j=0; j<8; j++) {
            //printf("shift_r: "); print_byte(shift_r); putchar('\n');            
            
            input_bit = current_byte & 1;
            current_byte >>= 1;
            //printf("input_bit: %d\n", input_bit);
            
            fb = (shift_r & 1) ^ input_bit;
            
            shift_r >>= 1;  
            if (fb == 1) {
                shift_r += 128;                
                //printf("shift_r: "); print_byte(shift_r); putchar('\n');
                shift_r ^= 0b00001100;
            }
            
            //printf("shift_r: "); print_byte(shift_r); putchar('\n');
            //printf("------------\n");
        }
    }
    
    printf("computed crc: %x\n", shift_r);
    //print_scratchpad();
}

void read_input(char *input) {
	int c = 0;
	FILE *file = NULL;
	
	scratchpad = (byte*) malloc(10 * sizeof(byte));
	int i=0;
	for(i=0; i<8; i++) {
		scratchpad[i] = 0;
	}

	file = fopen(input, "r");
	if (file) {
		char* byte_chars = (char*) malloc(3* sizeof(char));

		//temp LSB
		read_byte_from_file(file, byte_chars);
//		printf("%s\n", byte_chars);
		sscanf(byte_chars, "%2hhx", &temp_lsb);
//		printf("lsb: %d\n", temp_lsb);
//		print_byte(temp_lsb);
//		putchar('\n');

		//temp MSB
		read_byte_from_file(file, byte_chars);
//		printf("%s\n", byte_chars);
		sscanf(byte_chars, "%2hhx", &temp_msb);
//		printf("msb: %d\n", temp_msb);
//		print_byte(temp_msb);
//		putchar('\n');

		read_byte_from_file(file, byte_chars);  //skip TL register byte
		read_byte_from_file(file, byte_chars);	//skip TH register byte
		
		//configuration register
		read_byte_from_file(file, byte_chars);
//		printf("input: %s\n", byte_chars);
		sscanf(byte_chars, "%2hhx", &configuration);
//		printf("precision default value: %d\n", precision);
		precision = get_precision(configuration);
//		putchar('\n');

		float temp = parse_temperature();
		printf("temperature: %3.4f\n", temp);

		read_byte_from_file(file, byte_chars); // skip byte 5
		read_byte_from_file(file, byte_chars); // skip byte 6
		read_byte_from_file(file, byte_chars); // skip byte 7

		//CRC
		read_byte_from_file(file, byte_chars);
		printf("loaded crc: %s\n", byte_chars);

		computeCRC();

		free(byte_chars);
		free(scratchpad);                
		fclose(file);
	} else {
		printf("%s%s%s\n", "file: '", input, "' could not be opened");
	}
}

void parse(char *input) {
	printf("%s %s\n", "parsing...", input);
	read_input(input);
}
