#include "typedefs.h"
typedef struct
	{
		INTS4 l_com_num;    /* current num of msg */
		INTS4	l_com_type;
		INTS4 l_com_stype;
		CHARS c_message[60];
	} s_commands;

