#include "tcs/vehicle/sim_vehicle_adapter.h"

namespace tcs {

SimVehicleAdapter::~SimVehicleAdapter() {
  BOOST_LOG_TRIVIAL(debug) << "Exit SimVehicleAdapter";
  if (simulate_thread_.joinable()) simulate_thread_.join();
}

void SimVehicleAdapter::Enable() {
  BOOST_LOG_TRIVIAL(debug) << "Enable SimVehicleAdapter";
  enabled_ = true;
  UpdateVehicleStateEvent().Fire(VehicleState::kIdle);
  BOOST_LOG_TRIVIAL(debug) << "Simulation thread initializing";
  simulate_thread_ = std::thread(&SimVehicleAdapter::SimulateTasks, this);
}

void SimVehicleAdapter::Disable() {
  BOOST_LOG_TRIVIAL(debug) << "Disable SimVehicleAdapter";
  enabled_ = false;
  UpdateVehicleStateEvent().Fire(VehicleState::kUnknown);
}

bool SimVehicleAdapter::CanProcess(std::unordered_set<std::string> operations) {
  std::scoped_lock<std::mutex> lk{mut_};
  for (auto& op : operations) {
    if (op == "Load" && loaded_) return false;
  }
  return true;
}

bool SimVehicleAdapter::EnqueueCommand(MovementCommand command) {
  BOOST_LOG_TRIVIAL(debug) << "Enqueue command";
  std::scoped_lock<std::mutex> lk{mut_};
  if (command_queue_.size() >= kMaxCommandQueueSize) return false;
  command_queue_.push(command);
  return true;
}

void SimVehicleAdapter::ClearCommandQueue() {
  BOOST_LOG_TRIVIAL(debug) << "Clear command queue";
  std::scoped_lock<std::mutex> lk{mut_};
  command_queue_ = {};
}

void SimVehicleAdapter::SimulateTasks() {
  MovementCommand cmd;
  while (enabled_) {
    bool has_cmd = false;
    {
      std::scoped_lock<std::mutex> lk{mut_};
      if (!command_queue_.empty()) {
        cmd = std::move(command_queue_.front());
        command_queue_.pop();
        has_cmd = true;
      }
    }

    if (has_cmd) {
      try {
        // Sim move
        BOOST_LOG_TRIVIAL(debug)
            << "Simulating movement from " << cmd.step.source->get_id()
            << " towards " << cmd.step.destination->get_id();
        // double time_cost = cmd.step.path->get_length() / kSimSpeed * 1000;
        // std::this_thread::sleep_for(
        //     std::chrono::milliseconds(long(round(time_cost))));
        UpdateVehicleStateEvent().Fire(VehicleState::kExecuting);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        UpdatePositionEvent().Fire(cmd.step.destination->get_id());

        // Sim operation
        BOOST_LOG_TRIVIAL(debug) << "Simulating operation: " << cmd.operation;
        if (cmd.operation != kNoOperation) {
          if (cmd.operation == kChargeOperation) {
            UpdateVehicleStateEvent().Fire(VehicleState::kCharging);
            cnt_ = 0;
            std::this_thread::sleep_for(
                std::chrono::seconds(kSimChargeOperation));
          } else {
            std::this_thread::sleep_for(std::chrono::seconds(kSimOperation));
            cnt_ += 1;
          }

          {
            std::scoped_lock<std::mutex> lk{mut_};
            if (cmd.operation == kLoadOperation) loaded_ = true;
            if (cmd.operation == kUnloadOperation) loaded_ = false;
          }
        }
        if (cnt_ >= 20) RequestChargeEvent().Fire();
        UpdateVehicleStateEvent().Fire(VehicleState::kIdle);
        FinishCommandEvent().Fire(std::move(cmd));
      } catch (std::exception e) {
        BOOST_LOG_TRIVIAL(error) << e.what();
        FailCommandEvent().Fire(std::move(cmd));
      }
    }
  }
}

}  // namespace tcs