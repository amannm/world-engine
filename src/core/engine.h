#include "camera.h"
#include "entity.h"

#include <thread>
#include <boost/unordered/concurrent_flat_map.hpp>

typedef struct {
    EntityProps props;
    std::atomic<State*> state;
} Entity;

typedef struct {
    CameraProps props;
    std::atomic<State*> state;
} Camera;

class Engine {
private:
    std::mutex m;
    boost::concurrent_flat_map<EntityTag, std::shared_ptr<EntityNode>> entities;
    boost::concurrent_flat_map<CameraTag, std::shared_ptr<CameraNode>> cameras;
    std::jthread task;
public:
    ~Engine();
    void start();
    void stop();
    void step(uint_fast8_t);
    void create_entity(EntityTag, EntityProps, const std::shared_ptr<State>&);
    void update_entity(EntityTag, const std::shared_ptr<State>&);
    void delete_entity(EntityTag);
    void create_camera(CameraTag, CameraProps, const std::shared_ptr<State>&);
    void update_camera(CameraTag, const std::shared_ptr<State>&);
    void delete_camera(CameraTag);
    void observe_camera(CameraTag, Observer);
    void unobserve_camera(CameraTag, Observer);
    std::vector<std::shared_ptr<EntityNode>> simulate(uint_fast8_t);
};

