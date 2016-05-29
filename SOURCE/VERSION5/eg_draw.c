#include "eg_draw.h"

void draw_example (int app_handle, struct win_data * wd, int x, int y, int w, int h) {
	int i = 0;
	int dum, cell_h;

	vst_point (app_handle, 11, &dum, &dum, &dum, &cell_h);

	while (wd->poem[i] != 0) {
		v_gtext (app_handle, x+10, y+60+i*cell_h, wd->poem[i]);
		i = i + 1;
	}

}