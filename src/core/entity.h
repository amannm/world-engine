extern "C" {
#include "abi.h"
}

#include <utility>
#include <atomic>
#include <memory>

class EntityNode {
public:
    EntityTag tag;
    EntityProps props;
    std::shared_ptr<State> state;
    explicit EntityNode(EntityTag tag, EntityProps props, const std::shared_ptr<State>& state): tag(tag), props(props), state(state) {};
    void update(const std::shared_ptr<State>&);
};
