/* Header file generated with fdesign. */

/**** Callback routines ****/

extern void exit_cb(FL_OBJECT *, long);
extern void filmtype_cb(FL_OBJECT *, long);
extern void exposure_cb(FL_OBJECT *, long);
extern void nextpicture_cb(FL_OBJECT *, long);
extern void timer_cb(FL_OBJECT *, long);
extern void updatetimer_cb(FL_OBJECT *, long);
extern void invert_cb(FL_OBJECT *, long);



/**** Forms and Objects ****/

extern FL_FORM *box1;

extern FL_OBJECT
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



/**** Creation Routine ****/

extern void create_the_forms(void);
