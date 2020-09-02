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
   * The type code which is used in the Action struct
  */
  enum class ActionType {
    FAIL,
    ERROR,
    SYNTH,
    PASS,
    PIPE,
    HASH,
    PURGE,
    RESTART,
    FETCH,
    ABONDON
  };

  /**
   * The return type of StageFunctions
  */ 
  struct Action {
    ActionType  type;
    short       status_code;
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
    kDeliver,
    // kSynth,
    kBackendFetch,
    kBackendResponse
  };

}