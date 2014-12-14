// Copyright(c) 2014 The SignalX Authors. All rights reserved.
#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>

namespace sigx {

using SlotKey = std::array < std::uintptr_t, 2> ;

/// Thread-safe observer
class Observer {
  template <typename T> friend class Signal;

  std::map<SlotKey, Observer*> tracked_connections_;

  void insert(SlotKey const& key, Observer* observer) {
    std::lock_guard<std::mutex> lock(sync_);
    tracked_connections_.emplace(key, observer);
  }

  void insert(SlotKey const& key) {
    std::lock_guard<std::mutex> lock(sync_);
    tracked_connections_.emplace(key, this);
  }

  void remove(SlotKey const& key) {
    std::lock_guard<std::mutex> lock(sync_);
    tracked_connections_.erase(key);
  }

protected:

  ~Observer() {
    for (auto& tpair : tracked_connections_)
      tpair.second->remove(tpair.first);
  }
  std::mutex sync_;
};

}  // namespace sigx