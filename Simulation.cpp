// Simulation.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Simulation.h"

#include "Simulation_Cntrl.h"
#include "System_Info.h"

#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <variant>
#include <tuple>
#include <format>

// --------------------------------------------------------------------------------------
// forward declaration for states and meta states
struct OffState;
struct StartingState;
struct OnState;
  struct GreenState;
  struct YellowState;
  struct RedState;
     struct Red_Prep_State;
     struct Red_Main_State;
     struct Red_Post_State;
  struct RedYellowState;
struct StoppingState;


// --------------------------------------------------------------------------------------
// definition for the statemachine

using namespace std::chrono_literals;

struct TrafficControl : public TrafficSystem, public sc::state_machine<TrafficControl, OffState> {
 
   TrafficControl(std::atomic<bool>& pPower, std::atomic<bool>& pSwitch) : TrafficSystem(pPower, pSwitch) {
      LogMessage(" { simulation initialized and waiting }");
      }

   ~TrafficControl() { LogMessage(" { simulation stopped and deleted }");  }
   
   void operator ()() {
      get_Controls();
      if (boPower.load()) {
         process_event(Power_Switch(static_cast<TrafficSystem&>(*this)));
         boPower.store(false);
         }
      else {
         if (boSwitch.load()) {
            process_event(Call_Switch_Event(static_cast<TrafficSystem&>(*this)));
            boSwitch.store(false);
            }
         process_event(Change_Event(static_cast<TrafficSystem&>(*this)));
         }
      }
   };

struct State_Timed {
   std::chrono::time_point<std::chrono::system_clock> start_time;
   State_Timed() : start_time { std::chrono::system_clock::now() } { }
   };

struct OffState : public sc::state<OffState, TrafficControl>, public State_Timed {
   using my_base =  sc::state<OffState, TrafficControl>;
   OffState(my_context ctx) : my_base(ctx), State_Timed() {
      context<TrafficControl>().LogMessage(" [ traffic lights system is in OFF state. ]", start_time);
      }

   ~OffState() {  }

   typedef mpl::list<sc::custom_reaction<Power_Switch>,
                     sc::custom_reaction<Stop_Event>,
                     sc::custom_reaction<Null_Event>> reactions;

   sc::result react(Power_Switch const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights system starting now ...");
      return transit<StartingState>();
      }

   sc::result react(Stop_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights system finshed ...");
      return discard_event();
      }

   sc::result react(Null_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights null event ...");
      return discard_event();
   }

   };

/// state for stopping phase, traffic light yellow blinking, pedestrian light red
struct StartingState : public sc::state<StartingState, TrafficControl>, public State_Timed {
   typedef sc::state<StartingState, TrafficControl> my_base;
   StartingState(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_starting_lights(start_time);
      post_event(Power_Switch(static_cast<TrafficSystem&>(sm)));
      }

   ~StartingState() { }

   typedef mpl::list<sc::custom_reaction<Power_Switch>, 
                     sc::custom_reaction<Stop_Event>,
                     sc::custom_reaction<Null_Event>> reactions;

   sc::result react(Stop_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> starting stopped / critical");
      return transit<OffState>();
      }

   sc::result react(Power_Switch const& event) {
      //context<TrafficControl>().LogMessage(" -------> starting finished");
      return transit<OnState>();
      }

   sc::result react(Null_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights null event ...");
      return discard_event();
   }


   };

/// state for stopping phase, traffic light yellow blinking, pedestrian light red
struct StoppingState : public sc::state<StoppingState, TrafficControl>, public State_Timed {
   typedef sc::state<StoppingState, TrafficControl> my_base;
   StoppingState(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_stopping_lights(start_time);
      post_event(Power_Switch(static_cast<TrafficSystem&>(sm)));
      }

   ~StoppingState() { }

   typedef mpl::list<sc::custom_reaction<Power_Switch>,
                     sc::custom_reaction<Stop_Event>,
                     sc::custom_reaction<Null_Event>> reactions;

   sc::result react(Stop_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> finishing stopped / critical");
      return transit<OffState>();
      }

   sc::result react(Power_Switch const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights system stopping phase finished");
      return transit<OffState>();
   }

   sc::result react(Null_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights null event ...");
      return discard_event();
   }

};



struct OnState : public sc::state<OnState, TrafficControl, GreenState>, public State_Timed {
   typedef sc::state<OnState, TrafficControl, GreenState> my_base;
   OnState(my_context ctx) : my_base(ctx), State_Timed() {
      context<TrafficControl>().LogMessage(" [ traffic lights system is in ON state. ]", start_time);
      }

   typedef mpl::list<sc::custom_reaction<Power_Switch>,
                     sc::custom_reaction<Stop_Event>,
                     sc::custom_reaction<Call_Switch_Event>,
                     sc::custom_reaction<Null_Event>> reactions;

   sc::result react(Stop_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> stop");
      return transit<OffState>();
      }

   sc::result react(Power_Switch const& event) {
      //context<TrafficControl>().LogMessage(" -------> power switch off, system stopping.");
      return transit<StoppingState>();
      }

   sc::result react(Call_Switch_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> signal incoming ...");
      return discard_event(); // default reaction is discarding, only in green state haben something and in red state ignoring
      }

   sc::result react(Null_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> traffic lights null event ...");
      return discard_event();
   }


   };

struct GreenState : public sc::state<GreenState, OnState>, public State_Timed {
   typedef sc::state<GreenState, OnState> my_base;
   GreenState(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_green_lights(start_time);
      }
   ~GreenState() {  }

   typedef mpl::list<sc::custom_reaction<Change_Event>,
                     sc::custom_reaction<Call_Switch_Event> >reactions;

   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage("stay in green");
      return discard_event();
      }

   sc::result react(Call_Switch_Event const& event) {
      TrafficControl& sm = context<TrafficControl>();
      //sm.LogMessage(" -------> signal incoming ...");
      //sm.LogMessage(" -------> green to yellow");
      return transit<YellowState>();
      }
   };

struct YellowState : public sc::state<YellowState, OnState>, public State_Timed {
   typedef sc::state<YellowState, OnState> my_base;
   YellowState(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_yellow_lights(start_time);
      }
   ~YellowState() {  }

   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> yellow to red");
      return transit<RedState>();
      }

   };


struct RedState : public sc::state<RedState, OnState, Red_Prep_State>, public State_Timed {
   using my_base = sc::state<RedState, OnState, Red_Prep_State>;
   RedState(my_context ctx) : my_base(ctx), State_Timed() { std::cout << "red state begin\n"; }
   ~RedState() { std::cout << "red state left\n"; }

   typedef sc::custom_reaction<Call_Switch_Event> reactions;

   sc::result react(Call_Switch_Event const& event) {
      context<TrafficControl>().boSwitch.store(false); // ignore switch signal during red light phase
      return discard_event();;
      }

   };

struct Red_Prep_State : public sc::state<Red_Prep_State, RedState>, public State_Timed {
   typedef sc::state<Red_Prep_State, RedState> my_base;
   Red_Prep_State(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_red_lights_prep(start_time);
      }
   ~Red_Prep_State() {  }

   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> red to pedestian lights green");
      return transit<Red_Main_State>();
      }
   };

struct Red_Main_State : public sc::state<Red_Main_State, RedState>, public State_Timed {
   typedef sc::state<Red_Main_State, RedState> my_base;
   Red_Main_State(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.boSwitch.store(false);
      sm.set_red_lights(start_time);
      }

   ~Red_Main_State() {  }

   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> red change to post phase");
      return transit<Red_Post_State>();
   }
};

struct Red_Post_State : public sc::state<Red_Post_State, RedState>, public State_Timed {
   typedef sc::state<Red_Post_State, RedState> my_base;
   Red_Post_State(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_red_lights_post(start_time);
      }

   ~Red_Post_State() {  }

   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> red to red + yellow");
      return transit<RedYellowState>();
      }
   };


struct RedYellowState : public sc::state<RedYellowState, OnState>, public State_Timed {
   typedef sc::state<RedYellowState, OnState> my_base;
   RedYellowState(my_context ctx) : my_base(ctx), State_Timed() {
      TrafficControl& sm = context<TrafficControl>();
      sm.set_red_yellow_lights(start_time);
      }

   ~RedYellowState() {  }

   typedef sc::custom_reaction<Change_Event> reactions;
   sc::result react(Change_Event const& event) {
      //context<TrafficControl>().LogMessage(" -------> red + yellow to green");
      return transit<GreenState>();
      }
   };


void TrafficSimulation(std::atomic<bool>& boPower, std::atomic<bool>& boSwitch, std::atomic<bool>& boFinishing) {
   TrafficControl ts(boPower, boSwitch);
   ts.initiate();
   for(;;) {
      if (boFinishing.load()) {
         boFinishing.store(false);
         ts.process_event(Stop_Event(static_cast<TrafficSystem&>(ts)));
         break;
         }
      else ts();
      }
   return;
   }



int main() {
   std::atomic<bool> boPower     = false;
   std::atomic<bool> boSwitch    = false;
   std::atomic<bool> boFinishing = false;

   auto Write = [](std::string const& strText) {
      std::cout << std::format("{1:}: {0:~>60}> {2:} ...\n", '~', ClockedSystem::get_date_time_string(std::chrono::system_clock::now()), strText);
      };

   auto SwitchPressed = [&boSwitch, &Write]() {
      Write("switch button pressed");
      boSwitch.store(true);
      };

   auto PowerPressed = [&boPower, &Write]() {
      Write("power button pressed");
      boPower.store(true);
      };

   auto ShutOff = [&boFinishing, &Write]() {
      Write("system soft shutoff");
      boFinishing.store(true);
      };

   cntrl_set_ty simulation = {
      cntrl_ty { std::chrono::seconds(20), PowerPressed }, 
      cntrl_ty { std::chrono::seconds(40), SwitchPressed },
      cntrl_ty { std::chrono::minutes( 2), SwitchPressed },
      cntrl_ty { std::chrono::seconds(35), SwitchPressed },
      cntrl_ty { std::chrono::seconds(25), SwitchPressed },
      cntrl_ty { std::chrono::seconds(40), PowerPressed  },
      cntrl_ty { std::chrono::minutes( 1), ShutOff }
   };

	std::cout << "Finite State Machine with boost::statechart ...\n";
   SystemInfo();

   std::cout << "starting simulation ...\n"
             << "-----------------------\n\n";
   auto cntrl = std::thread(TrafficSimulation, std::ref(boPower), std::ref(boSwitch), std::ref(boFinishing));
   Excecute(simulation);
   cntrl.join();
   std::cout << "simulation finished ...\n";

   return 0;
   }
