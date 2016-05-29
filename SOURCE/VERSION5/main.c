/* start point for our program */

#include "windows.h"

void main (int argc, char ** argv) {
	appl_init ();
	open_vwork ();
	start_program ();
	rsrc_free ();
	v_clsvwk (app_handle);
	appl_exit ();
}