#include "oncsSub_idcaenv1730pha.h"
#include <cstring>

oncsSub_idcaenv1730pha::oncsSub_idcaenv1730pha(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{
    pos = 0;
}
  


void  oncsSub_idcaenv1730pha::dump ( OSTREAM& os )
{
  os << std::endl;
}


int* oncsSub_idcaenv1730pha::getIntArray(int *nwout, const char *what)
{
    return &SubeventHdr->data;
}
int oncsSub_idcaenv1730pha::getHitFormat() const
{
    return (int) SubeventHdr->reserved[0];
}

int oncsSub_idcaenv1730pha::iValue(const int sample, const int ch)
{
    int* word =  &SubeventHdr->data;
    return word[0];
}

int oncsSub_idcaenv1730pha::iValue(const int n,const char * what)
{
    int* word =  &SubeventHdr->data;
    return word[0];
}

int oncsSub_idcaenv1730pha::convert()
{
    int* word = &SubeventHdr->data;
    int totalsize = getPadding();
    if (pos>=totalsize) return 0;
    cout<<"Data convert"<<word[pos]<<endl;
    pos++;
    return 1;
}
