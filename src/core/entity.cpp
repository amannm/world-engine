#include "entity.h"

void EntityNode::update(const std::shared_ptr<State>& s) {
    state.store(s);
}
