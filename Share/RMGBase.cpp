#include "RMGBase.h"


CRMGBase::CRMGBase(void)
    :   CONST_HEADER_EXT(".ldr")
    ,   CONST_PREORB_EXT(".orb")
    ,   CONST_MATH_LIGHT_VELOCITY(299792458)
    ,   CONST_MATH_PI(3.141592658978)
{
}


CRMGBase::~CRMGBase(void)
{
}


string CRMGBase::GetHeadername()
{
    int docIndex=this->m_lpFullname.find_last_of('.');
    return docIndex>0?m_lpFullname.substr(0, docIndex)+CONST_HEADER_EXT:m_lpFullname+CONST_HEADER_EXT;
}
string CRMGBase::GetFilename()
{
    return m_lpFullname.substr(m_lpFullname.find_last_of('\\')+1);
}

string CRMGBase::GetPreOrbitname()
{
    int docIndex=this->m_lpFullname.find_last_of('.');
    return docIndex>0?m_lpFullname.substr(0, docIndex)+CONST_PREORB_EXT:m_lpFullname+CONST_PREORB_EXT;
}
