#include <syncstream>
#include <thread>
#include <iostream>
#include <chrono> 
using namespace std;
using namespace std::chrono_literals;

void f(char x, int i) {
    osyncstream(cout) << "From the set " << x << " done the " << i << ".\n";
    this_thread::sleep_for(1ms);
}