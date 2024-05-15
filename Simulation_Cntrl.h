
#include <chrono>
#include <variant>
#include <tuple>
#include <functional>
#include <vector>

using latency_ty = std::variant<std::chrono::seconds, std::chrono::minutes, std::chrono::hours>;
using cntrl_ty = std::tuple<latency_ty, std::function<void()>>;
using cntrl_set_ty = std::vector<cntrl_ty>;

void Excecute(cntrl_set_ty const& data);
