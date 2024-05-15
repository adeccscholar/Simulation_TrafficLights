#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <chrono>
#include <thread>
#include <format>
#include <functional>

struct ClockedSystem {
   using my_time_point = std::chrono::time_point<std::chrono::system_clock>;
   using duration_ty = std::variant<std::chrono::seconds, std::chrono::minutes, std::chrono::hours>;

   struct duration_ty_visitor {
      std::chrono::seconds operator()(const std::chrono::seconds& s) const {
         return s;
         }

      std::chrono::seconds operator()(const std::chrono::minutes& m) const {
         return std::chrono::duration_cast<std::chrono::seconds>(m);
         }

      std::chrono::seconds operator()(const std::chrono::hours& h) const {
         return std::chrono::duration_cast<std::chrono::seconds>(h);
         }
      };


   [[nodiscard]] static std::string get_date_time_string(my_time_point now) {
      auto const timew = std::chrono::current_zone()->to_local(now);
      // bug in lib mit format for time values, seconds as double
      const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timew.time_since_epoch()) % 60;
      const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(timew.time_since_epoch()) % 1000;
      return std::format("{:%d.%m.%Y %H:%M}:{:02d},{:03d}", timew, seconds.count(), millis.count());
      }

   // possible to make this to a Qt Object and use slot / signal to handle output
   // bug (or feature) with formating time_pount with double value for seconds force work around with function get_date_time_string 
   static void LogMessage(std::string const& out, my_time_point const& now = std::chrono::system_clock::now()) {
      std::cout << std::format("{}: {}\n", get_date_time_string(now), out);
      }


   [[nodiscard]] static constexpr my_time_point end_time(my_time_point const& start, duration_ty const& wait_time) {
      return start + std::visit(duration_ty_visitor{}, wait_time);
      }

   void wait_until(my_time_point const& end_time, std::function<void()> get_func, std::function<void()> func = nullptr) {
      while (std::chrono::system_clock::now() < end_time) {
         if (get_func != nullptr) get_func();
         if (func != nullptr) func();
         std::this_thread::sleep_for(std::chrono::milliseconds(500));
         }
      }

   };
