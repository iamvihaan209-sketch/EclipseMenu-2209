#pragma once

#include <functional>
#include <vector>

namespace eclipse::events {

    /// @brief Minimal typed event bus, replacing geode::Event (removed in Geode v6 alpha).
    /// Each event type keeps its own list of listeners, and post() calls all of them.
    template <typename Derived>
    class Event {
    public:
        /// @brief Calls every listener registered for this event type
        void post() {
            for (auto& listener : listeners()) listener(static_cast<Derived&>(*this));
        }

        /// @brief Listeners registered for this event type
        static std::vector<std::function<void(Derived&)>>& listeners() {
            static std::vector<std::function<void(Derived&)>> instance;
            return instance;
        }
    };
}
