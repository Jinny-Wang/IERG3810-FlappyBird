#ifndef __IERG3810_DRAWCOLUMN_H
#define __IERG3810_DRAWCOLUMN_H
#include "stm32f10x.h"

// put procedure header here
void draw_bottom_column(int x, u16 color, int bottom_level);
void draw_top_column(int x, u16 color, int top_level);
void draw_column(int x, u16 color, int bottom_level, int top_level);
void clear_column(int x, int bottom_level, int top_level);
int level_to_y(int level , int top_or_bottom); //transform level to "y- coordinate value" 0: bottom , 1:top
void clear_column_part(int old_column_x, int column_move_speed, int bottom_level, int top_level);
#endif
