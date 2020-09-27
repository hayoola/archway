/**
 * 
 * is_valid.h
 * 
 * A re-implementation of boost::hana:is_valid in prospect to avoid
 *  the dependency on boost.
 * Based on [https://jguegant.github.io/blogs/tech/sfinae-introduction.html]
 * 
 **/ 

#pragma once

#include <type_traits>

namespace archway {

  template <typename UnnamedType> struct container {
    // Let's put the test in private.
    private:
      // We use std::declval to 'recreate' an object of 'UnnamedType'.
      // We use std::declval to also 'recreate' an object of type 'Param'.
      // We can use both of these recreated objects to test the validity!
      // The first part of decltype is used by SFINAE, but the second part
      //  ( std::true_type() ) explicitly defines the return type this
      //  templated function, IF the first part doesn't fail.
      template <typename Param> 
      constexpr auto test_validity(int /* unused */)
        -> decltype(std::declval<UnnamedType>()(std::declval<Param>()), std::true_type()) {
        
        // If substitution didn't fail, we can return a true_type.
        return std::true_type();
      }

      template <typename Param> 
      constexpr std::false_type test_validity(...) {
        
        // Our sink-hole returns a false_type.
        return std::false_type();
      }

    public:
      // A public operator() that accept the argument we wish to test onto the UnnamedType.
      // Notice that the return type is automatic!
      template <typename Param> 
      constexpr auto operator()(const Param& p) {
        
        // The argument is forwarded to one of the two overloads.
        // The SFINAE on the 'true_type' will come into play to dispatch.
        //  int() is a type initializer 
        return test_validity<Param>(int());
      }
  };



  // Here the UnnamedType is of closure (lambda) type, by which we
  //  create a specialization of the 'container' template struct
  template <typename UnnamedType> 
  constexpr auto is_valid(const UnnamedType& t) {
    
    return container<UnnamedType>();
  }

  // Check if a type has a YamlNodeName method.
  // Note 'is_valid()' is a templeted function, which its template type
  //  will be deduced from the function argument, so we call it like 
  //  a normal function
  // Also note the parameter of the lambda is decleared 'auto', which makes
  //  an internally templeted function object to represent the lambda.
  //  So, the type of 'x' will be deduced by the compiler
  //
  // Non-static member example: (should be called as HasYamlNodeName(theObj))
  // auto HasYamlNodeName = is_valid([](auto&& x) -> decltype(x.YamlNodeName()) {});
  //
  //
  // Static member example: (should be called as HasStaticYName(type_c<MyClass>))
  // auto HasStaticYName = is_valid([](auto t) -> decltype( decltype(t)::type::YName()) {});

  
  // A utility constrict to wrap a type into values like type_c<MyClass>
  //  It enables us to check member validation of a type without instantiation.
  //  Inspired by boost::hana
  template <typename T>
  struct type_w
  {
      using type = T;
  };

  template <typename T>
  constexpr type_w<T> type_c{};
}