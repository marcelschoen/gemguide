#include "windows.h"

/* some text to display in the windows */

char * poem1[] = {
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
	"                - William Blake",
	0
};

char * poem2[] = {
  "My heart aches, and a drowsy numbness pains",
  "My sense, as though of hemlock I had drunk,",
  "Or emptied some dull opiate to the drains",
  "One minute past, and Lethe-wards had sunk:",
  "'Tis not through envy of thy happy lot,",
  "But being too happy in thine happiness,",
  "That thou, light-winged Dryad of the trees",
  "In some melodious plot",
  "Of beechen green, and shadows numberless,",
  "Singest of summer in full-throated ease.",
  "                           - John Keats",
  0
};

char * poem3[] = {
  "I wandered lonely as a cloud",
  "That floats on high o'er vales and hills,",
  "When all at once I saw a crowd,",
  "A host, of golden daffodils;",
  "Beside the lake, beneath the trees,",
  "Fluttering and dancing in the breeze.",
  "              - William Wordsworth",
  0
};

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

struct win_data * get_win_data (struct win_data * wd, int handle);
void set_clip (bool flag, GRECT rec);
bool is_full_window (struct win_data * wd);
int slider_size (int num_available, int num_shown);
int slider_posn (int num_available, int num_shown, int offset);
void update_sliders (struct win_data * wd);

/* ------- The main functions to create and drive a GUI ------- */

/* open window and enter event loop */
void start_program (void) {
	struct win_data wd1;
	struct win_data wd2;
	struct win_data wd3;
	int dum, fullx, fully, fullw, fullh;

	graf_mouse (ARROW, 0L); /* ensure mouse is an arrow */
	wind_get (0, WF_WORKXYWH, &fullx, &fully, &fullw, &fullh);

	/* 1. set up and open our first window */
	wd1.handle = wind_create (NAME|CLOSER|FULLER|MOVER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE, fullx, fully, fullw, fullh);
	wind_set (wd1.handle, WF_NAME, "Example: Version 6 - Blake", 0, 0);
	wind_open (wd1.handle, fullx, fully, 300, 200);
	wd1.poem = poem1;
	wd1.next = NULL;

	wd1.horz_posn = 0;
	wd1.vert_posn = 0;
	vst_point (app_handle, 11, &dum, &dum, &wd1.cell_w, &wd1.cell_h);

	/* set up and open our second window */
	wd2.handle = wind_create (NAME|CLOSER|FULLER|MOVER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE, fullx, fully, fullw, fullh);
	wind_set (wd2.handle, WF_NAME, "Example: Version 6 - Keats", 0, 0);
	wind_open (wd2.handle, fullx, fully+30, 300, 200);
	wd2.poem = poem2;
	wd2.next = NULL;

	wd2.horz_posn = 0;
	wd2.vert_posn = 0;
	vst_point (app_handle, 11, &dum, &dum, &wd2.cell_w, &wd2.cell_h);

	/* add second window to end of list */
	wd1.next = &wd2;

	/* set up and open our third window */
	wd3.handle = wind_create (NAME|CLOSER|FULLER|MOVER|SIZER|UPARROW|DNARROW|VSLIDE|LFARROW|RTARROW|HSLIDE, fullx, fully, fullw, fullh);
	wind_set (wd3.handle, WF_NAME, "Example: Version 6 - Wordsworth", 0, 0);
	wind_open (wd3.handle, fullx, fully+60, 300, 200);
	wd3.poem = poem3;
	wd3.next = NULL;

	wd3.horz_posn = 0;
	wd3.vert_posn = 0;
	vst_point (app_handle, 11, &dum, &dum, &wd3.cell_w, &wd3.cell_h);

	/* add second window to end of list */
	wd1.next->next = &wd3;

	/* 2. process events for our window */
	event_loop (&wd1);

	/* 3. close and remove our windows */
	wind_close (wd1.handle);
	wind_delete (wd1.handle);
	wind_close (wd2.handle);
	wind_delete (wd2.handle);
	wind_close (wd3.handle);
	wind_delete (wd3.handle);
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
				do_fulled (get_win_data(wd, msg_buf[3]));
				break;

			case WM_SIZED:
				do_sized (get_win_data(wd, msg_buf[3]), msg_buf);
				break;

			case WM_REDRAW:
				do_redraw (get_win_data(wd, msg_buf[3]), (GRECT *)&msg_buf[4]);
				break;

			case WM_ARROWED:
				wind_set (msg_buf[3], WF_TOP, 0, 0); /* bring to the top */
				do_arrow (get_win_data(wd, msg_buf[3]), msg_buf[4]);
				break;

			case WM_VSLID:
				wind_set (msg_buf[3], WF_TOP, 0, 0);
				do_vslide (get_win_data(wd, msg_buf[3]), msg_buf[4]);
				break;

			case WM_HSLID:
				wind_set (msg_buf[3], WF_TOP, 0, 0);
				do_hslide (get_win_data(wd, msg_buf[3]), msg_buf[4]);
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
	bool changed;

	if (msg_buf[6] < MIN_WIDTH) msg_buf[6] = MIN_WIDTH;
	if (msg_buf[7] < MIN_HEIGHT) msg_buf[7] = MIN_HEIGHT;

	new_height = (msg_buf[7] / wd->cell_h) + 1; /* find new height in characters */
	new_width = (msg_buf[6] / wd->cell_w) + 1;  /* find new width in characters */

	/* if new height is bigger than lines_shown - vert_posn,
	   we can decrease vert_posn to show more lines */
	if (new_height > wd->lines_shown - wd->vert_posn) {
		wd->vert_posn -= new_height - (wd->lines_shown - wd->vert_posn);
		if (wd->vert_posn < 0) wd->vert_posn = 0;
		changed = true;
	}
	/* if new height is less than lines_shown - vert_posn,
	   we leave vertical position in same place,
	   so nothing has to be done  */

	/* similarly, if new width is bigger than colns_shown - horz_posn,
	   we can decrease horz_posn to show more columns */
	if (new_width > wd->colns_shown - wd->horz_posn) {
		wd->horz_posn -= new_width - (wd->colns_shown - wd->horz_posn);
		if (wd->horz_posn < 0) wd->horz_posn = 0;
		changed = true;
	}

	wind_set (wd->handle, WF_CURRXYWH, msg_buf[4], msg_buf[5], msg_buf[6], msg_buf[7]);

	if (changed) {
		GRECT rec;

		wind_get (wd->handle, WF_WORKXYWH, &rec.g_x, &rec.g_y, &rec.g_w, &rec.g_h);
		do_redraw (wd, &rec);
	}
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

/* Find window data for given handle in the list */
struct win_data * get_win_data (struct win_data * wd, int handle) {
	while (wd != NULL) {
		if (wd->handle == handle) break;
		wd = wd->next;
	}
	return wd;
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
