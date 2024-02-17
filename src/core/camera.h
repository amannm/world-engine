

#include "entity.h"

#include <mutex>
#include <unordered_set>
#include <vector>

#include <boost/unordered/concurrent_flat_set.hpp>

class CameraNode {
private:
    std::mutex m;
    std::unordered_set<std::shared_ptr<EntityNode>> outbound;
    boost::unordered::concurrent_flat_set<Observer> observers;
public:
    CameraProps props;
    std::atomic<std::shared_ptr<State>> state;
    explicit CameraNode(CameraProps props): props(props){};
    void update(const std::shared_ptr<State>&);
    void merge_changes(const std::vector<std::shared_ptr<EntityNode>>&);
    void flush_changes();
    void observe(Observer);
    void unobserve(Observer);
};