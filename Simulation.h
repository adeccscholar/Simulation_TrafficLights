#pragma once

#include "Clocked_System.h"

#include <iostream>
#include <chrono>
#include <variant>
#include <thread>
#include <atomic>

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/in_state_reaction.hpp>

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

// --------------------------------------------------------------------------------------
// declaration of the events / trigger for the statemachine

class TrafficSystem;

struct Power_Switch : sc::event<Power_Switch> {
   TrafficSystem& cntrl_system;
   Power_Switch(TrafficSystem& sys) : cntrl_system{ sys } { }
   };

struct Call_Switch_Event : sc::event<Call_Switch_Event> {
   TrafficSystem& cntrl_system;
   Call_Switch_Event(TrafficSystem& sys) : cntrl_system{ sys } { }
   };

struct Change_Event : sc::event<Change_Event> {
   TrafficSystem& cntrl_system;
   Change_Event(TrafficSystem& sys) : cntrl_system{ sys } { }
   };

struct Stop_Event : sc::event<Stop_Event> {
   TrafficSystem& cntrl_system;
   Stop_Event(TrafficSystem& sys) : cntrl_system{ sys } { }
   };

struct Null_Event : sc::event<Null_Event> {
   TrafficSystem& cntrl_system;
   Null_Event(TrafficSystem& sys) : cntrl_system{ sys } { }
};



// --------------------------------------------------------------------------------------
// declaration of the TrafficSystem

class TrafficSystem : public ClockedSystem {
   public:

      enum struct EPhase : char32_t {
         starting, stopping, green_lights, yellow_lights, red_lights_pre,
         red_lights_pedestrian, red_lights_post, red_yellow_lights,
         yellow_blinking
         };

      using phases_durations_ty = std::map<EPhase, duration_ty>;

      TrafficSystem(std::atomic<bool>& pPower, std::atomic<bool>& pSwitch) : boPower{ pPower }, boSwitch{ pSwitch },
                    boWaiting{ false }, funcWaiting{ nullptr } { }

      template <EPhase phase>
      static my_time_point end_time(my_time_point const& start) {
         static const duration_ty to_wait = phases_durations.at(phase);
         return ClockedSystem::end_time(start, to_wait);
         }

      void get_Controls() {
         }

      void start_signal_incoming(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system, start signal incoming. ]", start_time);
         }

      void stop_signal_incoming(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system, stop signal incoming. ]", start_time);
         }

      void set_starting_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is starting now. ]", start_time);
         wait_until(end_time<EPhase::starting>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting); // blinking yellow traffic lights
         }

      void set_stopping_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is stopping now. ]", start_time);
         wait_until(end_time<EPhase::stopping>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting); // blinking yellow traffic lights
         }

      void set_green_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in green light state now. ]", start_time);
         wait_until(end_time<EPhase::green_lights>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }

      void set_yellow_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in yellow light state now. ]", start_time);
         wait_until(end_time<EPhase::yellow_lights>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }

      void set_red_lights_prep(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in red light state (1st Phase) now. ]", start_time);
         wait_until(end_time<EPhase::red_lights_pre>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }

      void set_red_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in red light state (with pedestrian) now. ]", start_time);
         wait_until(end_time<EPhase::red_lights_pedestrian>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }

      void set_red_lights_post(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in red light state (post phase) now. ]", start_time);
         wait_until(end_time<EPhase::red_lights_post>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }


      void set_red_yellow_lights(my_time_point const& start_time) {
         LogMessage(" [ traffic lights system is in red + yellow lights state now. ]", start_time);
         wait_until(end_time<EPhase::red_yellow_lights>(start_time), std::bind(&TrafficSystem::get_Controls, this), funcWaiting);
         }

      std::function<void()> funcWaiting;
      std::atomic<bool>& boSwitch;
      std::atomic<bool>& boPower;
      bool                  boWaiting;

      static const phases_durations_ty inline phases_durations = {
           { EPhase::starting,              std::chrono::seconds(10) },  // warning time when the traffic light system starting
           { EPhase::stopping,              std::chrono::seconds(10) },  // warning time when the traffic light system stopping
           { EPhase::green_lights,          std::chrono::seconds(34) },  // minimum green time
           { EPhase::yellow_lights,         std::chrono::seconds( 4) },  // 1 second processing time + 3 seconds yellow
           { EPhase::red_lights_pre,        std::chrono::seconds( 1) },  // clearance time for cars 
           { EPhase::red_lights_pedestrian, std::chrono::seconds( 6) },  // red for traffic and green for pedestrians
           { EPhase::red_lights_post,       std::chrono::seconds(12) },  // clearance time for peds, possible to send a warn signal
           { EPhase::red_yellow_lights,     std::chrono::seconds( 3) },  // corresponding to yellow time, most empirical values
           { EPhase::yellow_blinking,       std::chrono::minutes(20) }
         };

   };


