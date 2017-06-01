#include <bits/stdc++.h>
#include "file.hpp"

using namespace std;

struct Record {
    bool good;
    char name[256];
    char phone[15];
    char address[256];
    Record() {
    
    }
    Record(const std::string & argname, const std::string & argphone, const std::string & argaddress) {
        strcpy(name, argname.c_str());
        strcpy(phone, argphone.c_str());
        strcpy(address, argaddress.c_str());
        good = true;
    }
};

int main() {
    File<Record> f("teste.log");
    f.write(Record("gabriel", "99990000", "ufba"), 0);
    f.write(Record("dahia", "01321231", "im"), 1);
    f.write(Record("fernandes", "201510539", "departamento de ciencia da computacao"), 16);
    
    Record r = f.read(0);
    if (r.good)
        std::cout << r.name << std::endl;
    else
        puts("error");
    
    r = f.read(1);
    if (r.good)
        std::cout << r.name << " " << r.phone << " " << r.address << std::endl;
    else
        puts("error");
    
    r = f.read(2);
    if (r.good)
        std::cout << r.name << " " << r.phone << " " << r.address << std::endl;
    else
        puts("error");

    r = f.read(16);
    if (r.good)
        std::cout << r.name << " " << r.phone << " " << r.address << std::endl;
    else
        puts("error");
}
