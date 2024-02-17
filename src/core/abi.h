struct Universe;

typedef unsigned long long ts_t;

typedef unsigned long long e_id_t;
typedef unsigned long long c_id_t;

typedef unsigned short e_snap_count_t;
typedef unsigned char c_snap_count_t;

typedef struct {
    const float width;
    const float length;
    const float height;
} e_props_t;

typedef struct {
    const float max_range;
    const float horizontal_fov;
    const float vertical_fov;
} c_props_t;

typedef struct {
    const unsigned long long start;
    const unsigned long long finish;
    const float x;
    const float y;
    const float z;
    const float yaw;
    const float pitch;
    const float roll;
} delta_t;

typedef struct {
    delta_t* buffer;
    unsigned char length;
} c_delta_buffer;

typedef struct {
    _Atomic(c_delta_buffer*)* buffer;
    unsigned short int length;
} c_delta_frame;

c_delta_buffer* swap(c_delta_frame*, unsigned short int, c_delta_buffer*);



Universe* u_create();
void u_delete(Universe*);
u_delta_t* u_step(u_delta_t*);

e_id_t e_create(Universe*, e_props_t, delta_t);
void e_update(Universe*, e_delta_t*, e_snap_count_t);
void e_delete(Universe*, e_id_t);

c_id_t c_create(Universe*, c_props_t, delta_t);
void c_update(Universe*, c_delta_t*, c_snap_count_t);
void c_delete(Universe*, c_id_t);
