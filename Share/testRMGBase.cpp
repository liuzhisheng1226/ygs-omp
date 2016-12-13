#include "RMGBase.h"
#include <iostream>

using namespace std;

int main() {
    /*
    Point p;
    p.x = 1;
    p.y = 1;
    cout << "p.x:" << p.x << endl;
    CPreOrb *cpo = new CPreOrb();
    cpo->m_fileName = "iahrf";
    cout << cpo->m_fileName << endl;
    Rect *rct = new Rect(1,2,43,5);
    cout << rct->left << endl;
    CRMGHeader *rmgh = new CRMGHeader();
    rmgh->Sample = 1;
    cout << rmgh->Sample << endl; 
    */
    CRMGBase *rmgb = new CRMGBase();
    rmgb->m_lpFullname = "jsadlofj.reert";
    cout << rmgb->GetHeadername() << endl;
    return 0;
}

