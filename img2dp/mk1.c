/* Form definition file generated with fdesign. */

#include "forms.h"
#include "mk1.h"

FL_FORM *t1;


void create_form_t1(void)
{
  FL_OBJECT *obj;
  t1 = fl_bgn_form(FL_NO_BOX,440.0,320.0);
  obj = fl_add_box(FL_UP_BOX,0.0,0.0,440.0,320.0,"");
  obj = fl_add_menu(FL_PUSH_MENU,10.0,240.0,420.0,70.0,"Menu");
  obj = fl_add_valslider(FL_HOR_SLIDER,10.0,190.0,420.0,40.0,"");
  fl_end_form();
}

/*---------------------------------------*/

void create_the_forms(void)
{
  create_form_t1();
}

