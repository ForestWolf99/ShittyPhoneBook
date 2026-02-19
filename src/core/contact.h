#include <string> 
using namespace std;

class contact
{
private:
    /* data */
public:
    contact(/* args */);
    ~contact();

    string contact_name = string("Empty");
    int contact_number = 0;
};

contact::contact(/* args */)
{
}

contact::~contact()
{
}
