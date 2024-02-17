#include "camera.h"

#include <utility>
#include <CGAL/Cartesian.h>

typedef CGAL::Point_3<CGAL::Cartesian<float>> point_t;
typedef CGAL::Iso_cuboid_3<CGAL::Cartesian<float>> box_t;
typedef CGAL::Tetrahedron_3<CGAL::Cartesian<float>> tetrahedron_t;
typedef struct {
    tetrahedron_t first;
    tetrahedron_t second;
} pyramid_t;

static box_t calc_entity_bounds(const EntityProps& props, const Point& center) {
    auto centerX = center.x;
    auto centerY = center.y;
    auto centerZ = center.z;
    auto xOffset = props.width / 2;
    auto yOffset = props.length / 2;
    auto zOffset = props.height / 2;
    return {
            point_t{
                    centerX - xOffset,
                    centerY - yOffset,
                    centerZ - zOffset,
            },
            point_t{
                    centerX + xOffset,
                    centerY + yOffset,
                    centerZ + zOffset,
            },
    };
}


static pyramid_t calc_camera_bounds(const CameraProps& props, const State& origin) {
    auto far_y = props.max_range;
    auto far_x = far_y * std::tan(props.horizontal_fov / 2);
    auto far_z = far_y * std::tan(props.vertical_fov / 2);
    auto p_p = point_t{origin.offset.x, origin.offset.y, origin.offset.z};
    auto p_q = point_t{far_x, far_y, far_z};
    auto p_r = point_t{-far_x, far_y, -far_z};
    auto pa_s = point_t{-far_x, far_y, far_z};
    auto pb_s = point_t{far_x, far_y, -far_z};
    return {
            tetrahedron_t{p_p, p_q, p_r, pa_s},
            tetrahedron_t{p_p, p_q, p_r, pb_s},
    };
}

static bool intersects(const pyramid_t& pyramid, const box_t& box) {
    return CGAL::do_intersect(pyramid.first, box) || CGAL::do_intersect(pyramid.second, box);
}
static bool intersects(const box_t& a, const box_t& b) {
    return CGAL::do_intersect(a, b);
}

void CameraNode::merge_changes(const std::vector<std::shared_ptr<EntityNode>>& es) {
    auto results = std::unordered_set<std::shared_ptr<EntityNode>>();
    auto cam_snap = *state.load();
    auto view_pyramid = calc_camera_bounds(props, cam_snap);
    for (const std::shared_ptr<EntityNode>& e : es) {
        auto ent_box = calc_entity_bounds(e->props, e->state->offset);
        auto visible = intersects(view_pyramid, ent_box);
        if (visible) {
            results.insert(e);
        }
    }
}

void CameraNode::flush_changes() {
    m.lock();
    auto sink = std::vector<std::shared_ptr<EntityNode>>(std::cbegin(outbound), std::cend(outbound));
    outbound.clear();
    m.unlock();
    observers.cvisit_all([&sink](const auto& o){
        auto updates = std::vector<EntityUpdate>();
        for (const auto& en : sink) {
            updates.emplace_back(std::move(EntityUpdate{en->tag, *en->state.load()}));
        }
        o(updates.data(), updates.size());
    });
}


void CameraNode::update(const std::shared_ptr<State>& s) {
    state.store(s);
}

void CameraNode::observe(Observer o) {
    observers.insert(o);
}

void CameraNode::unobserve(Observer o) {
    observers.erase(o);
}






std::vector<std::pair<CameraTag, std::vector<EntityTag>>> simula(const std::vector<std::pair<pyramid_t , CameraTag>>& cameras, const std::vector<std::pair<box_t, EntityTag>>& entities) {
    auto visibility = std::vector<std::pair<CameraTag, std::vector<EntityTag>>>();
    auto collisions = std::vector<std::pair<EntityTag, EntityTag>>();
    for (int i = 0; i < entities.size(); i++) {
        for (int j = 0; j < i; j++) {
            auto a = entities.at(i);
            auto b = entities.at(j);
            if (intersects(a.first, b.first)) {
                collisions.emplace_back(a.second, b.second);
            }
        }
        for (const auto& camera : cameras) {
            auto visible = std::vector<EntityTag>();
            for (const auto& entity : entities) {
                if (intersects(camera.first, entity.first)) {
                    visible.push_back(entity.second);
                }
            }
            visibility.emplace_back(camera.second, visible);
        }
    }
    return visibility;
}

std::vector<std::pair<EntityTag, EntityTag>> intersection_table(const std::vector<std::pair<box_t, EntityTag>>& entities) {
    auto collisions = std::vector<std::pair<EntityTag, EntityTag>>();
    for (int i = 0; i < entities.size(); i++) {
        for (int j = 0; j < i; j++) {
            auto a = entities.at(i);
            auto b = entities.at(j);
            if (intersects(a.first, b.first)) {
                collisions.emplace_back(a.second, b.second);
            }
        }
    }
    return collisions;
}
