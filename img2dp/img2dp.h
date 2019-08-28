/*
	Structure for img2dp

*/

#define MAX_FILENAME 80

typedef struct
{

  /*
	DP information:
  */

  int background_mode;
  int background_pixel_value;
  int blue_color_balance;
  int blue_exp_time;
  int blue_luminant;
  int current_color;
  int filmnumber;
  int green_color_balance;
  int green_exp_time;
  int green_luminant;
  int imagesinqueue;
  int lighten_darken;
  float lines_exposed;
  unsigned char lower_left_color[3];
  unsigned char lower_right_color[3];
  int red_color_balance;
  int red_exp_time;
  int red_luminant;
  unsigned char upper_left_color[3];
  unsigned char upper_right_color[3];
  int invert;

  /*
	img2dp information:
  */

  char filename[MAX_FILENAME];
  int color_xmit;
  float lines_xmit;
  int wait_for_go, go_selected;

} IMG2DP_INFO;

