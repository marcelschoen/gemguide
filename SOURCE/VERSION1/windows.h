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

/* structure to hold data relevant to our window */
struct win_data {
	int handle; 	/* identifying handle of the window */

	char * text; 	/* text to display in window */
};

void start_program (void);
void open_vwork (void);

#endif
