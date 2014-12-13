// Copyright(c) 2014 The SignalX Authors. All rights reserved.
#pragma once
#include <cstdint>
#include <array>

namespace sigx {

using DelegateKey = std::array < std::uintptr_t, 2 >;

template <typename T_rv> class Function;

template <typename T_rv, typename... Args>
class Function < T_rv(Args...) > {
  template <typename T> friend class Signal;

  using Thunk = T_rv(*)(void*, Args...);

  void* this_ptr_;  ///< instance pointer
  Thunk stub_ptr_;  ///< free function pointer

  template <typename T, typename F>
  Function(T&& this_ptr, F&& stub_ptr) :
    this_ptr_{ std::forward<T>(this_ptr) },
    stub_ptr_{ std::forward<F>(stub_ptr) } {}

  /*Function (void* this_ptr, Thunk stub_ptr):
      this_ptr_ { this_ptr }, stub_ptr_ { stub_ptr } {}*/

  Function(DelegateKey const& k) :
    this_ptr_{ reinterpret_cast<void*>(std::get<0>(k)) },
    stub_ptr_{ reinterpret_cast<Thunk>(std::get<1>(k)) } {}

 public:
  template <T_rv(*fun_ptr)(Args...)>
  static inline Function bind() {
    return{ nullptr, [](void* /*NULL*/, Args... args) {
      return (*fun_ptr)(std::forward<Args>(args)...); } };
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  static inline Function bind(T* pointer) {
    return{ pointer, [](void* this_ptr, Args... args) {
      return (static_cast<T*>(this_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); } };
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  static inline Function bind(T* pointer) {
    return{ pointer, [](void* this_ptr, Args... args) {
      return (static_cast<T*>(this_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); } };
  }

  inline T_rv operator() (Args... args) {
    return (*stub_ptr_)(this_ptr_, std::forward<Args>(args)...);
  }

  inline operator DelegateKey() const {
    return{
      reinterpret_cast<std::uintptr_t>(this_ptr_),
      reinterpret_cast<std::uintptr_t>(stub_ptr_)
    };
  }
};
}  // namespace sigx