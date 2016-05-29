#include "eg_draw.h"

void draw_example (int app_handle, struct win_data * wd, int x, int y, int w, int h) {

	v_gtext (app_handle, x+10, y+60, wd->text);

}