#ifndef __BIRD_H
#define __BIRD_H 	  

/* Color Reference Table 
Color Index : RGB565 : color , usage
0x00        : draw nothing , show background
0x01        : 0x0000 : black ,outline of the bird 
0x02        : 0xFFFF : white , wing and eye of the bird
0x03        ��0xFF20 : light yellow, body of the bird
0x04        : 0xED61 : dark yellow, bottom part of the bird body
0x05        : 0xFB40 : orange, mouth of the bird 
  */
	
/* The two dimensional u8 array will specify the color index (defined by ourselves as shown in the previous table)
   at each position to draw the bird.*/
extern const u8 bird_basic[12][17];/* = {
	 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
	 {0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x04,0x04,0x04,0x04,0x01,0x01,0x01,0x01,0x01,0x00},
	 {0x00,0x00,0x00,0x00,0x01,0x04,0x04,0x04,0x04,0x04,0x01,0x05,0x05,0x05,0x05,0x01,0x00},
	 {0x00,0x00,0x01,0x01,0x01,0x04,0x04,0x04,0x04,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x00},
	 {0x00,0x01,0x03,0x03,0x03,0x01,0x03,0x03,0x03,0x03,0x01,0x05,0x05,0x05,0x05,0x05,0x01},
	 {0x01,0x03,0x02,0x02,0x02,0x03,0x01,0x03,0x03,0x03,0x03,0x01,0x01,0x01,0x01,0x01,0x00},
	 {0x01,0x02,0x02,0x02,0x02,0x02,0x01,0x03,0x03,0x03,0x01,0x02,0x02,0x02,0x01,0x00,0x00},
	 {0x01,0x02,0x02,0x02,0x02,0x01,0x03,0x03,0x03,0x01,0x02,0x02,0x01,0x02,0x01,0x00,0x00},
	 {0x00,0x01,0x01,0x01,0x01,0x03,0x03,0x03,0x03,0x01,0x02,0x02,0x01,0x02,0x01,0x00,0x00},
	 {0x00,0x00,0x00,0x01,0x03,0x03,0x03,0x03,0x03,0x01,0x02,0x02,0x02,0x01,0x00,0x00,0x00},
   {0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x03,0x01,0x02,0x01,0x00,0x00,0x00,0x00},
	 {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00}
};*/

void draw_bird_dot(int start_x, int start_y,u8 mode);
void draw_bird_rectangle(int start_x, int start_y,u16 length,u8 mode);
#endif

