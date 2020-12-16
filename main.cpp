#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <random>

using namespace std;

queue<int> customers;
mutex customers_mutex;

bool hairmaker_is_free = true;
mutex hairmaker_mutex;

bool running = true;

void safe_set_hairmaker_is_free(bool value)
{
    hairmaker_mutex.lock();
    hairmaker_is_free = value;
    hairmaker_mutex.unlock();
}

void hairmaker()
{
    safe_set_hairmaker_is_free(true);
    cout << "[hairmaker] Workday started!" << endl;
    int t;
    while (true)
    {
        customers_mutex.lock();
        t = customers.size();
        if (!(running || customers.size() > 0))
        {
            customers_mutex.unlock();
            break;
        }
        customers_mutex.unlock();
        if (t < 1)
        {
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }
        customers_mutex.lock();
        int customer = customers.front();
        customers.pop();
        cout << "[hairmaker] Hairmaker started with customer #" << customer << endl;
        safe_set_hairmaker_is_free(false);
        customers_mutex.unlock();

        this_thread::sleep_for(chrono::seconds(2));
        safe_set_hairmaker_is_free(true);
        // cout << "hairmaker ended with customer #" << customer << ", and go asleep" << endl;
        customers_mutex.lock();
        if (customers.size() > 0)
        {
            cout << "[hairmaker] hairmaker ended with customer #" << customer << ", and carries on with following customer" << endl;
        }
        else
        {
            cout << "[hairmaker] hairmaker ended with customer #" << customer << ", and goes asleep, waiting for customers to come" << endl;
        }
        customers_mutex.unlock();
    }

    cout << "[hairmaker] Woking day has finished" << endl;
}

void customers_flow(int customers_count)
{
    int i = 0;
    // cout << "customers flow is here" << endl;
    for (int j = 0; j < customers_count; ++j)
    {
        int next_customer_time = rand() % 3 + 1;
        this_thread::sleep_for(chrono::seconds(next_customer_time));
        customers_mutex.lock();
        customers.push(++i);
        //cout << "customer #" << i << "came to saloon ";
        if (hairmaker_is_free && customers.size() == 1)
        {
            cout << "[customers_flow] customer #" << i << " came to saloon"
                 << ", wakes up the hairmaker and being proccessed" << endl;
        }
        else
        {
            cout << "[customers_flow] customer #" << i << " came to saloon"
                 << ", and joins queue" << endl;
        }
        customers_mutex.unlock();
    }
    running = false;
}

int getCount()
{
    int c;
    do
    {
        cin >> c;
    } while (cout << (c > 0 ? "" : "Try again\n"), c <= 0);
    return c;
}

int main()
{
    int customers_count;
    cout << "How many customers will come today?" << endl;
    customers_count = getCount();

    thread t1(hairmaker);
    thread t2(customers_flow, customers_count);
    // this_thread::sleep_for(std::chrono::seconds(10));
    t1.join();
    t2.join();
}