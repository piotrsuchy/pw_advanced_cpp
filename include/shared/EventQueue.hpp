#pragma once

#include <utility>
#include <vector>

/**
 * @brief Small per-tick event buffer with typed drain semantics.
 *
 * `EventQueue<T>` is used by gameplay systems to accumulate events during a
 * simulation step and then hand them off in one move to another subsystem.
 *
 * @tparam T Event payload type stored in the queue.
 */
template <typename T>
class EventQueue {
   public:
    /**
     * @brief Appends one event to the queue by copy.
     *
     * @param event Event value to store.
     */
    void push(const T& event) {
        events_.push_back(event);
    }

    /**
     * @brief Appends one event to the queue by move.
     *
     * @param event Event value to store.
     */
    void push(T&& event) {
        events_.push_back(std::move(event));
    }

    /**
     * @brief Removes all currently buffered events.
     */
    void clear() {
        events_.clear();
    }

    /**
     * @brief Moves all buffered events into an output vector.
     *
     * After the call, the queue becomes empty and ready for the next tick.
     *
     * @param out Destination vector receiving the drained events.
     */
    void drainTo(std::vector<T>& out) {
        out = std::move(events_);
        events_.clear();
    }

   private:
    std::vector<T> events_;  ///< Buffered events waiting to be drained.
};
