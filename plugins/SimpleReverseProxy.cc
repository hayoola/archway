/**
 *
 *  plugin_SimpleReverseProxy.cc
 *
 */

#include "SimpleReverseProxy.h"

using namespace drogon;
using namespace my_plugin;

void SimpleReverseProxy::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    
    LOG_DEBUG << "starting...";
    
    if (config.isMember("backends") && config["backends"].isArray()) {
        
        for (auto &backend : config["backends"]) {
            
            backendAddrs_.emplace_back(backend.asString());
        }
        
        if (backendAddrs_.empty()) {
            
            LOG_ERROR << "You must set at least one backend";
            abort();
        }
    
    } else {
        
        LOG_ERROR << "Error in configuration";
        abort();
    }
    
    pipeliningDepth_ = config.get("pipelining", 0).asInt();
    
    sameClientToSameBackend_ =
        config.get("same_client_to_same_backend", false).asBool();
    
    connectionFactor_ = config.get("connection_factor", 1).asInt();
    if (connectionFactor_ == 0 || connectionFactor_ > 100) {
        
        LOG_ERROR << "invalid number of connection factor";
        abort();
    }
    
    // Init a ThreadStorage to store the HTTPClient pointers, which we need
    //  to forward the incoming requests to them.
    // The client pointers are stored in a std::vector, which is resized here
    //  for performance purposes.
    // The `connectionFactor_` is the number of clients which we create for 
    //  each backend
    clients_.init(
        
        [this](
            std::vector<HttpClientPtr> &clients, 
            size_t ioLoopIndex) {
            
            clients.resize(backendAddrs_.size() * connectionFactor_);
        }
    );
    
    
    clientIndex_.init(
        
        [this](
            size_t &index, 
            size_t ioLoopIndex) { 
            
            index = ioLoopIndex; 
        }
    );
    
    
    drogon::app().registerPreRoutingAdvice(
        
        [this](
            const HttpRequestPtr &req,
            AdviceCallback &&callback,
            AdviceChainCallback &&pass
        ) {
        
            preRouting(req, std::move(callback), std::move(pass));
        }

    );

    LOG_DEBUG << "ends";
}

void SimpleReverseProxy::shutdown()
{
}



void SimpleReverseProxy::preRouting(
    const HttpRequestPtr &req,
    AdviceCallback &&callback,
    AdviceChainCallback &&
) {
    
    LOG_DEBUG << "====================\n" << "New request... Path: " << req->getPath();

    auto the_headers = req->getHeaders();
    
    size_t index;
    auto &clientsVector = *clients_;
    if (sameClientToSameBackend_) {
        
        // Build a unique hash based on the remote (original) requester ip 
        // and normalize it to [0-clientsVector.size()]
        index = std::hash<uint32_t>{}(req->getPeerAddr().ipNetEndian()) %
                clientsVector.size();
        
        // Now map the normalized hash value into an index of the `clientsVector`
        // Why don't we just use the hash value as the index?
        index = (index + (++(*clientIndex_)) * backendAddrs_.size()) %
                clientsVector.size();
    } else {
        
        // Round-robin over the `clientsVector`
        index = ++(*clientIndex_) % clientsVector.size();
    }
    
    auto &clientPtr = clientsVector[index];
    if (!clientPtr){
        
        auto &addr = backendAddrs_[index % backendAddrs_.size()];
        clientPtr = HttpClient::newHttpClient(
            addr, trantor::EventLoop::getEventLoopOfCurrentThread());
        
        clientPtr->setPipeliningDepth(pipeliningDepth_);
    }
    
    // We don't want to add any additional header, so enabling 
    //  the `pass-through` mode
    req->setPassThrough(true);
    
    
    LOG_DEBUG << "Before sendRequest";

    clientPtr->sendRequest(
      req,
      [callback = std::move(callback)](
          ReqResult result,
          const HttpResponsePtr &resp
      ) {
          
          if (result == ReqResult::Ok) {
              
              LOG_DEBUG << "Response received!";

              auto headers = resp->headers();
              auto the_body{ resp->body() };    
              
              resp->setPassThrough(true);
              //resp->removeHeader("transfer-encoding");
              callback(resp);
          
          } else {
              
              LOG_DEBUG << "Error received!!";
              
              auto errResp = HttpResponse::newHttpResponse();
              errResp->setStatusCode(k500InternalServerError);
              callback(errResp);
          }
      }
    );
}