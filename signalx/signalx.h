// Copyright(c) 2014 The SignalX Authors. All rights reserved.
#pragma once
#include <functional>
#include "signalx/observer.h"

namespace sigx {

template <typename T_rv> class Signal;

template <typename T_rv, typename... Args>
class Signal<T_rv(Args...)> : private Observer {
  using FuncType = T_rv(*)(void*, Args...);

  template <typename T>
  SlotKey get_key(const FuncType& deg, typename T* obj) {
    return SlotKey{ reinterpret_cast<std::uintptr_t>(deg),
                    reinterpret_cast<std::uintptr_t>(obj) };
  }

  SlotKey get_key(const FuncType& deg) {
    return SlotKey{ reinterpret_cast<std::uintptr_t>(deg),
                    reinterpret_cast<std::uintptr_t>(nullptr) };
  }

  template <typename T>
  void insert_sfinae(const FuncType& deg, typename T::Observer* obj) {
    auto key = get_key(deg, obj);
    Observer::insert(key, obj);
    obj->insert(key, this);
  }

  template <typename T>
  void remove_sfinae(const FuncType& deg, typename T::Observer* obj) {
    SlotKey key = get_key(deg, obj);
    Observer::remove(key);
    obj->remove(key);
  }

  template <typename T>
  void insert_sfinae(const FuncType& deg, ...) {
    Observer::insert(get_key(deg));
  }

  template <typename T>
  void remove_sfinae(const FuncType& deg, ...) {
    Observer::remove(get_key(deg));
  }

public:
  // CONNECT ------------------------------------------------------------------
  template <T_rv(*fun_ptr)(Args...)>
  void connect() {
    auto key = get_key([](void*, Args... args) {
      return (*fun_ptr)(std::forward<Args>(args)...);
    });
    Observer::insert(key);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void connect(T* obj) {
    insert_sfinae<T>([](void* this_ptr, Args... args) {
      return (static_cast<T*>(this_ptr)->*mem_ptr)(std::forward<Args>(args)...);
    }, obj);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void connect(T* obj) {
    insert_sfinae<T>([](void* this_ptr, Args... args) {
      return (static_cast<T*>(this_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); }, obj);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void connect(T& obj) {
    connect<T, mem_ptr>(std::addressof(obj));
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void connect(T& obj) {
    connect<T, mem_ptr>(std::addressof(obj));
  }

  // DISCONNECT ---------------------------------------------------------------
  template <T_rv(*fun_ptr)(Args...)>
  void disconnect() {
    auto key = get_key([](void* /*NULL*/, Args... args) {
      return (*fun_ptr)(std::forward<Args>(args)...);
    });
    Observer::remove(key);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void disconnect(T* obj) {
    remove_sfinae<T>([](void* this_ptr, Args... args) {
      return (static_cast<T*>(this_ptr)->*mem_ptr)
        (std::forward<Args>(args)...); }, obj);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void disconnect(T* obj) {
      remove_sfinae<T>([](void* this_ptr, Args... args) {
        return (static_cast<T*>(this_ptr)->*mem_ptr)
          (std::forward<Args>(args)...); }, obj);
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...)>
  void disconnect(T& obj) {
    disconnect<T, mem_ptr>(std::addressof(obj));
  }

  template <typename T, T_rv(T::*mem_ptr)(Args...) const>
  void disconnect(T& obj) {
    disconnect<T, mem_ptr>(std::addressof(obj));
  }

  // EMIT ---------------------------------------------------------------------
  void operator() (Args... args) {
    for (auto const& slot : tracked_connections_) {
      reinterpret_cast<FuncType>(slot.first[0])(reinterpret_cast<void*>(slot.first[1]), std::forward<Args>(args)...);
    }
  }
  template <typename Accumulator>
  void operator() (Args... args, Accumulator sink) {
    for (auto const& slot : tracked_connections_) {
      sink(reinterpret_cast<FuncType>(slot.first[0])(reinterpret_cast<void*>(slot.first[1]), std::forward<Args>(args)...));
    }
  }
};

} // namespace sigx