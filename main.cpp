#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <random>
#include <ctime>
const double MAX_WAIT = 1000; const double MIN_WAIT = MAX_WAIT/2;
const double MAX_TRY_PAUSE= MIN_WAIT; const double MIN_TRY_PAUSE = MAX_TRY_PAUSE/2;
using namespace std;

int myrand(int min, int max) {
    static mt19937 rnd(time(nullptr));
    return uniform_int_distribution<>(min,max)(rnd);
}


void philosopher(int ph, mutex& ma, mutex& mb, mutex& mo) {
    auto get_hungry_at = chrono::high_resolution_clock::now();
    chrono::duration<double> max_starved_for = get_hungry_at-get_hungry_at;
    for (;;) {  // prevent thread from termination

        int duration = myrand(MIN_WAIT, MAX_WAIT);
        {
            // Block { } limits scope of lock
            lock_guard<mutex> gmo(mo);
            cout<<ph<<" thinks "<<duration<<"ms\n";
        }
        this_thread::sleep_for(chrono::milliseconds(duration));
        {
            lock_guard<mutex> gmo(mo);
            get_hungry_at = chrono::high_resolution_clock::now();
            cout<<"\t\t"<<ph<<" is hungry\n";
        }
        unique_lock<mutex> ul_ma(ma, defer_lock);
        unique_lock<mutex> ul_mb(mb, defer_lock);
        {
            while (true) {
                ul_ma.lock();
                if (ul_mb.try_lock()) {
                    break;
                } else {
                    ul_ma.unlock();
                    duration = myrand(MIN_TRY_PAUSE, MAX_TRY_PAUSE);
                    this_thread::sleep_for(chrono::milliseconds(duration));
                }

            }
        }
        duration = myrand(MIN_WAIT, MAX_WAIT);
        {
            lock_guard<mutex> gmo(mo);
            chrono::duration<double> passed_time = chrono::high_resolution_clock::now()-get_hungry_at;
            if (max_starved_for < passed_time){
                max_starved_for = passed_time;
            }
            cout<<"\t\t\t\t"<<ph<<" eats "<<duration<<"ms --- starved for "<< passed_time.count() << "s --- max starved for " << max_starved_for.count() << "\n";
        }
        this_thread::sleep_for(chrono::milliseconds(duration));
    }
}

int main() {
    int CONST = 5;
    cout<<"dining Philosophers C++11 with Resource hierarchy\n";
    mutex m1, m2, m3, m4, m5;   // 5 forks are 5 mutexes
    mutex mo;           // for proper output

    thread t1([&] {philosopher(1, m1, m2, mo);});
    thread t2([&] {philosopher(2, m2, m3, mo);});
    thread t3([&] {philosopher(3, m3, m4, mo);});
    thread t4([&] {philosopher(4, m4, m5, mo);});
    thread t5([&] {philosopher(5, m5, m1, mo);});  // Force a resource hierarchy
    t1.join();  // prevent threads from termination
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    /**
    mutex mo;
    mutex m0;

    mutex prev;
    mutex next;
    auto start = &prev;
    for (int i=0; i < CONST; i++){
        thread t([&] {philosopher(1, prev, next, mo);});

    }

     * **/

}