#include <future>
#include <iostream>
#include <string>
#include <stdlib.h>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#define PUSHPULL 1

int main(int argc, char *argv[]) {
    /*
     * No I/O threads are involved in passing messages using the inproc transport.
     * Therefore, if you are using a ØMQ context for in-process messaging only you
     * can initialise the context with zero I/O threads.
     *
     * Source: http://api.zeromq.org/4-3:zmq-inproc
     */

    //! recieved data from daqread
    zmq::context_t ctx;
#ifdef PUSHPULL
    zmq::socket_t datasinkget(ctx, zmq::socket_type::pull);
#else
    zmq::socket_t datasinkget(ctx, zmq::socket_type::sub);
#endif
    datasinkget.bind("tcp://*:5555");
#ifndef PUSHPULL
    datasinkget.set(zmq::sockopt::subscribe, "A");
#endif

    //! send to online
    //zmq::context_t ctx2;
    zmq::socket_t datasinksend(ctx, zmq::socket_type::pub);
    datasinksend.bind("tcp://*:6666");

    zmq::pollitem_t items[]={
        { datasinkget, 0, ZMQ_POLLIN, 0 },
               { datasinksend,  0, ZMQ_POLLIN, 0 }
    };

    while (1) {
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            std::vector<zmq::message_t> recv_msgs;
            zmq::recv_result_t result =
              zmq::recv_multipart(datasinkget, std::back_inserter(recv_msgs));
            assert(result && "recv failed");
            assert(*result == 2);
            std::cout << " datasinkget received from port 4444: [" << recv_msgs[0].to_string() << "] "
                                 << recv_msgs[1].to_string() << std::endl;
                       datasinksend.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
                       datasinksend.send(zmq::buffer(recv_msgs[1].data(),recv_msgs[1].size()),zmq::send_flags::dontwait);
                       std::cout << "-datasinkget sent (port 4444)!" <<std::endl;
        }
    }


}

