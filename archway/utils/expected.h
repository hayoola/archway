
/**
 * expected.h
 * 
 * Functional error and exception handling based on the talk:
 *  C++ and Beyond 2012: Systematic Error Handling in C++, Andrei Alexandrescu
 *  
 * In the first version, all I wanted was simpilicity, so no space optimization
 *  were made!
 * 
 * TODO: Investigate the cost of copying std::exception objects
*/

#pragma once


#include <exception>
#include <stdexcept>
#include <atomic>

namespace archway {
	
	template<class T>
	class Expected {
	  protected:
      union {
        T result_;
        std::exception error_;
      };

      bool gotResult_;							// true iff valid result is available
      Expected() {};							// private constructor

	  public:
      
      // constructors and destructor
      Expected(const T& r) : result_(r), gotResult_(true) {}
      
      Expected(T&& r) : result_(std::move(r)), gotResult_(true) {}
      
      Expected(const Expected& e) : gotResult_(e.gotResult_) {
        
        if (gotResult_)
          new(&result_) T(e.result_);
        else
          new(&error_) std::exception(e.error_);
      }
      
      Expected(Expected&& e) : gotResult_(e.gotResult_) {
        
        if (gotResult_)
          new(&result_) T(std::move(e.result_));
        else
          new(&error_) std::exception(std::move(e.error_));
      }
		  
      ~Expected() {}

      // creating expect from exceptions
      Expected<T>(std::exception const& e) : error_(e), gotResult_(false) { }

      /*
      template<class E>
      static Expected<T> fromException(const E& exception) {
        
        if (typeid(exception) != typeid(E))
          throw std::invalid_argument("slicing detected!\n");
        return fromException(exception);
      }
		
      static Expected<T> fromException(std::exception p) {
        Expected<T> e;
        e.gotResult_ = false;
        new(&e.error_) std::exception(std::move(p));
        return e;
      }
      */

      // operator overload
      Expected<T>& operator=(const Expected<T>& e) {
        
        gotResult_ = e.gotResult_;
        if (gotResult_)
          new(&result_) T(e.result_);
        else
          new(&error_) std::exception(e.error_);
        return *this;
      }

		  // getters
		  bool isValid() const { return gotResult_; };
		  
      bool wasSuccessful() const { return gotResult_; };

      T& get() {
        
        if (!gotResult_)
          throw error_;
        return result_;
      }
		
      const T& get() const {
        if (!gotResult_)
          throw error_;
        return result_;
      }

      // probe for exception
      template<class E>
      bool hasException() const {
        try {
          
          if (!gotResult_)
            throw error_;
        
        } catch (const E& object) {
          (void)object;
          return true;
        
        } catch (...) {

        }

        return false;
      }

		friend class Expected<void>;
	};

	
  // void specialization
  template<>
	class Expected<void> {
		
    std::exception error_;
    bool gotResult_;

	public:
		
    // constructors and destructor
		Expected(std::exception const& e) : error_(e), gotResult_(false) {
      int my_dummy{-1};
    }
		
    template<typename T>
		Expected(const Expected<T>& e) {
			if (!e.gotResult_)
				new(&error_) std::exception(e.error_);
		}

		Expected(Expected&& o) : error_(std::move(o.error_)) , gotResult_(std::move(o.gotResult_)) { }
		
    Expected() : error_(), gotResult_(true) {
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
		bool isValid() const { return gotResult_; }
		
    bool wasSuccessful() const { return gotResult_; }
		
    void get() const { if (!isValid()) throw error_; }

    const std::exception& error() { return error_; }
		
    void suppress() {}

	};
}