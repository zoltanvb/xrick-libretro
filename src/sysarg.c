/*
 * xrick/src/sysarg.c
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net). All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

/*
 * 20021010 added test to prevent buffer overrun in -keys parsing.
 */

#include <stdlib.h>  /* atoi */
#include <string.h>  /* strcasecmp */

#include "system.h"
#include "config.h"
#include "game.h"

#include "maps.h"
#include "syssnd.h"

/* handle Microsoft Visual C (must come after system.h!) */
#ifdef __MSVC__
#define strcasecmp stricmp
#endif

int sysarg_args_period = 0;
int sysarg_args_map = 0;
int sysarg_args_submap = 0;
int sysarg_args_fullscreen = 0;
int sysarg_args_zoom = 0;
int sysarg_args_nosound = 0;
int sysarg_args_vol = 0;
char *sysarg_args_data = NULL;

/*
 * Fail
 */
void
sysarg_fail(char *msg)
{
#ifdef ENABLE_SOUND
	printf("xrick [version #%s]: %s\nusage: xrick [<options>]\n<option> =\n  -h, -help : Display this information. -fullscreen : Run in fullscreen mode.\n    The default is to run in a window.\n  -speed <speed> : Run at speed <speed>. Speed must be an integer between 1\n    (fast) and 100 (slow). The default is %d\n  -zoom <zoom> : Display with zoom factor <zoom>. <zoom> must be an integer\n   between 1 (320x200) and %d (%d times bigger). The default is 2.\n  -map <map> : Start at map number <map>. <map> must be an integer between\n    1 and %d. The default is to start at map number 1\n  -submap <submap> : Start at submap <submap>. <submap> must be an integer\n    between 1 and %d. The default is to start at submap number 1 or, if a map\n    was specified, at the first submap of that map.\n  -keys <left>-<right>-<up>-<down>-<fire> : Override the default key\n    bindings (cf. KeyCodes)\n  -nosound : Disable sounds. The default is to play with sounds enabled.\n  -vol <vol> : Play sounds at volume <vol>. <vol> must be an integer\n    between 0 (silence) and %d (max). The default is to play sounds\n    at maximal volume (%d).\n", VERSION, msg, GAME_PERIOD, SYSVID_MAXZOOM, SYSVID_MAXZOOM, MAP_NBR_MAPS-1, MAP_NBR_SUBMAPS, SYSSND_MAXVOL, SYSSND_MAXVOL);
#else
	printf("xrick [version #%s]: %s\nusage: xrick [<options>]\n<option> =\n  -h, -help : Display this information. -fullscreen : Run in fullscreen mode.\n    The default is to run in a window.\n  -speed <speed> : Run at speed <speed>. Speed must be an integer between 1\n    (fast) and 100 (slow). The default is %d\n  -zoom <zoom> : Display with zoom factor <zoom>. <zoom> must be an integer\n   between 1 (320x200) and %d (%d times bigger). The default is 2.\n  -map <map> : Start at map number <map>. <map> must be an integer between\n    1 and %d. The default is to start at map number 1\n  -submap <submap> : Start at submap <submap>. <submap> must be an integer\n    between 1 and %d. The default is to start at submap number 1 or, if a map\n    was specified, at the first submap of that map.\n  -keys <left>-<right>-<up>-<down>-<fire> : Override the default key\n    bindings (cf. KeyCodes)\n", VERSION, msg, GAME_PERIOD, SYSVID_MAXZOOM, SYSVID_MAXZOOM, MAP_NBR_MAPS-1, MAP_NBR_SUBMAPS);
#endif
	exit(1);
}

/*
 * Read and process arguments
 */
void
sysarg_init(int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++) {

    if (!strcmp(argv[i], "-speed")) {
      if (++i == argc) sysarg_fail("missing speed value");
      sysarg_args_period = atoi(argv[i]) - 1;
      if (sysarg_args_period < 0 || sysarg_args_period > 99)
	sysarg_fail("invalid speed value");
    }

    else if (!strcmp(argv[i], "-map")) {
      if (++i == argc) sysarg_fail("missing map number");
      sysarg_args_map = atoi(argv[i]) - 1;
      if (sysarg_args_map < 0 || sysarg_args_map >= MAP_NBR_MAPS-1)
	sysarg_fail("invalid map number");
    }

    else if (!strcmp(argv[i], "-submap")) {
      if (++i == argc) sysarg_fail("missing submap number");
      sysarg_args_submap = atoi(argv[i]) - 1;
      if (sysarg_args_submap < 0 || sysarg_args_submap >= MAP_NBR_SUBMAPS)
	sysarg_fail("invalid submap number");
    }
#ifdef ENABLE_SOUND
    else if (!strcmp(argv[i], "-vol")) {
      if (++i == argc) sysarg_fail("missing volume");
      sysarg_args_vol = atoi(argv[i]) - 1;
      if (sysarg_args_submap < 0 || sysarg_args_submap >= SYSSND_MAXVOL)
	sysarg_fail("invalid volume");
    }

    else if (!strcmp(argv[i], "-nosound")) {
      sysarg_args_nosound = 1;
    }
#endif
	else if (!strcmp(argv[i], "-data")) {
		if (++i == argc) sysarg_fail("missing data");
		sysarg_args_data = argv[i];
	}

    else {
      sysarg_fail("invalid argument(s)");
    }

  }

  /* this is dirty (sort of) */
  if (sysarg_args_submap > 0 && sysarg_args_submap < 9)
    sysarg_args_map = 0;
  if (sysarg_args_submap >= 9 && sysarg_args_submap < 20)
    sysarg_args_map = 1;
  if (sysarg_args_submap >= 20 && sysarg_args_submap < 38)
    sysarg_args_map = 2;
  if (sysarg_args_submap >= 38)
    sysarg_args_map = 3;
  if (sysarg_args_submap == 9 ||
      sysarg_args_submap == 20 ||
      sysarg_args_submap == 38)
    sysarg_args_submap = 0;

}

/* eof */





