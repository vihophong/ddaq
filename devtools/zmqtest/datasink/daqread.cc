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
    zmq::context_t ctx;
#ifdef PUSHPULL
    zmq::socket_t server(ctx, zmq::socket_type::push);
#else
    zmq::socket_t server(ctx, zmq::socket_type::pub);
#endif

    server.connect("tcp://localhost:5555");

    int k=0;
    while (1) {
        server.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
        char mess[500];
        int ichar=sprintf(mess,"Message id = %i",k);
        server.send(zmq::buffer(mess,ichar));
        server.send(zmq::str_buffer("B"), zmq::send_flags::sndmore);
        ichar=sprintf(mess,"Message id = %i",k);
        std::cout<<mess<<std::endl;
        server.send(zmq::buffer(mess,ichar));
        zmq_sleep(1);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        k++;
    }
}

