/**
 * redis_trantor_client.h
 * 
 * A TCP client adapter for the cpp_redis library using trantor as the
 *  non-blocking connection engine
*/

#pragma once


#include <cpp_redis/network/tcp_client_iface.hpp>
#include <trantor/net/TcpClient.h>
#include <trantor/net/EventLoop.h>
#include <condition_variable>


namespace archway {

  class RedisTrantorClient : public cpp_redis::network::tcp_client_iface,
    public std::enable_shared_from_this<RedisTrantorClient> {

    public:

      RedisTrantorClient() :
        tcp_client_(nullptr),
        loop_(nullptr),
        is_connected_(false),
        last_read_request_({0, nullptr}) {

      }

      ~RedisTrantorClient() override = default;

      /**
       * start the tcp client
       *
       * @param in_addr host to be connected to
       * @param in_port port to be connected to
       * @param in_timeout_ms max time to connect in ms
       *
      */
      void connect(
        const std::string& in_addr, 
        std::uint32_t in_port, 
        std::uint32_t in_timeout_ms
      ) override;


      /**
       * stop the tcp client
       *
       * @param in_wait_for_removal when sets to true, disconnect blocks until the 
       *  underlying TCP client has been effectively removed from the io_service 
       *  and that all the underlying callbacks have completed.
       *
      */
      void disconnect(
        bool in_wait_for_removal = false
      ) override;


      /**
       * @return whether the client is currently connected or not
       *
      */
      bool is_connected() const override {
        return is_connected_;
      }


      /**
       * async read operation
       *
       * @param in_request information about what should be read and what should 
       *  be done after completion
       *
      */
      void async_read(
        read_request& in_request
      ) override;


      /**
       * async write operation
       *
       * @param in_request information about what should be written and what should 
       *  be done after completion
       *
      */
      void async_write(
        write_request& in_request
      ) override;


      void set_on_disconnection_handler(
        const disconnection_handler_t& disconnection_handler
      ) override;

    private:

      void on_receive_message(const trantor::TcpConnectionPtr &, trantor::MsgBuffer *);

      std::shared_ptr<trantor::TcpClient> tcp_client_;
      trantor::EventLoop *loop_;
      trantor::InetAddress address_;
      bool is_connected_;

      read_request last_read_request_;

      std::mutex waiting_mutex_;
      std::condition_variable connecting_condition_;
      bool  was_connection_attempt_done_;
  };

}

  