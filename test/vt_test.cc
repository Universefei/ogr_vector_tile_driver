
#include "ogrsf_frmts.h"
#include <iostream>
using namespace std;

int main (int argc, char const* argv[])
{
    OGRRegisterAll();
    RegisterOGRVT();
    RegisterOGRMEM();
    cout<<""<<endl;

    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("Vector Tile");

    if(!poDriver)
        cout<<"error"<<endl;
    
    cout<<"registerall over"<<endl;
    
    return 0;
}
