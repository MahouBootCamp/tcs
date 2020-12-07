#include "tcs/kernal.h"

namespace tcs {

Kernal::~Kernal() {
  if (state_ != KernalState::kExit) Exit();
  if (monitor_.joinable()) monitor_.join();
}

void Kernal::Start() {
  BOOST_LOG_TRIVIAL(info) << "Kernal: start operating...";
  state_ = KernalState::kOperating;
  quit_fut_ = on_quit_.get_future();
  monitor_ = std::thread([this]() {
    if (quit_fut_.valid()) quit_fut_.wait();
  });
}

void Kernal::Exit() {
  BOOST_LOG_TRIVIAL(info) << "Kernal: exiting...";
  state_ = KernalState::kExit;
  on_quit_.set_value();
}

void Kernal::SetVehicleController() {}

void Kernal::AddTransportOrder() {}

}  // namespace tcs