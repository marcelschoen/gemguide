#include "windows.h"

void event_loop (struct win_data * wd);

void do_redraw (struct win_data * wd, GRECT * rec1);

void set_clip (bool flag, GRECT rec);
void draw_interior (struct win_data * wd, GRECT clip);

/* open window and enter event loop */
void start_program (void) {
	struct win_data wd;
	int fullx, fully, fullw, fullh;

	graf_mouse (ARROW, 0L); /* ensure mouse is an arrow */

	/* 1. set up and open our window */
	wind_get (0, WF_WORKXYWH, &fullx, &fully, &fullw, &fullh);
	wd.handle = wind_create (NAME|CLOSER|MOVER, fullx, fully, fullw, fullh);
	wind_set (wd.handle, WF_NAME, "Example: Version 3", 0, 0);
	wind_open (wd.handle, fullx, fully, 300, 200);

	/* create any application-specific data: the string to show, in this case */
	wd.text = "Hello";

	/* 2. process events for our window */
	event_loop (&wd);

	/* 3. close and remove our window */
	wind_close (wd.handle);
	wind_delete (wd.handle);
}

/* Standard code to set up GEM arrays and open work area.
 * In particular, this gets app_handle, which is needed to use the screen.
*/
void open_vwork (void) {
	int i;
	int dum;

	app_handle = graf_handle (&dum, &dum, &dum, &dum);
	for (i = 0; i < 10; work_in[i++] = 1);
	work_in[10] = 2;
	v_opnvwk (work_in, &app_handle, work_out);
}

void event_loop (struct win_data * wd) {
	int msg_buf[8];

	do {
		evnt_mesag (msg_buf);

		switch (msg_buf[0]) {

			case WM_TOPPED:
				wind_set (msg_buf[3], WF_TOP, 0, 0);
				break;

			case WM_MOVED:
				wind_set (msg_buf[3], WF_CURRXYWH, msg_buf[4],
				msg_buf[5], msg_buf[6], msg_buf[7]);
				break;

			case WM_REDRAW:
				do_redraw (wd, (GRECT *)&msg_buf[4]);
				break;
		}
	} while (msg_buf[0] != WM_CLOSED);

}

/* Called when application asked to redraw parts of its display.
   Walks the rectangle list, redrawing the relevant part of the window.
 */
void do_redraw (struct win_data * wd, GRECT * rec1) {
	GRECT rec2;

	wind_update (BEG_UPDATE);

	wind_get (wd->handle, WF_FIRSTXYWH, &rec2.g_x, &rec2.g_y, &rec2.g_w, &rec2.g_h);
	while (rec2.g_w && rec2.g_h) {
		if (rc_intersect (rec1, &rec2)) {
			draw_interior (wd, rec2);
		}
		wind_get (wd->handle, WF_NEXTXYWH, &rec2.g_x, &rec2.g_y, &rec2.g_w, &rec2.g_h);
	}

	wind_update (END_UPDATE);
}

/* sets/unsets clipping rectangle in VDI */
void set_clip (bool flag, GRECT rec) {
	int pxy[4];

	pxy[0] = rec.g_x;
	pxy[1] = rec.g_y;
	pxy[2] = rec.g_x + rec.g_w - 1;
	pxy[3] = rec.g_y + rec.g_h - 1;

	vs_clip (app_handle, flag, pxy);
}

/* Draw interior of window, within given clipping rectangle */
void draw_interior (struct win_data * wd, GRECT clip) {
	int pxy[4];
	int wrkx, wrky, wrkw, wrkh; /* some variables describing current working area */

	/* set up drawing, by hiding mouse and setting clipping on */
	graf_mouse (M_OFF, 0L);
	set_clip (true, clip);
	wind_get (wd->handle, WF_WORKXYWH, &wrkx, &wrky, &wrkw, &wrkh);

	/* clears the display */
	vsf_color (app_handle, WHITE);
	pxy[0] = wrkx;
	pxy[1] = wrky;
	pxy[2] = wrkx + wrkw - 1;
	pxy[3] = wrky + wrkh - 1;
	vr_recfl (app_handle, pxy);

	/* draws our specific code */
	draw_example (app_handle, wd, wrkx, wrky, wrkw, wrkh);

	/* tidies up */
	set_clip (false, clip);
	graf_mouse (M_ON, 0L);
}