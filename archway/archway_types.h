/**
 *
 *  archway_types.h
 * 
 *  Archway general types
 *
*/

#pragma once

namespace archway {

  /**
   * The code, which returns from archway::Functions
  */
  enum class Action {
    kFail,    // Fatal error: deliver a short error response ==> Redundant (we can return error response)
    kError,   // Transition to kBackendError stage, which can deliver descriptive errors ==> Redundant
    kPass,
    kPipe,
    kHash,
    kPurge,
    kRestart,
    kFetch,
    kDeliver
  };



  /**
   * The definition of different stages during the request's journey from admission 
   * into the proxy to departure from it and head to the client.
   * Corresponding to each stage, an `Archway::Function` can be defined to
   * catch the request or response and processes it.
  */
  enum Stage {
    kReceive = 0,
    kPipe,
    kPass,
    kHash,
    kPurge,
    kMiss,
    kHit,
    kBackendFetch,
    kBackendResponse,
    kBackendError,
    kDeliver
  };

}