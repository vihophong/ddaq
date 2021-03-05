#include "oncsSub_idnodecode.h"
#include <cstring>

oncsSub_idnodecode::oncsSub_idnodecode(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{
}
  


void  oncsSub_idnodecode::dump ( OSTREAM& os )
{
  os << std::endl;
}


int* oncsSub_idnodecode::getIntArray(int *nwout, const char *what)
{
    return &SubeventHdr->data;
}
int oncsSub_idnodecode::getHitFormat() const
{
    return (int) SubeventHdr->reserved[0];
}
