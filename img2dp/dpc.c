/*
	Program dpc.

	Digital Palette Control Program.
*/

#include <stdio.h>
#include <dpalette.h>
#include <fcntl.h>
#undef RED
#undef GREEN
#undef BLUE

#include <forms.h>
#include "dpcform.h"
#include "img2dp.h"

#define UPDATE_INTERVAL 1
#define INFFILE "img2dp.inf"
#define TIMER_SETTING 90.0

static IMG2DP_INFO img2dp_info;
static DP_DATA out_dp;
static DP_DATA *dp = &out_dp;
static char inffilename[140];
static int waiting_for_go;

#define MIN(x,y) ( (x) < (y) ? (x) : (y) )

extern char *getenv();

/*
	Callback routines:
*/

void exit_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
  exit(0);
}

void filmtype_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
  int i;
  get_info();
  i = fl_get_choice(filmtype);
  if(i)
  {
    img2dp_info.filmnumber = i-1;
    put_info();
#ifdef DEBUG
    printf("Setting filmnumber: %d\n", img2dp_info.filmnumber);
#endif
  }
}

void exposure_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
  int i;
  get_info();
  i = fl_get_choice(exposure);
  if(i)
  {
    img2dp_info.lighten_darken = i-1;
    put_info();
#ifdef DEBUG
    printf("Setting lighten_darken: %d\n", img2dp_info.lighten_darken);
#endif
  }
}

void invert_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
  int i;
  get_info();
  i = fl_get_choice(invert);
  if(i)
  {
    img2dp_info.invert = i==2;
    put_info();
#ifdef DEBUG
    printf("Setting invert: %d\n", img2dp_info.invert);
#endif
  }
}

void nextpicture_status(stat)
int stat;
{
  if(stat)
  {
    fl_set_object_lcol(nextpicture_pb,0);
    fl_activate_object(nextpicture_pb);
  }
  else
  {
    fl_set_object_lcol(nextpicture_pb,8);
    fl_deactivate_object(nextpicture_pb);
  }
}

void nextpicture_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
  get_info();
  img2dp_info.wait_for_go = FALSE;
  img2dp_info.go_selected = TRUE;
  waiting_for_go = FALSE;
  nextpicture_status(FALSE);
  put_info();

  /*
	Start the timer:
  */

  if(img2dp_info.filmnumber == 12)
  {
    fl_set_timer(timer, (float) 300.0);
  }
  else
  {
    fl_set_timer(timer, (float) TIMER_SETTING);
  }
}

void timer_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
}

void updatetimer_cb(obj, arg)
FL_OBJECT *obj;
long arg;
{
#ifdef DEBUG2
  printf("updatetimer_cb called.\n");
#endif

  /*
	Get status and update form:
  */

  get_info();
  update_forms();

  /*
	Check, if wait_for_go is selected:
  */

  if(img2dp_info.wait_for_go && !waiting_for_go)
  {
    waiting_for_go = TRUE;
    /*
	Enable the pushbutton:
    */
    nextpicture_status(TRUE);
  }
  /*
	Arm the timer again:
  */
  fl_set_timer(updatetimer, (float) UPDATE_INTERVAL);
}

/*
	Get the info file:
*/
get_info()
{
  int retry;
  int fh, st;

  for(retry=0; retry<100000; retry++)
  {
    fh = open(inffilename, O_RDONLY);
    if(fh == -1)
    {
      perror("Opening info file.");
      sleep(2);
      continue;
    }

    st=read(fh, &img2dp_info, sizeof(img2dp_info));
    if(st != sizeof(img2dp_info))
    {
      close(fh);
      sleep(1);
      continue;
    }
    close(fh);
    return;
  }

  fprintf(stderr, "Could not read file: %s\n", inffilename);
  exit(-1);
}

/*
	Put the info file:
*/
put_info()
{
  int retry;
  int fh, st;

  umask(0000);

  for(retry=0; retry<10; retry++)
  {
    fh = open(inffilename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fh == -1) {sleep(1); continue;}

    st=write(fh, &img2dp_info, sizeof(img2dp_info));
    if(st != sizeof(img2dp_info))
    {
      close(fh);
      sleep(1);
      continue;
    }
    close(fh);
    return;
  }

  fprintf(stderr, "Could not write file: %s\n", inffilename);
  exit(-1);
}

main(argc,argv)
int argc;
char **argv;
{
  int i,j;
  int st;
  char *ConfigDir;
  char Drive[3];
  char Path[129];
  char Fname[9];
  char Fext[5];
  int Size;
  char buf1[80], buf2[80];
  int DoRGB=TRUE, DoDouble=TRUE;

  /*
	Analyse argv:
  */

  for(i=1; i<argc; i++)
  {
    if(!strncasecmp(argv[i],"rgb",3))
    {
      DoRGB=TRUE;
    }
    else if(!strncasecmp(argv[i],"norgb",5))
    {
      DoRGB=FALSE;
    }
    else if(!strncasecmp(argv[i],"double",6))
    {
      DoDouble=TRUE;
    }
    else if(!strncasecmp(argv[i],"nodouble",8))
    {
      DoDouble=FALSE;
    }
  }

  fl_init();
  fl_set_graphics_mode(DoRGB, DoDouble); /* RGB, double buffer */
  /* fl_set_graphics_mode(FALSE, TRUE); Pseudo, double */
  create_the_forms();

  /*
	Build inffilename:
  */

  ConfigDir = getenv("DPALETTE") ;
  if (!ConfigDir)
  {
    printf("Environment DPALETTE not found.") ;
    return(CONFIGERR) ;
  }
  DP_PathParse(ConfigDir, Drive, Path, Fname, Fext) ;
  sprintf(inffilename, "%s/%s", Path, INFFILE);

  /*
	Get DPALETTE.CFG to get file names:
  */

  if (DP_FileOpen(dp, ConfigDir))
  {
    printf("Config File open error.") ;
    return(CONFIGERR) ;
  }
  Size = DP_FileRead(dp, (char *) dp, CONFIG_PARAM_BYTES) ;
  DP_FileClose(dp) ;
  if (Size < CONFIG_PARAM_BYTES)
  {
    printf("Config File read error.") ;
    return(CONFIGERR) ;
  }
  DP_SwapShorts(dp);

  waiting_for_go = FALSE;
  nextpicture_status(FALSE);

  get_info();

  /*
	Insert information into menus:
  */

  for(i=0; i<20; i++)
  {
    for(j=0; j<27; j++)
    {
      buf1[j] = DP_FILM_NAME(dp, i)[j];
      if(!buf1[j]) buf1[j] = ' ';
    }
    buf1[27] = '\0';
    sprintf(buf2, "%2d: %27s", i, buf1);
    fl_addto_choice(filmtype, buf2);
  }

  fl_addto_choice(exposure, "Darkest");
  fl_addto_choice(exposure, "Darker");
  fl_addto_choice(exposure, "Dark");
  fl_addto_choice(exposure, "Normal");
  fl_addto_choice(exposure, "Light");
  fl_addto_choice(exposure, "Lighter");
  fl_addto_choice(exposure, "Lightest");

  fl_addto_choice(invert, "No");
  fl_addto_choice(invert, "Yes");

  update_forms();

  fl_show_form(box1, FL_PLACE_CENTER, TRUE, "DP Control Program");

  fl_set_timer(updatetimer, (float) UPDATE_INTERVAL);

  while(TRUE) fl_do_forms();
}

update_forms()
{
  char buf1[80];

  fl_freeze_form(box1);
  fl_set_choice(filmtype, img2dp_info.filmnumber+1);
  fl_set_choice(exposure, img2dp_info.lighten_darken+1);
  fl_set_choice(invert, img2dp_info.invert+1);
  if(img2dp_info.color_xmit < 0)
  {
    fl_set_object_color(sendcolor, 47, 47);
    fl_set_slider_value(sendcolor, 0.0);
    fl_set_object_label(sendfile, "- no file -");
  }
  else
  {
    fl_set_object_color(sendcolor, 47, 1<<(img2dp_info.color_xmit));
    fl_set_slider_value(sendcolor, MIN(img2dp_info.lines_xmit, 1.0));
    fl_set_object_label(sendfile, img2dp_info.filename);
  }
  if(img2dp_info.current_color < 0)
  {
    fl_set_object_color(exposecolor, 47, 47);
    fl_set_slider_value(exposecolor, 0.0);
  }
  else
  {
    fl_set_object_color(exposecolor, 47, 1<<(img2dp_info.current_color));
    fl_set_slider_value(exposecolor, MIN(img2dp_info.lines_exposed, 1.0));
  }
  if(img2dp_info.imagesinqueue==0)
  {
    sprintf(buf1, "Last picture");
  }
  else if(img2dp_info.imagesinqueue==1)
  {
    sprintf(buf1, "1 Picture in queue");
  }
  else
  {
    sprintf(buf1, "%d Pictures in queue", img2dp_info.imagesinqueue);
  }

  fl_set_object_label(exposefile, buf1);
  fl_unfreeze_form(box1);
}


