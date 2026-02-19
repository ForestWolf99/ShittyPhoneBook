#include "core/contact.h" 
using namespace std;

class phonebook
{
private:
    int con_idx = 0;

    void moveElementToEnd(contact arr[], int size, int index_to_move) {
    if (index_to_move >= size || index_to_move < 0) return;

    contact temp = arr[index_to_move];

    // Shift all elements after the index to the left by one position
    for (int i = index_to_move; i < size - 1; ++i) {
        arr[i] = arr[i + 1];
    }

    // Place the original element at the end
    arr[size - 1] = temp;
}    


public:
    phonebook(/* args */);
    ~phonebook();

    contact contacts[8] = {contact(), contact(), contact(), contact(), contact(), contact(), contact(), contact()};

    void add_contact(string name, int number) {
        if (con_idx < sizeof(contacts) / sizeof(contacts[0])) {
        contacts[con_idx].contact_name = name;
        contacts[con_idx].contact_number = number;
        con_idx++;
    } else {
        moveElementToEnd(contacts, sizeof(contacts) / sizeof(contacts[0]), 0);
        contacts[con_idx - 1].contact_name = name;
        contacts[con_idx - 1].contact_number = number;
    }
}
};

phonebook::phonebook(/* args */)
{
}

phonebook::~phonebook()
{
}
