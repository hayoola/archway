/**
 * event_listener.h
*/

#pragma once

#include "event.h"

namespace archway {

  class EventListener {

    public:

      virtual int HandleEvent( const Event& in_event) = 0;

  };
}