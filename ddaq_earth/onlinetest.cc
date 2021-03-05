#include <future>
#include <iostream>
#include <string>

#include "zmq.hpp"
#include "zmq_addon.hpp"

int main() {
    /*
     * No I/O threads are involved in passing messages using the inproc transport.
     * Therefore, if you are using a ØMQ context for in-process messaging only you
     * can initialise the context with zero I/O threads.
     *
     * Source: http://api.zeromq.org/4-3:zmq-inproc
     */
    zmq::context_t ctx;
    //  Prepare subscriber
    zmq::socket_t online(ctx, zmq::socket_type::sub);
    online.connect("tcp://localhost:6666");
    online.set(zmq::sockopt::subscribe, "");//subcribe all

    while (1) {
        std::vector<zmq::message_t> recv_msgs;
        zmq::recv_result_t result =
          zmq::recv_multipart(online, std::back_inserter(recv_msgs));
        assert(result && "recv failed");
        int id=atoi(recv_msgs[0].to_string().erase(0,1).data());
        int* bufferno=(int*) recv_msgs[1].data();
        int* totalsize=(int*) recv_msgs[2].data();

        std::cout << "Get "<<*result<<"\t"<<id<<"\t"<<*bufferno<<"\t"<<*totalsize<<"\t"<<std::endl;
    }
}

