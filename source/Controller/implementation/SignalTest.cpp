#include "Signal.hpp"
#include <atomic>
#include <chrono>
#include <exception>
#include <iterator>
#include <thread>
#include <iostream>

using namespace CSOL_Utilities;

auto sig1 = Signal::create();
auto sig2 = Signal::create();
auto sig3 = Signal::create();
auto sig4 = Signal::create();
auto sig5 = Signal::create();
auto sig6 = Signal::create();

auto ut_1 = TokenFactory::make_unit_token(sig1);
auto ut_2 = TokenFactory::make_unit_token(sig1);

auto and_tok1 = TokenFactory::make_and_token(sig1);
auto and_tok2 = TokenFactory::make_and_token(sig1);
auto and_tok3 = TokenFactory::make_and_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto and_tok4 = TokenFactory::make_and_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto and_tok5 = TokenFactory::make_and_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto or_tok1 = TokenFactory::make_or_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto or_tok2 = TokenFactory::make_or_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto or_tok3 = TokenFactory::make_or_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto or_tok4 = TokenFactory::make_or_token(sig1, sig2, sig3, sig4, sig5, sig6);
auto or_tok5 = TokenFactory::make_or_token(sig1, sig2, sig3, sig4, sig5, sig6);

void worker_1()
{
    int repeat = 10000;
    while (repeat--)
    {
        ut_1->acquire();
        std::cout << "Worker 1 acquires token." << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ut_1->release();
        std::cout << "Worker 1 releases token." << std::endl;
    }
    std::cout << "1 ok" << std::endl;
}

void worker_2()
{
    int repeat = 10000;
    while (repeat--)
    {
        ut_2->acquire();
        std::cout << "Worker 2 acquires token." << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ut_2->release();
        std::cout << "Worker 2 releases token." << std::endl;
    }
    std::cout << "2 ok" << std::endl;
}

void worker_3()
{
    int repeat = 10000;
    while (repeat--)
    {
        or_tok3->acquire();
        std::cout << "Worker 3 acquires token." << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        or_tok3->release();
        std::cout << "Worker 3 releases token." << std::endl;
    }
    std::cout << "3 ok" << std::endl;
}
void worker_4()
{
    int repeat = 10000;
    while (repeat--)
    {
        and_tok4->acquire();
        std::cout << "Worker 4 acquires token." << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        and_tok4->release();
        std::cout << "Worker 4 releases token." << std::endl;
    }
    std::cout << "4 ok" << std::endl;
}
void worker_5()
{
    int repeat = 10000;
    while (repeat--)
    {
        and_tok5->acquire();
        std::cout << "Worker 5 acquires token." << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        and_tok5->release();
        std::cout << "Worker 5 releases token." << std::endl;
    }
    std::cout << "5 ok" << std::endl;
}

int main()
{
    try{
        std::thread t1(worker_1);
        std::thread t2(worker_2);
        // std::thread t3(worker_3);
        // std::thread t4(worker_4);
        // std::thread t5(worker_5);
        t1.join();
        t2.join();
        // t3.join();
        // t4.join();
        // t5.join();
    } catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
