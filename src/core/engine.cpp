#include "engine.h"

#include <algorithm>


constexpr uint_fast8_t min_interval_ms{std::numeric_limits<uint_fast8_t>::min()};
constexpr uint_fast8_t max_interval_ms{std::numeric_limits<uint_fast8_t>::max()};

Engine::~Engine() {
    task.request_stop();
}

void Engine::start() {
    const auto start_timestamp = std::chrono::system_clock::now();
    task = std::jthread([this](auto st) {
        uint_fast8_t simulation_interval = 0;
        do {
            const auto start = std::chrono::steady_clock::now();
            step(simulation_interval);
            const auto end = std::chrono::steady_clock::now();
            const auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            simulation_interval = std::clamp<uint_fast8_t>(interval, min_interval_ms, max_interval_ms);
        } while (!st.stop_requested());
    });
}

void Engine::step(uint_fast8_t interval) {
    // simulate state over the previously recorded simulation time then return the full delta
    const auto delta = simulate(interval);
    // with a thread per camera
    cameras.cvisit_all(std::execution::par_unseq, [&delta](auto entry) {
        // perform a scan down the full delta, performing hit testing based on the camera state
        entry.second->merge_changes(delta);
    });
}


void Engine::stop() {
    task.request_stop();
}

void Engine::create_entity(EntityTag e, EntityProps p, const std::shared_ptr<State>& s) {
    entities.insert_or_assign(e, std::make_shared<EntityNode>(e, p, s));
}

void Engine::update_entity(EntityTag e, const std::shared_ptr<State>& s) {
    entities.cvisit(e, [s](auto pair) {
        pair.second->update(s);
    });
}

void Engine::delete_entity(EntityTag e) {
    entities.erase(e);
}

void Engine::create_camera(CameraTag c, CameraProps p, const std::shared_ptr<State>& s) {
    cameras.insert_or_assign(c, std::make_shared<CameraNode>(p, s));
}

void Engine::update_camera(CameraTag e, const std::shared_ptr<State>& s) {
    cameras.cvisit(e, [s](auto pair) {
        pair.second->update(s);
    });
}

void Engine::delete_camera(CameraTag c) {
    cameras.erase(c);
}

void Engine::observe_camera(CameraTag c, Observer o) {
    cameras.cvisit(c, [this, o](auto pair) {
        pair.second->observe(o);
    });
}

void Engine::unobserve_camera(CameraTag c, Observer o) {
    cameras.cvisit(c, [o](auto pair) {
        pair.second->unobserve(o);
    });
}

std::vector<std::shared_ptr<EntityNode>> Engine::simulate(uint_fast8_t interval) {
    auto results = std::vector<std::shared_ptr<EntityNode>>(std::cbegin(entities), std::cend(entities));
    m.lock();
    std::copy(std::begin(results),)
    entities.cvisit_all([&results](auto pair) {
        // TODO: physics and collisions
        results.push_back(pair.second);
    });
    m.unlock();
    return results;
}


