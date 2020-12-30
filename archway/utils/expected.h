
/**
 * expected.h
 * 
 * Functional error and exception handling based on the talk:
 *  C++ and Beyond 2012: Systematic Error Handling in C++, Andrei Alexandrescu
 *  
 * I decided to not store the std::excpetion inside this object, because the `what()`
 *  string will get lost, and it is quite expensive to infer the type of 
 *  an excpetion during runtime. So I just store the result of what()
 * 
 * TODO: Add constructors for std::logic_error and std::runtime_error and
 * TODO:  a 'fatal() -> bool' method
*/

#pragma once


#include <exception>
#include <stdexcept>
#include <atomic>

namespace archway {


  /**
   * Through this class we can recored more detailed info
   *  about the compilation error, like where().
   * Then via a specialized constructor in the `Expected` object
   *  we can access these info and ...!
  */
  class CompileError : public std::logic_error {

    public:

      CompileError(const std::string& in_description) :
      std::logic_error(in_description) {

      }
  };

  
  class RoutingError : public std::logic_error {

    public:
      
      RoutingError(const std::string& in_description) :
      std::logic_error(in_description) {
      
      }
  };


  class CacheError : public std::logic_error {

    public:

      CacheError(const std::string& in_description) :
      std::logic_error(in_description) {

      }
  };

	
	template<class T>
	class Expected {
	  protected:
      union {
        T result_;
        std::string error_;
      };

      bool gotResult_;						// true iff valid result is available
      Expected() {};							// private constructor

	  public:
      
      // constructors and destructor
      Expected(const T& r) : result_(r), gotResult_(true) {}
      
      Expected(T&& r) : result_(std::move(r)), gotResult_(true) {}
      
      Expected(const Expected& e) : gotResult_(e.gotResult_) {
        
        if (gotResult_) {
          //new(&result_) T(e.result_);
          result_ = e,result_;
        
        } else {
          //new(&error_) std::string(e.error_);
          error_ = e.error_;
        }
      }
      
      Expected(Expected&& e) : gotResult_(e.gotResult_) {
        
        if (gotResult_) {
          new(&result_) T(std::move(e.result_));
        
        } else {
          
          new(&error_) std::string(std::move(e.error_));
        }
      }
		  
      ~Expected() {}

      // creating expect from exceptions
      
      Expected<T>(std::exception const& e) : 
      error_(e.what()), gotResult_(false) { 

      }

      
      Expected<T>(CompileError const& e) : 
      error_(e.what()), gotResult_(false) { 

        // TODO: Extract and store specialized info, like where()
      }


      Expected<T>(RoutingError const& e) : 
      error_(e.what()), gotResult_(false) { 

        // TODO: Extract and store specialized info, like where()
      }

      // operator overload
      Expected<T>& operator=(const Expected<T>& e) {
        
        gotResult_ = e.gotResult_;
        if (gotResult_)
          new(&result_) T(e.result_);
        else
          new(&error_) std::string(e.error_);
        return *this;
      }

		  // getters
		  bool isValid() const { return gotResult_; };
		  
      bool wasSuccessful() const { return gotResult_; };

      T& get() {
        
        if (!gotResult_)
          throw std::logic_error("Can't get a result from an errornous Expected object");
        return result_;
      }
		
      const T& get() const {
        if (!gotResult_)
          throw std::logic_error("Can't get a result from an errornous Expected object");
        return result_;
      }


      const std::string & error() { 
      return error_; 
    }

      

		friend class Expected<void>;
	};

	
  // void specialization
  template<>
	class Expected<void> {
		
    std::string error_;
    bool gotResult_;

	public:
		
    // constructors and destructor
		
    Expected(const std::exception& e) : 
    error_(e.what()), gotResult_(false) {
      
    }


    Expected(const CompileError& e) : 
    error_(e.what()), gotResult_(false) {
      
      //TODO: Record specialized info
    }


    Expected(const RoutingError& e) : 
    error_(e.what()), gotResult_(false) {
      
      //TODO: Record specialized info
    }
		
    template<typename T>
		Expected(const Expected<T>& e) {
			
      if (!e.gotResult_) {
        error_ = e.error_;
      }
				
		}

		Expected(Expected&& o) : 
      error_(std::move(o.error_)) , gotResult_(std::move(o.gotResult_)) { 

    }
		
    Expected() : error_({""}), gotResult_(true) {
      
      int my_dummy{-1};
    }

		// operator overload
		Expected& operator=(const Expected& e) {
			
      this->gotResult_ = e.gotResult_;

      if (!e.isValid()) {
        this->error_ = e.error_;
      }
				
			return *this;
		};

		// getters
		bool isValid() const { 
      return gotResult_; 
    }
		
    bool wasSuccessful() const { 
      return gotResult_; 
    }
		
    void get() const { 
      if (!isValid()) throw error_; 
    }

    const std::string & error() { 
      return error_; 
    }
		
    void suppress() {}

	};
}