#include "windows.h"

void event_loop (struct win_data * wd);

void do_arrow (struct win_data * wd,int arrow);
void do_fulled (struct win_data * wd);
void do_redraw (struct win_data * wd, GRECT * rec1);
void do_sized (struct win_data * wd, int * msg_buf);
void do_vslide (struct win_data * wd, int posn);
void do_hslide (struct win_data * wd, int posn);

void do_uppage (struct win_data * wd);
void do_dnpage (struct win_data * wd);
void do_upline (struct win_data * wd);
void do_dnline (struct win_data * wd);
void do_lfpage (struct win_data * wd);
void do_rtpage (struct win_data * wd);
void do_lfline (struct win_data * wd);
void do_rtline (struct win_data * wd);

void draw_interior (struct win_data * wd, GRECT clip);

void set_clip (bool flag, GRECT rec);
bool is_full_window (struct win_data * wd);
int slider_size (int num_available, int num_shown);
int slider_posn (int num_available, int num_shown, int offset);
void update_sliders (struct win_data * wd);

/* some text to display in the window */
char * poem[] = {
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	"outside",
	"gardening",
	0
};

/* ------- The main functions to create and drive a GUI ------- */

/* open window and enter event loop */
void start_program (void) {
	struct win_data wd;
	int fullx, fully, fullw, fullh;
	int dum;

	graf_mouse (ARROW, 0L); /* ensure mouse is an arrow */

	/* 1. set up and open our window */
	wind_get (0, WF_WORKXYWH, &fullx, &fully, &fullw, &fullh);
	wd.handle = wind_create (NAME|CLOSER|FULLER|MOVER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE, fullx, fully, fullw, fullh);
	wind_set (wd.handle, WF_NAME, "Example: Version 5", 0, 0);
	wind_open (wd.handle, fullx, fully, 300, 200);

	/* set up application-specific data */
	wd.poem = poem;

	/* initialise position in display */
	wd.horz_posn = 0;
	wd.vert_posn = 0;

	/* initialise text size */
	vst_point (app_handle, 11, &dum, &dum, &wd.cell_w, &wd.cell_h);

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

/* Process all events for our program */
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

			case WM_ARROWED:
				wind_set (msg_buf[3], WF_TOP, 0, 0); /* bring to the top */
				do_arrow (wd, msg_buf[4]);
				break;

			case WM_VSLID:
				wind_set (msg_buf[3], WF_TOP, 0, 0);
				do_vslide (wd, msg_buf[4]);
				break;

			case WM_HSLID:
				wind_set (msg_buf[3], WF_TOP, 0, 0);
				do_hslide (wd, msg_buf[4]);
				break;

		}
	} while (msg_buf[0] != WM_CLOSED);

}

/* ---------- The following functions respond to events ---------- */

/* Respond to an arrow event by determining the type of arrow event, and
   calling appropriate function.
 */
void do_arrow (struct win_data * wd, int arrow) {
	switch (arrow) {
		case WA_UPPAGE:
			do_uppage (wd);
			break;

		case WA_DNPAGE:
			do_dnpage (wd);
			break;

		case WA_UPLINE:
			do_upline (wd);
			break;

		case WA_DNLINE:
			do_dnline (wd);
			break;

		case WA_LFPAGE:
			do_lfpage (wd);
			break;

		case WA_RTPAGE:
			do_rtpage (wd);
			break;

		case WA_LFLINE:
			do_lfline (wd);
			break;

		case WA_RTLINE:
			do_rtline (wd);
			break;
	}
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
	int new_height, new_width;

	if (msg_buf[6] < MIN_WIDTH) msg_buf[6] = MIN_WIDTH;
	if (msg_buf[7] < MIN_HEIGHT) msg_buf[7] = MIN_HEIGHT;

	new_height = (msg_buf[7] / wd->cell_h) + 1; /* find new height in characters */
	new_width = (msg_buf[6] / wd->cell_w) + 1;  /* find new width in characters */

	/* if new height is bigger than lines_shown - vert_posn,
	   we can decrease vert_posn to show more lines */
	if (new_height > wd->lines_shown - wd->vert_posn) {
		wd->vert_posn -= new_height - (wd->lines_shown - wd->vert_posn);
		if (wd->vert_posn < 0) wd->vert_posn = 0;
	}
	/* if new height is less than lines_shown - vert_posn,
	   we leave vertical position in same place,
	   so nothing has to be done  */

	/* similarly, if new width is bigger than colns_shown - horz_posn,
	   we can decrease horz_posn to show more columns */
	if (new_width > wd->colns_shown - wd->horz_posn) {
		wd->horz_posn -= new_width - (wd->colns_shown - wd->horz_posn);
		if (wd->horz_posn < 0) wd->horz_posn = 0;
	}

	wind_set (wd->handle, WF_CURRXYWH, msg_buf[4], msg_buf[5], msg_buf[6], msg_buf[7]);
}

/* Called when vertical slider moved. */
void do_vslide (struct win_data * wd, int posn) {
	GRECT r;
	int lines_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	lines_avail = r.g_h / wd->cell_h;
	wd->vert_posn = (posn * (long)(wd->lines_shown - lines_avail)) / 1000;
	if (wd->vert_posn < 0) wd->vert_posn = 0;
	wind_set (wd->handle, WF_VSLIDE, posn, 0, 0, 0);
	do_redraw (wd, &r);
}

/* Called when horizontal slider moved. */
void do_hslide (struct win_data * wd, int posn) {
	GRECT r;
	int colns_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	colns_avail = r.g_w / wd->cell_w;
	wd->horz_posn = (posn * (long)(wd->colns_shown - colns_avail)) / 1000;
	if (wd->horz_posn < 0) wd->horz_posn = 0;
	wind_set (wd->handle, WF_HSLIDE, posn, 0, 0, 0);
	do_redraw (wd, &r);
}

/* ------- The following functions are for arrow events ------- */

/* Called to move vertical slider up by a page */
void do_uppage (struct win_data * wd) {
	GRECT r;
	int lines_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	lines_avail = r.g_h / wd->cell_h;
	wd->vert_posn -= lines_avail;
	if (wd->vert_posn < 0) wd->vert_posn = 0;
	do_redraw (wd, &r);
}

/* Called to move vertical slider down by a page */
void do_dnpage (struct win_data * wd) {
	GRECT r;
	int lines_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	lines_avail = r.g_h / wd->cell_h;
	wd->vert_posn += lines_avail;
	if (wd->lines_shown > lines_avail && wd->vert_posn > wd->lines_shown - lines_avail) {
		wd->vert_posn = wd->lines_shown - lines_avail;
	} else if (lines_avail >= wd->lines_shown) {
		wd->vert_posn = 0;
	}
	do_redraw (wd, &r);
}

/* Called to move vertical slider up by a single row */
void do_upline (struct win_data * wd) {
	FDB s, d;
	GRECT r;
	int pxy[8];

	if (wd->vert_posn == 0) return; /* already at top of screen */
	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	wd->vert_posn -= 1;
	if (wd->vert_posn < 0) wd->vert_posn = 0;

	set_clip (true, r);
	graf_mouse (M_OFF, 0L);
	s.fd_addr = 0L;
	d.fd_addr = 0L;
	pxy[0] = r.g_x;
	pxy[1] = r.g_y + 1;
	pxy[2] = r.g_x + r.g_w;
	pxy[3] = r.g_y + r.g_h - wd->cell_h - 1;
	pxy[4] = r.g_x;
	pxy[5] = r.g_y + wd->cell_h + 1;
	pxy[6] = r.g_x + r.g_w;
	pxy[7] = r.g_y + r.g_h - 1;
	vro_cpyfm (wd->handle, S_ONLY, pxy, &s, &d);

	graf_mouse (M_ON, 0L);
	set_clip (false, r);
	r.g_h = 2*wd->cell_h; /* draw the height of two rows at top */
	do_redraw (wd, &r);
}

/* Called to move vertical slider down by a single row */
void do_dnline (struct win_data * wd) {
	FDB s, d;
	GRECT r;
	int pxy[8];
	int lines_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	lines_avail = r.g_h / wd->cell_h;

	if (wd->vert_posn >= (wd->lines_shown - lines_avail)) return; /* at bottom */

	wd->vert_posn += 1;
	if (wd->lines_shown > lines_avail && wd->vert_posn > wd->lines_shown - lines_avail) {
		wd->vert_posn = wd->lines_shown - lines_avail;
	} else if (lines_avail >= wd->lines_shown) {
		/* don't move if all lines are showing */
		wd->vert_posn = 0;
	}

	set_clip (true, r);
	graf_mouse (M_OFF, 0L);
	s.fd_addr = 0L;
	d.fd_addr = 0L;
	pxy[0] = r.g_x;
	pxy[1] = r.g_y + wd->cell_h + 1;
	pxy[2] = r.g_x + r.g_w;
	pxy[3] = r.g_y + r.g_h - 1;
	pxy[4] = r.g_x;
	pxy[5] = r.g_y + 1;
	pxy[6] = r.g_x + r.g_w;
	pxy[7] = r.g_y + r.g_h - wd->cell_h - 1;
	vro_cpyfm (wd->handle, S_ONLY, pxy, &s, &d);

	graf_mouse (M_ON, 0L);
	set_clip (false, r);
	r.g_y = r.g_y + r.g_h - 2*wd->cell_h;
	r.g_h = 2*wd->cell_h; /* draw the height of two rows at bottom */
	do_redraw (wd, &r);
}

/* Called to move horizontal slider left by a page */
void do_lfpage (struct win_data * wd) {
	GRECT r;
	int colns_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	colns_avail = r.g_w / wd->cell_w;
	wd->horz_posn -= colns_avail;
	if (wd->horz_posn < 0) wd->horz_posn = 0;
	do_redraw (wd, &r);
}

/* Called to move horizontal slider right by a page */
void do_rtpage (struct win_data * wd) {
	GRECT r;
	int colns_avail;

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	colns_avail = r.g_w / wd->cell_w;
	wd->horz_posn += colns_avail;
	if (wd->colns_shown > colns_avail && wd->horz_posn > wd->colns_shown - colns_avail) {
		wd->horz_posn = wd->colns_shown - colns_avail;
	} else if (colns_avail >= wd->colns_shown) {
		wd->horz_posn = 0;
	}
	do_redraw (wd, &r);
}

/* Called to move horizontal slider left by a single column */
void do_lfline (struct win_data * wd) {
	FDB s, d;
	GRECT r;
	int pxy[8];

	if (wd->horz_posn == 0) return; /* already on left */

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	wd->horz_posn -= 1;
	if (wd->horz_posn < 0) wd->horz_posn = 0;

	set_clip (true, r);
	graf_mouse (M_OFF, 0L);
	s.fd_addr = 0L;
	d.fd_addr = 0L;
	pxy[0] = r.g_x;
	pxy[1] = r.g_y + 1;
	pxy[2] = r.g_x + r.g_w - wd->cell_w;
	pxy[3] = r.g_y + r.g_h - 1;
	pxy[4] = r.g_x + wd->cell_w;
	pxy[5] = r.g_y + 1;
	pxy[6] = r.g_x + r.g_w;
	pxy[7] = r.g_y + r.g_h - 1;
	vro_cpyfm (wd->handle, S_ONLY, pxy, &s, &d);

	graf_mouse (M_ON, 0L);
	set_clip (false, r);
	r.g_w = 2*wd->cell_w; /* draw the width of two chars at left */

	do_redraw (wd, &r);
}

/* Called to move horizontal slider right by a single column */
void do_rtline (struct win_data * wd) {
	FDB s, d;
	GRECT r;
	int colns_avail;
	int pxy[8];

	wind_get (wd->handle, WF_WORKXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	colns_avail = r.g_w / wd->cell_w;

	if (wd->horz_posn == (wd->colns_shown - colns_avail)) return; /* already on right */

	wd->horz_posn += 1;
	if (wd->colns_shown > colns_avail && wd->horz_posn > wd->colns_shown - colns_avail) {
		wd->horz_posn = wd->colns_shown - colns_avail;
	} else if (colns_avail >= wd->colns_shown) {
		wd->horz_posn = 0;
	}

	set_clip (true, r);
	graf_mouse (M_OFF, 0L);
	s.fd_addr = 0L;
	d.fd_addr = 0L;
	pxy[0] = r.g_x + wd->cell_w;
	pxy[1] = r.g_y + 1;
	pxy[2] = r.g_x + r.g_w;
	pxy[3] = r.g_y + r.g_h - 1;
	pxy[4] = r.g_x;
	pxy[5] = r.g_y + 1;
	pxy[6] = r.g_x + r.g_w - wd->cell_w;
	pxy[7] = r.g_y + r.g_h - 1;
	vro_cpyfm (wd->handle, S_ONLY, pxy, &s, &d);

	graf_mouse (M_ON, 0L);
	set_clip (false, r);
	r.g_x = r.g_x + r.g_w - 2*wd->cell_w;
	r.g_w = 2*wd->cell_w; /* draw the width of two chars at right */

	do_redraw (wd, &r);
}

/* ---------- The actual drawing code ---------- */

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
	update_sliders (wd);
	graf_mouse (M_ON, 0L);
}

/* ------- The following functions are for calculations ------- */

/* sets/unsets clipping rectangle in VDI */
void set_clip (bool flag, GRECT rec) {
	int pxy[4];

	pxy[0] = rec.g_x;
	pxy[1] = rec.g_y;
	pxy[2] = rec.g_x + rec.g_w - 1;
	pxy[3] = rec.g_y + rec.g_h - 1;

	vs_clip (app_handle, flag, pxy);
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

/* Computes size of slider, between 0 and 1000, based on the number
   of lines or characters available in the display, and the number
   actually to be shown.
 */
int slider_size (int num_available, int num_shown) {
	int result;

	/* in case number shown is smaller than those available */
	if (num_available >= num_shown) { /* all visible */
		result = 1000; /* so slider complete */
	} else {
		result = (1000 * (long)num_available) / num_shown;
	}

	return result;
}

/* Computes position of slider, between 0 and 1000, based on the number
   of lines or characters available in the display, the number
   actually to be shown, and the current offset.
 */
 int slider_posn (int num_available, int num_shown, int offset) {
	int result;
	int scrollable_region, tmp1, tmp2;

	/* in case number shown is smaller than those available */
	if (num_available >= num_shown) { /* all visible */
		result = 0; /* so slider complete, and show bar at top position */
	} else {
		/* number of positions scrollbar can move across: must be positive due to above check */
		scrollable_region = num_shown - num_available;
		tmp1 = offset / scrollable_region;
		tmp2 = offset % scrollable_region;

		result = (1000 * (long)tmp1) + ((1000 * (long)tmp2) / scrollable_region);
	}

	return result;
}

/* Update the position and size of both sliders */
void update_sliders (struct win_data * wd) {
	int lines_avail, cols_avail;
	int wrkx, wrky, wrkw, wrkh;

	wind_get (wd->handle, WF_WORKXYWH, &wrkx, &wrky, &wrkw, &wrkh);
	lines_avail = wrkh / wd->cell_h;
	cols_avail = wrkw / wd->cell_w;

 	/* handle vertical slider */
	wind_set (wd->handle, WF_VSLSIZE, slider_size (lines_avail, wd->lines_shown), 0, 0, 0);
	wind_set (wd->handle, WF_VSLIDE, slider_posn (lines_avail, wd->lines_shown, wd->vert_posn), 0, 0, 0);

	/* handle horizontal slider */
	wind_set (wd->handle, WF_HSLSIZE, slider_size (cols_avail, wd->colns_shown), 0, 0, 0);
	wind_set (wd->handle, WF_HSLIDE, slider_posn (cols_avail, wd->colns_shown, wd->horz_posn), 0, 0, 0);
}