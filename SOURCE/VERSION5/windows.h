#if !defined(WINDOWS_H)
#define WINDOWS_H

/* include some standard GEM and C headers */
#include <aes.h>
#include <gemf.h>
#include <vdi.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "eg_draw.h"

/* Define some space for GEM arrays */
int work_in[11],
	work_out[57],
	contrl[12],
	intin[128],
	ptsin[128],
	intout[128],
	ptsout[128];

static int app_handle; /* application graphics handle */

#define MIN_WIDTH 60
#define MIN_HEIGHT 100

/* structure to hold data relevant to our window */
struct win_data {
	int handle; 	/* identifying handle of the window */

	char ** poem; /* poem to display in window */

	int cell_h;
	int cell_w;
	int vert_posn;
	int horz_posn;
	int lines_shown;
	int colns_shown;
};

void start_program (void);
void open_vwork (void);

#endif
