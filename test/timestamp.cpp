#include <ctime>
#include <iostream>

int main()
{
    std::time_t t;
    std::time(&t);
    std::cout << t << std::endl;
}