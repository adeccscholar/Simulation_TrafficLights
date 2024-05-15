#include "Simulation_Cntrl.h"

#include <thread>

void Excecute(cntrl_set_ty const& data) {
   for (auto const& [duration, func] : data) {
      std::visit([&](auto&& dur) {
         using DurationType = std::decay_t<decltype(dur)>;
         if constexpr (std::is_same_v<DurationType, std::chrono::seconds>) {
            std::this_thread::sleep_for(dur);
            }
         else if constexpr (std::is_same_v<DurationType, std::chrono::minutes>) {
            std::this_thread::sleep_for(dur);
            }
         else if constexpr (std::is_same_v<DurationType, std::chrono::hours>) {
            std::this_thread::sleep_for(dur);
            }
         }, duration);
      func();
      }
   }
