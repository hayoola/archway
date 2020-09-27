/**
 * event.h
 * 
*/


#pragma once


#include <unordered_map>
#include <string>


namespace archway {

  enum class EventFields {
    kType = 1,
    kSubject,
    kData
  };

  class Event {

    public:

      Event() = delete;

      Event(
        std::string in_type,
        std::string in_subject
      ) {

        _init_vital_fields(in_type, in_subject);
      }

      
      Event(
        std::string in_type,
        std::string in_subject,
        std::string in_data
      ) {

        _init_vital_fields(in_type, in_subject);
        
        params_.emplace(EventFields::kData, in_data);
      }


      std::string Type() {
        
        std::string the_type{""};

        auto the_iter = params_.find(EventFields::kType);
        if( the_iter != params_.end()) {
          the_type = the_iter->second;
        }

        return the_type;
      }


      std::string Subject() {

        std::string the_subject{""};

        auto the_iter = params_.find(EventFields::kSubject);
        if( the_iter != params_.end()) {
          the_subject = the_iter->second;
        }

        return the_subject;
      }

      void SetData( std::string in_data ) {

        auto the_iter = params_.find(EventFields::kData);

        if( the_iter != params_.end()) {
          the_iter->second = in_data;
        
        } else {

          params_.emplace(EventFields::kData, in_data);
        }
      }


    private:

      void _init_vital_fields(
          std::string in_type,
          std::string in_subject
      ) {
          params_.emplace(EventFields::kType, in_type);
          params_.emplace(EventFields::kSubject, in_subject);
      }

      std::unordered_map<EventFields, std::string> params_;
  
  };
}