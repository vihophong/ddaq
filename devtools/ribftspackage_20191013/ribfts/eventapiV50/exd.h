#ifndef EI
#define EI

/* all varibles used in this module is defined here************
   because the structure of the program is streamline, no
   cross calling. And every varible is a concreat object
   which will not be confused or it's a temp varible which
   will cast off after use                                */
/* all these varibles should be static after I combined all
   procedure into this one module, except "root_node_ptr" */

   XtAppContext app_context;

/* all widgets I used defined here */
   /* abbreviation : the widget's parent is P, daughters is D */
extern   Widget toplevel;    /* P=NULL, D=mainwindow */
extern    Widget mainwindow;  /* P=toplevel, D= menubar,drawingarea */
extern    Widget menubar;     /* P=mainwindow, D=pulldown[2],cascade[2] */
extern    Widget pulldown[2]; /* P=menubar, D=pbutton[3] */
extern    Widget pbutton[MAX_BTN_NUM];  /* P=pulldown,rowcolumn, D=NULL */
      /* notice it has 2 parents at 2 case */
extern    Widget cascade[2];  /* P=menubar, D=NULL */
extern    Widget drawingarea; /* P=mainwindow, D=mform */
extern    Widget mform;       /* P=drawingarea, D=main window button form,
      separator1,selection item form,separator2,command input form */
extern    Widget btnform;     /* P=mform, D=rowcolumn */
extern    Widget rowcolumn;   /* P=btnform,selform,scrwin,cmdwin, D=pbutton[3] +      /* notice it has 4 parents at 4 case */
extern    Widget separator1;  /* P=mform, D=NULL */
extern    Widget selform;     /* P=mform, D=scrwin,sicform */
extern    Widget scrwin;     /* P=selform, D=rowcolumn */
extern    Widget sicolumn;
extern    Widget sicform;     /* P=selform, D=pbutton[3] *//* sel-item-cmd-form+extern    Widget separator2;  /* P=mform, D=NULL */
extern    Widget cmdform;     /* P=mform, D=cmdwin,cicform */
extern    Widget cmdwin;      /* P=cmdform, D=NULL */
extern    Widget cicform;     /* P=cmdform, D=pbutton[2] *//* cmd-input-cmd-for+extern    Widget pmtbox;      /* P=??, D=?? */
extern    Widget fselbox;     /* P=??, D=?? */
/* after 1 nov */
extern Widget argudlg,arguform,argucmd,pressed_btn,tasksel;
extern XtTranslations xt;
extern int inc_task[MAX_TASK_NUM]; /* 1 mean include relevent task */
extern int save_posi;
extern char pc_cmd[MAX_LINE_CHARS];
extern int cur_posi;
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
                                   --cicform--pbutton[n2]

***************end of widget define                            */

extern s_task * root_task_ptr;
extern s_task * temp_task_ptr;
extern s_coms * root_node_ptr;
extern s_coms * cmd_node_ptr;
extern s_coms * temp_node_ptr;
extern s_args * argu_node_ptr;

/* globle varibles define */
extern    int same_btn[MAX_BTN_NUM]; /* 0 mean no namesake for this btn */
extern    int n;      /* arguments count */
extern    int i_b;    /* count for children */
extern    int i_temp; /* be sure temp usage */
extern    int i_loop; /* loop count */

extern    char * label_string;
extern    char * pc_temp;
extern    char * pc_temp2;
extern    char c_temp[MAX_LINE_CHARS]; /* temp 80 chars */
extern    char temp[]; /* debug use */
extern    Arg args[MAX_ARGS]; /* arg list */
extern    XmString x_str_temp;
   /* XmNwidth actually is short type, so error if you give int type */
extern   Dimension da_width, da_height; /* drawing_area's width and height */

/****************end of all varibles definition ********************/

#endif

