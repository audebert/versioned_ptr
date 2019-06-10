#pragma once

#include <forward_list>
#include <memory>

template <typename T> class VersionedPtr {
public:
  VersionedPtr(std::unique_ptr<T> root) : current_(std::move(root)) {}

  void save() { versions_.push_front(std::unique_ptr<T>(new T(*current_))); }
  bool can_restore() const { return versions_.cbegin() != versions_.cend(); }
  void restore() {
    current_.swap(versions_.front());
    versions_.pop_front();
  }
  void clear() { versions_.clear(); }

  T *operator->() { return current_.get(); }
  const T *operator->() const { return current_.get(); }

private:
  std::unique_ptr<T> current_;
  std::forward_list<std::unique_ptr<T>> versions_;
};

template <class T, class... Args>
VersionedPtr<T> make_versioned(Args &&... args) {
  return VersionedPtr<T>(std::make_unique<T>(std::forward<Args>(args)...));
}
