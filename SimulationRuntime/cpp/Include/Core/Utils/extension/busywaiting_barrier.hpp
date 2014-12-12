/*
 * busywaitingbarrier.hpp
 *
 *  Created on: 03.11.2014
 *      Author: marcus
 */

#ifndef BUSYWAITINGBARRIER_HPP_
#define BUSYWAITINGBARRIER_HPP_

#ifdef USE_BOOST_THREAD

#include <boost/atomic.hpp>
#include <iostream>
#include <Modelica.h>

class spinlock
{
    //boost::atomic_flag flag;
    volatile bool locked;
public:
    spinlock() : locked(false) {}

    ~spinlock() {}

    FORCE_INLINE void lock()
    {
        //while(flag.test_and_set(boost::memory_order_acquire));
        while(locked) {}
        locked = true;
    }

    FORCE_INLINE void unlock()
    {
        //flag.clear(boost::memory_order_release);
        locked = false;
    }
};

class busywaiting_barrier
{
 public:
    busywaiting_barrier(int counterValueMax) : counterValue(counterValueMax), counterValueRelease(0), ready(true), counterValueMax(counterValueMax) {}
    ~busywaiting_barrier() {}

    FORCE_INLINE inline void wait()
    {
        //std::cerr << "entering wait function (counterValueMax: " << counterValueMax << ")" << std::endl;
        while(!ready) {}

        bool reset = (counterValue.fetch_sub(1,boost::memory_order_release ) == 1); //decrement counter value
        if(reset)
        {
            //std::cerr << "ready state set to false (counterValueMax: " << counterValueMax << ")" << std::endl;
            ready = false;
        }

        //std::cerr << "counter decremented (counterValueMax: " << counterValueMax << ")" << std::endl;

        while(counterValue.load(boost::memory_order_relaxed) > 0)
        {
            //int val = counterValue.load(boost::memory_order_seq_cst );
            //std::cerr << "waiting because counter value is " << val << " (counterValueMax: " << counterValueMax << ")" << std::endl;
            //sleep(1);
        }

        //std::cerr << "leaving wait function (counterValueMax: " << counterValueMax << ")" << std::endl;

        if(counterValueRelease.fetch_add(1,boost::memory_order_release) == counterValueMax-1)
        {
            counterValue.store(counterValueMax, boost::memory_order_release);
            counterValueRelease.store(0, boost::memory_order_release);
            ready = true;

            //std::cerr << "set ready to true (counterValueMax: " << counterValueMax << ")" << std::endl;
        }

        //while(counterValueRelease.load(boost::memory_order_acquire ) > 0) {}
        while(counterValueRelease.load(boost::memory_order_relaxed ) > 0) {}
    }

 private:
    boost::atomic<int> counterValue;
    boost::atomic<int> counterValueRelease;
    volatile bool ready;
    int counterValueMax;
};

#endif //USE_BOOST_THREAD

#endif /* BUSYWAITINGBARRIER_HPP_ */
