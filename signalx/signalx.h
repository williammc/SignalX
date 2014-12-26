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

  void* obj_ptr_; // instance obj
  FuncType func_ptr_; // free function obj

  template <typename T, typename F>
  Function(T&& obj_ptr, F&& func_ptr) :
    obj_ptr_{ std::forward<T>(obj_ptr) },
    func_ptr_{ std::forward<F>(func_ptr) } {}

  /*Function (void* obj_ptr, FuncType func_ptr):
  obj_ptr_ { obj_ptr }, func_ptr_ { func_ptr } {}*/

  Function(SlotKey const& _k) :
    obj_ptr_{ reinterpret_cast<void*>(std::get<0>(_k)) },
    func_ptr_{ reinterpret_cast<FuncType>(std::get<1>(_k)) } {}

public:

  template <T_rv(*func_ptr)(Args...)>
  static inline Function bind() {
    return{ nullptr, [](void* /*NULL*/, Args... args) {
      return (*func_ptr)(std::forward<Args>(args)...); } };
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  static inline Function bind(T* obj) {
    return{ obj, [](void* obj_ptr, Args... args) {
      return (static_cast<T*>(obj_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); } };
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  static inline Function bind(T* obj) {
    return{ obj, [](void* obj_ptr, Args... args) {
      return (static_cast<T*>(obj_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); } };
  }

  inline T_rv operator() (Args... args) {
    return (*func_ptr_)(obj_ptr_, std::forward<Args>(args)...);
  }

  inline operator SlotKey() const {
    return{
      reinterpret_cast<std::uintptr_t>(obj_ptr_),
      reinterpret_cast<std::uintptr_t>(func_ptr_)
    };
  }
};

// Signals class ==============================================================
template <typename T_rv> class Signal;

template <typename T_rv, typename... Args>
class Signal<T_rv(Args...)> : public Observer {
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
  template <T_rv(*func_ptr)(Args...)>
  void connect() {
    Observer::insert(Delegate::template bind<func_ptr>());
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
  template <T_rv(*func_ptr)(Args...)>
  void disconnect() {
    Observer::remove(Delegate::template bind<func_ptr>());
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
      //Delegate(std::get<0>(slot))(std::forward<Args>(args)...);  // this cause move for smart_ptr => args become emptry in the 2+ iterations
      Delegate(std::get<0>(slot))(args...);
    }
  }
  template <typename Accumulator>
  void operator() (Args... args, Accumulator sink) {
    for (auto const& slot : tracked_connections_) {
      //sink(Delegate(std::get<0>(slot))(std::forward<Args>(args)...));
      sink(Delegate(std::get<0>(slot))(args...));
    }
  }
};

} // namespace sigx