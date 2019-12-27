#include <iostream>
#include<string>
#include<vector>
using namespace std;
struct line {
    int length;
    char contents[0];
};
void work(vector<int>* v);
int main() {
    char name[20];
    char* ptr = name;
    strcpy(name, "fuck");
    int bias = 0; 
    //cout << *(char*)ptr;
    for (int i = 0; i < 4; i++)
    {
        cout << *(ptr+bias);
        bias += 1;
    }
}
