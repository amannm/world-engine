#include <CGAL/Cartesian.h>

typedef CGAL::Point_3<CGAL::Cartesian<float>> point_t;
typedef CGAL::Iso_cuboid_3<CGAL::Cartesian<float>> box_t;
typedef CGAL::Tetrahedron_3<CGAL::Cartesian<float>> tetrahedron_t;
typedef struct {
    tetrahedron_t first;
    tetrahedron_t second;
} pyramid_t;

static box_t calc_bounds(const Point& center, const EntityProps& props) {
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

static pyramid_t calc_bounds(const Point& origin, const CameraProps& props) {
    auto far_y = props.max_range;
    auto far_x = far_y * std::tan(props.horizontal_fov / 2);
    auto far_z = far_y * std::tan(props.vertical_fov / 2);
    auto p_p = point_t{origin.x, origin.y, origin.z};
    auto p_q = point_t{far_x, far_y, far_z};
    auto p_r = point_t{-far_x, far_y, -far_z};
    auto pa_s = point_t{-far_x, far_y, far_z};
    auto pb_s = point_t{far_x, far_y, -far_z};
    return {
            tetrahedron_t{p_p, p_q, p_r, pa_s},
            tetrahedron_t{p_p, p_q, p_r, pb_s},
    };
}
