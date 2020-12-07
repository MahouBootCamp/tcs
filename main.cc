#include "tcs/kernal.h"

int main(int, char**) {
  // Read/Initialize model
  // Initialize kernal
  // Set vehicle controller and set initial position for vehicles
  // Set kernal as running
  // Create transport orders
  // Exit

  std::unique_ptr<tcs::IKernal> kernal{new tcs::Kernal{}};
  kernal->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  kernal->Exit();
  return 0;
}