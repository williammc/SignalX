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
 public:
  ~Observer() {
    for (auto& tpair : tracked_connections_)
      tpair.second->remove(tpair.first);
  }

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
  std::mutex sync_;
  std::map<SlotKey, Observer*> tracked_connections_;

 public:
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar& tracked_connections_;
  }
};

}  // namespace sigx