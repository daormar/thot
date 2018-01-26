/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file ThreadSafePrint.h
 * 
 * @brief Declares the ThreadSafePrint class for logging in thread-safe
 * manner to avoid racing. It uses singleton pattern to provide mutual
 * exclusive printing.
 */

#ifndef _ThreadSafePrint_h
#define _ThreadSafePrint_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- ThreadSafePrint template class

/**
 * @brief Class implementing thread-safe printing
 */

class ThreadSafePrint
{
public:
    static ThreadSafePrint& getInstance(void)
    {
        static ThreadSafePrint instance;

        return instance;
    }

    void print(std::string s)
    {
        pthread_mutex_lock(&mutex);
        std::cerr << s;
        pthread_mutex_unlock(&mutex);
    }

    void printWithThreadId(std::string s)
    {
        // Make thread id convertion before entering to critical section
        std::string tid = selfThreadIdToStr();
        pthread_mutex_lock(&mutex);
        std::cerr << tid << "\t" << s;
        pthread_mutex_unlock(&mutex);
    }

private:
    pthread_mutex_t mutex;
    ThreadSafePrint()
    {
        pthread_mutex_init(&mutex, NULL);
    }

    std::string selfThreadIdToStr(void)
    {
        // Convert pthread_t structure to string by encoding its content in hex
        // format. Such representation should be portable among different
        // platforms.
        pthread_t pt = pthread_self();
        std::stringstream ss;

        unsigned char *ptc = (unsigned char*) (void*) (&pt);
        ss << "0x";
        for (size_t i = 0; i < sizeof(pt); i++) {
            ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned) (ptc[i]);
        }

        return ss.str();
    }

    ~ThreadSafePrint()
    {
        pthread_mutex_destroy(&mutex);
    }

    // Block possibility to create copy of this class
    // by hiding following methods
    ThreadSafePrint(ThreadSafePrint const&);
    void operator=(ThreadSafePrint const&);
};

#endif
