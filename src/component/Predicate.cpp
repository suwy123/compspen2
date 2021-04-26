#include "component/Predicate.h"

void Predicate::show() {

    cout << "pars: \n";
    for (unsigned int i=0; i<m_pars.size(); i++) {
        cout << m_pars[i] << " ";
    }
    cout << endl;

    cout << "base: " << m_base_rule << endl;
    cout << "rec: " << m_rec_rule << endl;
}
