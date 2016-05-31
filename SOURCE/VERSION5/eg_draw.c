#include "eg_draw.h"

void draw_example (int app_handle, struct win_data * wd, int x, int y, int w, int h) {
	int i = 0;
	int lines_to_ignore = wd->vert_posn;
	int cur_y = y + wd->cell_h;

	wd->lines_shown = 0;
	wd->colns_shown = 0;

	while (wd->poem[i] != 0) {
		if (lines_to_ignore == 0) {
			v_gtext (app_handle, x+wd->cell_w*(1-wd->horz_posn), cur_y, wd->poem[i]);

			if (strlen(wd->poem[i])+2 > wd->colns_shown) {
				wd->colns_shown = strlen (wd->poem[i]) + 2;
			}

			cur_y += wd->cell_h;
		} else {
			lines_to_ignore -= 1;
		}

		wd->lines_shown += 1;

		i = i + 1;
	}

}