// Copyright(c) 2014 The SignalX Authors. All rights reserved.
#pragma once
#include <functional>
#include "signalx/observer.h"

namespace sigx {


template <typename T_rv> class Function;

/// used to generate static functions
template <typename T_rv, typename... Args>
class Function<T_rv(Args...)> {
  template <typename T> friend class Signal;

  using FuncType = T_rv(*)(void*, Args...);

  void* this_ptr_; // instance pointer
  FuncType stub_ptr_; // free function pointer

  template <typename T, typename F>
  Function(T&& this_ptr, F&& stub_ptr) :
    this_ptr_{ std::forward<T>(this_ptr) },
    stub_ptr_{ std::forward<F>(stub_ptr) } {}

  /*Function (void* this_ptr, FuncType stub_ptr):
  this_ptr_ { this_ptr }, stub_ptr_ { stub_ptr } {}*/

  Function(SlotKey const& _k) :
    this_ptr_{ reinterpret_cast<void*>(std::get<0>(_k)) },
    stub_ptr_{ reinterpret_cast<FuncType>(std::get<1>(_k)) } {}

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

  inline operator SlotKey() const {
    return{
      reinterpret_cast<std::uintptr_t>(this_ptr_),
      reinterpret_cast<std::uintptr_t>(stub_ptr_)
    };
  }
};

// Signals class ==============================================================
template <typename T_rv> class Signal;

template <typename T_rv, typename... Args>
class Signal<T_rv(Args...)> : private Observer {
  template <typename T>
  void insert_sfinae(const SlotKey& key, typename T::Observer* obj) {
    Observer::insert(key, obj);
    obj->insert(key, this);
  }

  template <typename T>
  void remove_sfinae(const SlotKey& key, typename T::Observer* obj) {
    Observer::remove(key);
    obj->remove(key);
  }

  template <typename T>
  void insert_sfinae(const SlotKey& key, ...) {
    Observer::insert(key);
  }

  template <typename T>
  void remove_sfinae(const SlotKey& key, ...) {
    Observer::remove(key);
  }

public:
  using Delegate = Function<T_rv(Args...)>;
  // CONNECT ------------------------------------------------------------------
  template <T_rv(*fun_ptr)(Args...)>
  void connect() {
    Observer::insert(Delegate::template bind<fun_ptr>());
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void connect(T* instance) {
    auto delegate = Delegate::template bind<T, mem_ptr>(instance);
    insert_sfinae<T>(delegate, instance);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void connect(T* instance) {
    auto delegate = Delegate::template bind<T, mem_ptr>(instance);
    insert_sfinae<T>(delegate, instance);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void connect(T& instance) {
    connect<T, mem_ptr>(std::addressof(instance));
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void connect(T& instance) {
    connect<T, mem_ptr>(std::addressof(instance));
  }

  // DISCONNECT ---------------------------------------------------------------
  template <T_rv(*fun_ptr)(Args...)>
  void disconnect() {
    Observer::remove(Delegate::template bind<fun_ptr>());
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void disconnect(T* instance) {
    auto delegate = Delegate::template bind<T, mem_ptr>(instance);
    remove_sfinae<T>(delegate, instance);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void disconnect(T* instance) {
    auto delegate = Delegate::template bind<T, mem_ptr>(instance);
    remove_sfinae<T>(delegate, instance);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void disconnect(T& instance) {
    disconnect<T, mem_ptr>(std::addressof(instance));
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void disconnect(T& instance) {
    disconnect<T, mem_ptr>(std::addressof(instance));
  }

  // EMIT ---------------------------------------------------------------------
  void operator() (Args... args) {
    for (auto const& slot : tracked_connections_) {
      Delegate(std::get<0>(slot))(std::forward<Args>(args)...);
    }
  }
  template <typename Accumulator>
  void operator() (Args... args, Accumulator sink) {
    for (auto const& slot : tracked_connections_) {
      sink(Delegate(std::get<0>(slot))(std::forward<Args>(args)...));
    }
  }
};

} // namespace sigx