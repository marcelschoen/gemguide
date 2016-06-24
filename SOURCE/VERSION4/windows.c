#include "windows.h"

void event_loop (struct win_data * wd);

void do_fulled (struct win_data * wd);
void do_redraw (struct win_data * wd, GRECT * rec1);
void do_sized (struct win_data * wd, int * msg_buf);

void set_clip (bool flag, GRECT rec);
void draw_interior (struct win_data * wd, GRECT clip);

bool is_full_window (struct win_data * wd);

/* some text to display in the window */
char * poem[] = {
	"    The Tiger",
	"",
	"Tiger! Tiger! burning bright",
	"In the forests of the night,",
	"What immortal hand or eye",
	"Could frame thy fearful symmetry?",
	"",
	"In what distant deeps or skies",
	"Burnt the fire of thine eyes?",
	"On what wings dare he aspire?",
	"What the hand dare seize the fire?",
	"",
	"And what shoulder, and what art,",
	"Could twist the sinews of thy heart?",
	"And when thy heart began to beat,",
	"What dread hand? and what dread feet?",
	"",
	"What the hammer? what the chain?",
	"In what furnace was they brain?",
	"What the anvil? what dread grasp",
	"Dare its deadly terrors clasp?",
	"",
	"When the stars threw down their spears,",
	"And water'd heaven with their tears,",
	"Did he smile his work to see?",
	"Did he who made the Lamb make thee?",
	"",
	"Tiger! Tiger! burning bright",
	"In the forests of the night,",
	"What immortal hand or eye",
	"Dare frame thy fearful symmetry?",
	"",
	0
};

/* open window and enter event loop */
void start_program (void) {
	struct win_data wd;
	int fullx, fully, fullw, fullh;

	graf_mouse (ARROW, 0L); /* ensure mouse is an arrow */

	/* 1. set up and open our window */
	wind_get (0, WF_WORKXYWH, &fullx, &fully, &fullw, &fullh);
	wd.handle = wind_create (NAME|CLOSER|FULLER|MOVER|SIZER, fullx, fully, fullw, fullh);
	wind_set (wd.handle, WF_NAME, "Example: Version 4", 0, 0);
	wind_open (wd.handle, fullx, fully, 300, 200);

	/* set up application-specific data */
	wd.poem = poem;

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
	work_in[0] = 2 + Getrez ();
	for (i = 1; i < 10; work_in[i++] = 1);
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

			case WM_FULLED:
				do_fulled (wd);
				break;

			case WM_SIZED:
				do_sized (wd, msg_buf);
				break;

			case WM_REDRAW:
				do_redraw (wd, (GRECT *)&msg_buf[4]);
				break;
		}
	} while (msg_buf[0] != WM_CLOSED);

}

/* called when the full-box is clicked.
   Window will be made full or return to its previous size, depending
   on current state.
 */
void do_fulled (struct win_data * wd) {
	if (is_full_window (wd)) { /* it's full, so shrink to previous size */
		int oldx, oldy, oldw, oldh;
		int fullx, fully, fullw, fullh;

		wind_get (wd->handle, WF_PREVXYWH, &oldx, &oldy, &oldw, &oldh);
		wind_get (wd->handle, WF_FULLXYWH, &fullx, &fully, &fullw, &fullh);
		graf_shrinkbox (oldx, oldy, oldw, oldh, fullx, fully, fullw, fullh);
		wind_set (wd->handle, WF_CURRXYWH, oldx, oldy, oldw, oldh);

	} else { /* make full size */
		int curx, cury, curw, curh;
		int fullx, fully, fullw, fullh;

		wind_get (wd->handle, WF_CURRXYWH, &curx, &cury, &curw, &curh);
		wind_get (wd->handle, WF_FULLXYWH, &fullx, &fully, &fullw, &fullh); // <2>
		graf_growbox (curx, cury, curw, curh, fullx, fully, fullw, fullh);
		wind_set (wd->handle, WF_CURRXYWH, fullx, fully, fullw, fullh);
	}
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

/* called when the window is resized.  It resizes the current window
   to the new dimensions, but stops the new dimensions going beyond
   the minimum allowed height and width.
   */
void do_sized (struct win_data * wd, int * msg_buf) {
	if (msg_buf[6] < MIN_WIDTH) msg_buf[6] = MIN_WIDTH;
	if (msg_buf[7] < MIN_HEIGHT) msg_buf[7] = MIN_HEIGHT;

	wind_set (wd->handle, WF_CURRXYWH, msg_buf[4], msg_buf[5], msg_buf[6], msg_buf[7]);
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

/* calculates if window is currently at maximum size */
bool is_full_window (struct win_data * wd) {
	int curx, cury, curw, curh;
	int fullx, fully, fullw, fullh;

	wind_get (wd->handle, WF_CURRXYWH, &curx, &cury, &curw, &curh);
	wind_get (wd->handle, WF_FULLXYWH, &fullx, &fully, &fullw, &fullh);
	if (curx != fullx || cury != fully || curw != fullw || curh != fullh) {
		return false;
	} else {
		return true;
	}
}
