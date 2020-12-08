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
  Event(std::function<void(Args...)> handler) : handler_{handler} {}
  void Fire(Args&&... args) {
    std::scoped_lock<std::mutex> lk{mut_};
    futures_.push_back(
        std::async(std::launch::async, handler_, std::forward<Args>(args)...));
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
  std::function<void(Args...)> handler_;
  std::list<std::future<void>> futures_;
};

}  // namespace tcs

#endif /* EVENT_H */
