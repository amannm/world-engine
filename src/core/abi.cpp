#include "engine.h"

struct Universe {
    std::unique_ptr<Engine> engine;
    std::atomic<EntityTag> entity_counter;
    std::atomic<CameraTag> camera_counter;
};

Universe* w_create() {
    return new Universe{std::make_unique<Engine>()};
}
void w_delete(Universe* u) {
    delete u;
}
void w_start(Universe* u) {
    u->engine->start();
}
void w_stop(Universe* u) {
    u->engine->stop();
}

EntityTag e_create(Universe* u, EntityProps p, State s) {
    auto e = u->entity_counter++;
    u->engine->create_entity(e, p, s);
    return e;
}
void e_update(Universe* u, EntityTag e, State s) {
    auto sp = std::make_shared<State>(s);
    return u->engine->update_entity(e, sp);
}
void e_delete(Universe* u, EntityTag e) {
    u->engine->delete_entity(e);
}

CameraTag c_create(Universe* u, CameraProps p, State s) {
    auto c = u->camera_counter++;
    u->engine->create_camera(c, p, s);
    return c;
}
void c_update(Universe* u, CameraTag c, State s) {
    auto sp = std::make_shared<State>(s);
    return u->engine->update_camera(c, sp);
}
void c_delete(Universe* u, CameraTag c) {
    u->engine->delete_camera(c);
}

void c_attach(Universe* u, CameraTag c, Observer o) {
    u->engine->observe_camera(c, o);
}
void c_detach(Universe* u, CameraTag c, Observer o) {
    u->engine->unobserve_camera(c, o);
}