#include <cstdint>
#include <iostream>
#include "dto.h"

using namespace std;

int main()
{
    uint8_t* buf = (uint8_t*)malloc(10 * sizeof(int));
    /* test read */
    BinaryWriter binary_writer(buf, 10 * sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        binary_writer.write(i);
    }
    BinaryReader binary_reader(buf, 10 * sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        int* x = new int();
        binary_reader.read(x);
        cout << *x << endl;
    }
}