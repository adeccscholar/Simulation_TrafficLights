// Simulation.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Simulation.h"

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>

namespace sc  = boost::statechart;
namespace mpl = boost::mpl;


// --------------------------------------------------------------------------------------

// declaration of the events / trigger for the statemachine

struct Power_Switch : sc::event<Power_Switch> {
   Power_Switch() { }
   };

struct Call_Switch_Event : sc::event<Call_Switch_Event> {
   Call_Switch_Event() { }
};

struct Change_Event : sc::event<Change_Event> {
   Change_Event() { }
   };

// --------------------------------------------------------------------------------------
// forward declaration for states and meta states
// 

/*
Das CRTP steht für "Curiously Recurring Template Pattern" und ist ein Designmuster in der C++-Programmierung. 
Es handelt sich um eine fortgeschrittene Technik, die es ermöglicht, statische Polymorphie zu erreichen. 
Das CRTP wird oft verwendet, um Laufzeitkosten zu vermeiden, die mit dynamischer Polymorphie verbunden sind.

Das Grundkonzept des CRTP besteht darin, eine Basisklasse als Template-Klasse zu definieren, die einen 
Typparameter für die abgeleitete Klasse verwendet. Die abgeleitete Klasse erbt dann von der Basisklasse, 
wobei der abgeleitete Klassentyp als Vorlageparameter angegeben wird. Dadurch wird ermöglicht, dass die 
Basisklasse auf den abgeleiteten Klassentyp zugreifen kann.

Ein häufiges Anwendungsbeispiel des CRTP ist die Implementierung von "Curiously Recurring Template Pattern" 
besteht darin, dass die abgeleitete Klasse Methoden oder Daten aus der Basisklasse verwendet, um spezifische 
Funktionalitäten bereitzustellen, während die Basisklasse auf Methoden oder Daten in der abgeleiteten Klasse 
zugreifen kann, um bestimmte Aufgaben zu erledigen. Dies ermöglicht eine starke statische Bindung zur 
Compilezeit und kann in vielen Fällen die Leistung verbessern, da keine dynamischen Polymorphiekosten entstehen.
*/

struct OffState;
// struct OnState;   // with meta state
struct GreenState;
struct YellowState;
struct RedState;
struct RedYellowState;

// --------------------------------------------------------------------------------------
// definition for the statemachine
struct TrafficControl : public sc::state_machine<TrafficControl, OffState> {
   TrafficControl() { std::cout << "simulation initialiazed and waiting ...\n"; }
   ~TrafficControl() { std::cout << "simulation finished.\n"; }
   };


struct OffState : public sc::simple_state<OffState, TrafficControl> {
   OffState() { std::cout << ": traffic lights system is in OFF state.\n";  }
   ~OffState() {  }
   typedef sc::transition<Power_Switch, GreenState> reactions;

   /*
   typedef sc::custom_reaction<Power_Switch> reactions;

   sc::result react(Power_Switch const& event) {
      std::cout << " ---> traffic lights system starting now ...");
      return transit<GreenState>();
      }
   */
   };

struct GreenState : public sc::simple_state<GreenState, TrafficControl> {
// struct GreenState : public sc::simple_state<GreenState, OnState> {
   GreenState()  { std::cout << ": traffic lights system is in green state.\n"; }
   ~GreenState() {  }
   typedef mpl::list<sc::transition<Call_Switch_Event, YellowState>,
                     sc::transition< Power_Switch, OffState>> reactions;

   /*
   typedef mpl::list<sc::custom_reaction<Change_Event>,
                     sc::custom_reaction<Call_Switch_Event> >reactions;

   sc::result react(Change_Event const& event) {
      std::cout << "stay in green");
      return discard_event();
      }

   sc::result react(Call_Switch_Event const& event) {
      std::cout << "green to yellow\n";
      return transit<YellowState>();
      }
   */
   /*
   sc::result react(Power_Switch const& event) {
      std::cout << "power switch off, system stopping.\n");
      return transit<OffState>();
      }
   */
   };

struct YellowState : public sc::simple_state<YellowState, TrafficControl> {
//struct YellowState : public sc::simple_state<YellowState, OnState> {
   YellowState()  { std::cout << ": traffic lights system is in yellow state.\n"; }
   ~YellowState() {  }
   
   typedef mpl::list<sc::transition<Power_Switch, OffState>,
                     sc::transition<Change_Event, RedYellowState>> reactions;
   
   /*
   typedef mpl::list<sc::custom_reaction<Change_Event>,
                     sc::custom_reaction<Power_Switch>>  reactions;
   
   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      event.cntrl_system.Control("yellow to red");
      return transit<RedState>();
      }
   */
   /*
   sc::result react(Power_Switch const& event) {
      event.cntrl_system.Control("power off");
      return transit<OffState>();
      }
   */
   };

struct RedState : public sc::simple_state<RedState, TrafficControl> {
//struct RedState : public sc::simple_state<RedState, OnState> {
   RedState() { std::cout << ": traffic lights system is in red state.\n"; }
   ~RedState() { /* std::cout << "switch to next state ...\n"; */ }

   typedef mpl::list <sc::transition<Power_Switch, OffState>,
                      sc::transition<Change_Event, RedYellowState>> reactions;
   /*
   typedef mpl::list<sc::custom_reaction<Change_Event>,
                     sc::custom_reaction<Power_Switch>>  reactions;
   */
   /*
   typedef sc::custom_reaction<Change_Event> reactions;

   sc::result react(Change_Event const& event) {
      event.cntrl_system.Control("red to red + yellow");
      return transit<RedYellowState>();
      }
   */
   /*
   sc::result react(Power_Switch const& event) {
      event.cntrl_system.Control("power off");
      return transit<OffState>();
      }
   */
   };

struct RedYellowState : public sc::simple_state<RedYellowState, TrafficControl> {
//struct RedYellowState : public sc::simple_state<RedYellowState, OnState> {
   RedYellowState()  { std::cout << ": traffic lights system is in red + yellow state.\n"; }
   ~RedYellowState() {  }
   
   typedef mpl::list <sc::transition<Power_Switch, OffState>,
                      sc::transition<Change_Event, GreenState>> reactions;
   
   /*
   typedef mpl::list<sc::custom_reaction<Change_Event>,
                     sc::custom_reaction<Power_Switch>>  reactions;
   */
   /*
   typedef sc::custom_reaction<Change_Event> reactions;
   sc::result react(Change_Event const& event) {
      event.cntrl_system.Control("red + yellow to green");
      return transit<GreenState>();
      }
   */
   /*
   sc::result react(Power_Switch const& event) {
      event.cntrl_system.Control("power off");
      return transit<OffState>();
      }
   */
   };



int main() {
   std::cout << "Simple Finite State Machine with boost::statechart (first try) ...\n";
   TrafficControl ts;
   ts.initiate();
   ts.process_event(Power_Switch());
   for (int i = 0; i < 15; ++i) ts.process_event(Change_Event());
   ts.process_event(Call_Switch_Event());
   for (int i = 0; i < 15; ++i) ts.process_event(Change_Event());
   ts.process_event(Power_Switch());
   return 0;
   }
