#ifndef EVENT_H
#define EVENT_H

#include <future>
#include <list>
#include <mutex>
#include <vector>

namespace tcs {

// My event implementation
template <class... Args>
class Event {
 public:
  Event() = default;

  std::size_t Subscribe(std::function<void(Args...)> handler) {
    std::scoped_lock<std::mutex> lk{mut_};
    auto index = handlers_.size();
    handlers_.push_back(handler);
    return index;
  }

  void Unsubscribe(std::size_t index) {
    std::scoped_lock<std::mutex> lk{mut_};
    auto itr = handlers_.begin();
    for (size_t i = 0; i != index; ++i) {
      itr++;
    }
    handlers_.erase(itr);
  }

  void Fire(Args... args) {
    std::scoped_lock<std::mutex> lk{mut_};
    for (auto& handler : handlers_) {
      futures_.push_back(
          std::async(std::launch::async, handler, args...));
      // futures_.push_back(std::async(std::launch::async, handler,
      //                               std::forward<Args>(args)...));
    }
    CheckStatus();
  }

  ~Event() {
    std::scoped_lock<std::mutex> lk{mut_};
    for (auto& fut : futures_) {
      fut.get();
    }
  }

 private:
  void CheckStatus() {
    for (auto i = futures_.begin(); i != futures_.end();) {
      if (!i->valid() || i->wait_for(std::chrono::nanoseconds(1)) ==
                             std::future_status::ready) {
        futures_.erase(i);
      } else
        ++i;
    }
  }

  mutable std::mutex mut_;
  std::list<std::function<void(Args...)>> handlers_;
  std::list<std::future<void>> futures_;
};

}  // namespace tcs

#endif /* EVENT_H */
