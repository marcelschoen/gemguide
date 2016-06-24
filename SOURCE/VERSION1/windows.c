#include "windows.h"

void event_loop (struct win_data * wd);

/* open window and enter event loop */
void start_program (void) {
	struct win_data wd;
	int fullx, fully, fullw, fullh;

	graf_mouse (ARROW, 0L); /* ensure mouse is an arrow */

	/* 1. set up and open our window */
	wind_get (0, WF_WORKXYWH, &fullx, &fully, &fullw, &fullh);
	wd.handle = wind_create (NAME|CLOSER, fullx, fully, fullw, fullh);
	wind_set (wd.handle, WF_NAME, "Example: Version 1", 0, 0);
	wind_open (wd.handle, fullx, fully, 300, 200);

	/* create any application-specific data: the string to show, in this case */
	wd.text = "Hello";

	/* display content in window */
	draw_example (app_handle, &wd);

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
	} while (msg_buf[0] != WM_CLOSED);

}
