#include "typedefs.h"
#ifndef UT
#define UT

/*****************************************************************
** new data type
*/
typedef struct{
  char * pc_item;
  int l_selected;
} s_list;

/******************************************************************
** forward declarations
*/
/* May 26 1996 */
void f_sel_file(char *pc_title,char * pc_file_name);
void f_prompt(char *pc_title,char * pc_prompt,char * pc_input);
void f_question(char *pc_title,char * pc_message,int * pi_yes_or_no);
void f_select(char *pc_title,char * pc_prompt,char * pc_item_array[],int list_num,char * pc_input);
void f_print_warning(char *pc_title,char * pc_warning);
void f_print_error(char *pc_title,char * pc_error);
void f_print_message(char *pc_title,char * pc_message);
void f_multi_select(char *pc_title,char * pc_prompt,s_list s_user_list[],int list_num);

#endif

