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
    online.set(zmq::sockopt::subscribe, "A");

    while (1) {
        std::vector<zmq::message_t> recv_msgs;
        zmq::recv_result_t result =
          zmq::recv_multipart(online, std::back_inserter(recv_msgs));
        assert(result && "recv failed");
        assert(*result == 2);
        std::cout << "online thread 1: [" << recv_msgs[0].to_string() << "] "
                  << recv_msgs[1].to_string() << std::endl;
    }
}

