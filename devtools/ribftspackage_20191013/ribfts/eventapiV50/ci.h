#ifndef CI
#define CI

/************************************************************
** include files
*/
#include <sys/errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef SYS_DIR
#include <sys/dir.h>
#else
#ifdef NDIR
#include <ndir.h>
#else
#include <dirent.h>
#endif
#endif

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <Xm/XmAll.h>

#include "f_cmd_tree.h"
#include "f_cmd.h"

/*****************************************************************
** macros
*/
#define MAX_LEVELS 20
#define MAX_ARGS 20
#define TITLE_STRING "COMMAND INTERFACE"
#define DEBUG 1 /* for condition compile */

#define MIN_WIDTH 300
#define MIN_HEIGHT 400
#define MAIN_FORM_WIDTH 350
#define MAIN_FORM_HEIGHT 500
#define MF_WIDTH_DIFF 40
#define MF_HEIGHT_DIFF 40

#define SEPARATOR_HEIGHT 10
#define SUB_FORM1_HEIGHT 25
#define SUB_FORM2_HEIGHT 25
#define BUTTON_WIDTH 40
#define BUTTON_HEIGHT 30
#define BUTTON_ROW_WIDTH (BUTTON_WIDTH*2)
#define ITEM_WIDTH 100

#define MAX_LINE_CHARS 80
#define TASK_SEP "::"
#define MAX_BTN_NUM 400
#define MAX_TASK_NUM 100
#define MAX_HISTORY_NUM 50
#define MAX_NAMESAKE 50

/*****************************************************************
** new data type
*/

/******************************************************************
** forward declarations
*/
Widget f_cre_app(Widget parent,char *c_prompt);
Widget f_drawing_area(Widget parent,char *c_prompt);
Widget f_menu_bar(Widget parent,char *c_prompt);
Widget f_sel_item(Widget parent,char *c_prompt);
Widget f_cmd_in(Widget parent,char *c_prompt);
Widget f_prompt(Widget parent,char *c_prompt);
Widget f_sel_file(Widget parent,char *c_prompt);
Widget f_mw_button(Widget parent,char *c_prompt);
Widget f_msg(Widget parent,char *c_prompt);
Widget f_item_button(Widget parent,char *c_prompt);

void f_get_mw_size();

void f_re_sz_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_op_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_hp_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_si_pb_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_ci_pb_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_cmd_in_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_prompt_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_sel_file_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_mw_pb_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_sel_item_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_dp_cmd();
int  f_run_cmd(int run_flag);
int  f_spawn_cmd(char * pc_cmd,int run_flag);

/* after 1 nov */
void f_argu_dlg();
void f_argu_entry();
int f_argu_append();

void f_add_his(s_coms * tcp,int fg);
s_coms * f_back_his();
s_coms * f_forw_his();
void f_c_normal_btn(s_coms * tcp);
void f_c_all_btn();
void f_c_cmd_btn(s_coms * tcpp);
void f_dp_cur_node();
void f_c_cmdbtn_in_son(char * patern,s_coms * itself);
void f_c_all_son_btn(s_coms * itself);

/*int f_check_exist(s_coms * patern);
int f_check_exist_in_son(s_coms * patern,s_coms * itself);*/
void f_dc_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_sel_cmd_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_button_box();
void f_adjust_button();
s_coms * f_locate_btn(char * btnstr,s_coms * begn);
int f_sort_btn(char * btnstr);
void f_task_sel();
void f_task_cancel_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_task_help_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_sel_task_cb(Widget parent,caddr_t client_data,caddr_t call_data);
void f_task_dlg(Widget parent,char *c_prompt);
void f_cmd_str();
void f_f_namesake();
void f_task_entry();
s_task * f_next_task(s_task *);

#endif

