/* Form definition file generated with fdesign. */

#include "forms.h"
#include "dpcform.h"

FL_FORM *box1;

FL_OBJECT
        *sendfile,
        *exit_pb,
        *filmtype,
        *exposure,
        *nextpicture_pb,
        *timer,
        *updatetimer,
        *sendcolor,
        *exposecolor,
        *exposefile,
        *invert;

void create_form_box1(void)
{
  FL_OBJECT *obj;
  box1 = fl_bgn_form(FL_NO_BOX,800.0,600.0);
  obj = fl_add_box(FL_UP_BOX,0.0,0.0,800.0,600.0,"");
  obj = fl_add_box(FL_UP_BOX,0.0,0.0,800.0,600.0,"");
    fl_set_object_color(obj,4,1);
  obj = fl_add_box(FL_DOWN_BOX,420.0,10.0,370.0,470.0,"Progress");
    fl_set_object_lsize(obj,16.000000);
    fl_set_object_align(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  sendfile = obj = fl_add_box(FL_DOWN_BOX,530.0,400.0,240.0,40.0,"- no file -");
  obj = fl_add_box(FL_DOWN_BOX,10.0,520.0,780.0,70.0,"Digital Palette Control Panel");
    fl_set_object_lsize(obj,16.000000);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_box(FL_DOWN_BOX,10.0,10.0,400.0,470.0,"Setup");
    fl_set_object_lsize(obj,16.000000);
    fl_set_object_align(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  exit_pb = obj = fl_add_button(FL_NORMAL_BUTTON,660.0,540.0,100.0,30.0,"Exit");
    fl_set_call_back(obj,exit_cb,0);
  filmtype = obj = fl_add_choice(FL_NORMAL_CHOICE,90.0,400.0,300.0,40.0,"Film Type");
    fl_set_call_back(obj,filmtype_cb,0);
  exposure = obj = fl_add_choice(FL_NORMAL_CHOICE,90.0,340.0,300.0,40.0,"Exposure");
    fl_set_call_back(obj,exposure_cb,0);
  nextpicture_pb = obj = fl_add_button(FL_NORMAL_BUTTON,90.0,110.0,300.0,40.0,"Next Picture");
    fl_set_call_back(obj,nextpicture_cb,0);
  timer = obj = fl_add_timer(FL_VALUE_TIMER,90.0,40.0,300.0,40.0,"Timer");
    fl_set_call_back(obj,timer_cb,90);
  updatetimer = obj = fl_add_timer(FL_HIDDEN_TIMER,320.0,450.0,190.0,60.0,"Timer");
    fl_set_call_back(obj,updatetimer_cb,0);
  sendcolor = obj = fl_add_slider(FL_HOR_SLIDER,530.0,340.0,240.0,40.0,"Sending Color");
    fl_set_object_align(obj,FL_ALIGN_LEFT);
  exposecolor = obj = fl_add_slider(FL_HOR_SLIDER,530.0,200.0,240.0,40.0,"Exposing Color");
    fl_set_object_align(obj,FL_ALIGN_LEFT);
  exposefile = obj = fl_add_box(FL_DOWN_BOX,530.0,260.0,240.0,40.0,"");
  invert = obj = fl_add_choice(FL_NORMAL_CHOICE,90.0,280.0,300.0,40.0,"Invert");
    fl_set_call_back(obj,invert_cb,0);
  fl_end_form();
}

/*---------------------------------------*/

void create_the_forms(void)
{
  create_form_box1();
}

