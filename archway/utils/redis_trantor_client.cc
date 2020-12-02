/**
 * redis_trantor_client.cpp
 * 
*/

#include "redis_trantor_client.h"
#include <drogon/HttpAppFramework.h>
#include <thread>


using namespace archway;



void RedisTrantorClient::connect(
  const std::string& in_addr, 
  std::uint32_t in_port, 
  std::uint32_t in_timeout_ms
) {

  if( is_connected_ ) {
    std::cout << "RedisTrantorClient::connect: Is has been already connected!\n";
    return;
  }
  
  
  loop_ = loop_ == nullptr ? drogon::app().getLoop() : loop_;
  address_ = trantor::InetAddress(in_addr, in_port);
  
  tcp_client_ = std::make_shared<trantor::TcpClient>(
    loop_,
    address_,
    "RedisClient"
  );

  auto this_ptr = shared_from_this();
  std::weak_ptr<RedisTrantorClient> the_weak_this = this_ptr;
  was_connection_attempt_done_ = false;

  tcp_client_->setConnectionCallback(
    [the_weak_this] (const trantor::TcpConnectionPtr &in_connection_ptr) {

      auto this_ptr = the_weak_this.lock();
      if( !this_ptr ) {
        return;
      }

      if( in_connection_ptr->connected() ) {
        std::cout << "RedisTrantorClient: TCP connection established!\n";
        this_ptr->is_connected_ = true;
      
      } else {

        std::cout << "RedisTrantorClient: TCP connection failure!\n";
        this_ptr->is_connected_ = false;   
      }

      this_ptr->was_connection_attempt_done_ = true;
      this_ptr->connecting_condition_.notify_one();
    }
  );

  tcp_client_->setConnectionErrorCallback(
    [the_weak_this] () {

      // We can't connect to the Redis instance
      std::cout << "RedisTrantorClient: TCP connection failure callback!\n";

      auto this_ptr = the_weak_this.lock();
      if( !this_ptr ) {
        return;
      }

      this_ptr->is_connected_ = false; 
      this_ptr->was_connection_attempt_done_ = true;
      this_ptr->connecting_condition_.notify_one();

    }
  );

  tcp_client_->setMessageCallback(
    [the_weak_this](const trantor::TcpConnectionPtr &in_connection_ptr,
      trantor::MsgBuffer *in_msg_buffer) {
      
      auto this_ptr = the_weak_this.lock();
      if( !this_ptr ) {
        return;
      }

      this_ptr->on_receive_message(in_connection_ptr, in_msg_buffer);

    }
  );


  tcp_client_->setWriteCompleteCallback(
    [the_weak_this](const trantor::TcpConnectionPtr& in_connection_ptr) {

      std::cout << "RedisTrantorClient: Write completion callback!\n";

      auto this_ptr = the_weak_this.lock();
      if( !this_ptr ) {
        return;
      }
    }
  );

  tcp_client_->connect();

  // Since this member function is blocking (WHAT ?!),
  //  we have to wait here for the connection attempt to get completed
  std::unique_lock<std::mutex> the_lock(waiting_mutex_);
  connecting_condition_.wait( the_lock, [this] {
    return was_connection_attempt_done_;
  });

}







void RedisTrantorClient::on_receive_message(
  const trantor::TcpConnectionPtr& in_connection_ptr, 
  trantor::MsgBuffer* in_message_buffer
) {

  std::cout << "RedisTrantorClient::on_receive_message start...\n";

  do {

    if( last_read_request_.size == 0 || last_read_request_.async_read_callback == nullptr ) {
    
      std::cout << "RedisTrantorClient::on_receive_message: No previous read request!\n";
      break;
    }

    auto the_message_size = in_message_buffer->readableBytes();
    if( the_message_size == 0 ) {
      
      std::cout << "RedisTrantorClient::on_receive_message: No readable byte!\n";
      break;
    }

    size_t the_size_to_read = the_message_size > last_read_request_.size ?
      last_read_request_.size : the_message_size
    ;

    // We construct a vector by COPYING input buffer!
    // TODO Find a more efficient method
    read_result the_read_result {
      true,
      std::vector<char>(
        &(*in_message_buffer)[0],
        &(*in_message_buffer)[the_size_to_read]
      )
    };
    

    in_message_buffer->retrieve( the_message_size);

    last_read_request_.async_read_callback( the_read_result);

  } while( false );

}




void RedisTrantorClient::disconnect(
  bool in_wait_for_removal
) {

  tcp_client_->disconnect();
}




void RedisTrantorClient::async_read(
  read_request& in_request
) {

  // The in_request.size is the max size to read
  std::cout << "RedisTrantorClient::async_read start... size: " << in_request.size << "\n";

  last_read_request_.size = in_request.size;
  last_read_request_.async_read_callback = in_request.async_read_callback;

  //TODO If the read buffer wasn't drained, send the remaining bytes here

}




void RedisTrantorClient::async_write(
  write_request& in_request
) {

  std::cout << "RedisTrantorClient::async_write start...\n";

  do {

    auto the_connection = tcp_client_->connection();
    if( ! the_connection || !(the_connection->connected()) ) {

      std::cout << "RedisTrantorClient::async_write: There is no valid connection!\n";
      break;
    }
  
    trantor::MsgBuffer the_buffer;
    the_buffer.append( &in_request.buffer[0], in_request.buffer.size());

    the_connection->send( the_buffer);

  } while( false );
  

}




void RedisTrantorClient::set_on_disconnection_handler(
  const disconnection_handler_t& disconnection_handler
) {


}