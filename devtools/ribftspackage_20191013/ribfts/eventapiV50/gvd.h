#ifndef GI
#define GI

/* all varibles used in this module is defined here************
   because the structure of the program is streamline, no
   cross calling. And every varible is a concreat object
   which will not be confused or it's a temp varible which
   will cast off after use                                */
/* all these varibles should be static after I combined all
   procedure into this one module, except "root_node_ptr" */

   XtAppContext app_context;

/* all widgets I used defined here */
   /* abbreviation : the widget's parent is P, clones is D */
   Widget toplevel;    /* P=NULL, D=mainwindow */
   Widget mainwindow;  /* P=toplevel, D= menubar,drawingarea */
   Widget menubar;     /* P=mainwindow, D=pulldown[2],cascade[2] */
   Widget pulldown[2]; /* P=menubar, D=pbutton[3] */
   Widget pbutton[MAX_BTN_NUM];  /* P=many widgets, D=NULL */
   Widget cascade[2];  /* P=menubar, D=NULL */
   Widget drawingarea; /* P=mainwindow, D=mform */
   Widget mform;       /* P=drawingarea, D=main window button form,
      separator1,selection item form,separator2,command input form, 
      separator3,msgform                                           */
   Widget btnform;     /* P=mform, D=rowcolumn */
   Widget rowcolumn;   /* P=btnform, D=pbutton[3] */
   Widget separator1;  /* P=mform, D=NULL */
   Widget selform;     /* P=mform, D=scrwin,sicform */
   Widget scrwin;      /* P=selform, D=sicolumn */
   Widget sicolumn;    /* P=scrwin, D=pbutton[0] */
   Widget sicform;     /* P=selform, D=pbutton[3] *//* sel-item-cmd-form */
   Widget separator2;  /* P=mform, D=NULL */
   Widget cmdform;     /* P=mform, D=cmdwin,cicform */
   Widget cmdwin;      /* P=cmdform, D=NULL */
   Widget cicform;     /* P=cmdform, D=pbutton[2] *//* cmd-input-cmd-form */
   Widget separator3;  /* P=mform, D=NULL */
   Widget msgform;     /* P=mform, D=msglabel */
   Widget msglabel;    /* P=msgform, D=NULL */
   Widget pmtbox;      /* P=??, D=?? */
   Widget fselbox;     /* P=??, D=?? */

/* after 1 nov */
   Widget argudlg,arguform,argucmd,tasksel,pressed_btn;
   XtTranslations xt;
   int inc_task[MAX_TASK_NUM]; /* 1 mean include relevent task */
   int same_btn[MAX_BTN_NUM]; /* 0 mean no namesake for this btn */
   /* so I have a widget tree as below *********************************

        toplevel
            |                     --pulldown[n1]--pbutton[n2]
       mainwindow---------menubar-|
            |                     --cascade[n1]
            |
            |             --btnform--rowcolumn--pbutton[n2]
            |             |
            |             |-separator1    
            |             |          
            |             |         --scrwin--sicolumn--pbutton[n2]
       drawingarea--mform-|-selform-|
                          |         --sicform--pbutton[n2]
                          |
                          |-separator2
                          |
                          |        --cmdwin
                          -cmdform-|
                          |        --cicform--pbutton[n2]
                          |
                          |-separator3
                          |
                          |
                          -msgform--msglabel
                           
***************end of widget define                            */

s_task * root_task_ptr;
s_task * temp_task_ptr;
s_coms * root_node_ptr;
s_coms * cmd_node_ptr; 
s_coms * temp_node_ptr; 
s_args * argu_node_ptr;

/* globle varibles define */
   int n;      /* arguments count */
   int i_b;    /* count for children */
   int i_temp; /* be sure temp usage */
   int i_loop; /* loop count */

   char * label_string;
   char * pc_temp;
   char * pc_temp2;
   char c_temp[MAX_LINE_CHARS]; /* temp 80 chars */
   char temp[]="1 time"; /* debug use */

   Arg args[MAX_ARGS]; /* arg list */
   XmString x_str_temp;
   /* XmNwidth actually is short type, so error if you give int type */
   Dimension da_width=MAIN_FORM_WIDTH+MF_WIDTH_DIFF;
   Dimension da_height; /* drawing_area's width and height */
   int save_posi=-1;

/****************end of all varibles definition ********************/

#endif

