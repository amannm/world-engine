#include "buffer.h"

#include <math.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdbool.h>

// for each client, make an array of length N, where each index represents one client side input poll, so for 1000hz, we have a each index representing 1 ms
// we can determine the min buffer size by max_simulation_interval_ms * client_input_poll_interval
// 20ms * 1ms
float lerp(double a, double b, double t) {
    return (float) (a + t * (b - a));
}

delta* lerp_delta(delta* a, delta* b, double t) {
    delta* interp = (delta*) malloc(sizeof(delta));
    interp->x = lerp(a->x, b->x, t);
    interp->y = lerp(a->y, b->y, t);
    interp->z = lerp(a->z, b->z, t);
    interp->pitch = lerp(a->pitch, b->pitch, t);
    interp->roll = lerp(a->roll, b->roll, t);
    interp->yaw = lerp(a->yaw, b->yaw, t);
    return interp;
}

delta* create_delta(float x, float y, float z, float pitch, float roll, float yaw) {
    delta* dp = (delta*) malloc(sizeof(delta));
    dp->x = x;
    dp->y = y;
    dp->z = z;
    dp->pitch = pitch;
    dp->roll = roll;
    dp->yaw = yaw;
    return dp;
}

void print_delta(delta** ds, unsigned char length) {
    for (unsigned char i = 0; i < length; i++) {
        delta* dp = *(ds + i);
        if (dp != NULL) {
            printf("%f %f %f %f %f %f\n", dp->x, dp->y, dp->z, dp->pitch, dp->roll, dp->yaw);
        }
    }
}

delta** create_buffer(unsigned char length) {
    return (delta**) malloc(length * sizeof(delta*));
}

void destroy_buffer(delta** db, unsigned char length) {
    for (int i = 0; i < length; i++) {
        delta* d = *(db + i);
        if (d != NULL) {
            free(d);
        }
    }
    free(db);
}

void reset_buffer(delta** db, unsigned char length) {
    for (unsigned char i = 0; i < length; i++) {
        delta* d = db[i];
        if (d != NULL) {
            free(d);
        }
        db[i] = (delta*) malloc(sizeof(delta));
    }
}

void flush(delta*** global_current_ptr, _Atomic (delta*)** global_incoming_ptr, unsigned short num_clients) {
    _Atomic (delta*)* global_incoming = *global_incoming_ptr;
    for (unsigned short i = 0; i < num_clients; i++) {
        delta** db = global_incoming + i;
        reset_buffer(db, 255);
        delta** dbin = global_incoming[i];
        db = atomic_exchange(&global_incoming[i], db);
    }
}



// precondition: client_inputs_snapshot starts with a non-null pointer
void fill_gaps(delta** db, unsigned char length) {
    if (db == NULL) {
        return;
    }
    unsigned char current_idx = 1;
    unsigned char pre_gap_idx = 0;
    bool in_gap = false;
    while (current_idx < length) {
        delta* current = db[current_idx];
        if (current == NULL) {
            if (in_gap == false) {
                pre_gap_idx = current_idx - 1;
                in_gap = true;
            }
        } else {
            if (in_gap == true) {
                delta* pre_gap = db[pre_gap_idx];
                unsigned char gap_length = current_idx - pre_gap_idx;
                for (unsigned char i = 1; i < gap_length; i++) {
                    db[pre_gap_idx + i] = lerp_delta(pre_gap, current, (double) i / gap_length);
                }
                in_gap = false;
            }
        }
        current_idx++;
    }
}


int main() {
    delta** db = create_buffer(9);
    delta* d1 = create_delta(1, 1, 1, -40, 20, 10);
    delta* d2 = create_delta(4, 4, 4, 0, 60, 5);
    delta* d3 = create_delta(2, 2, 2, 60, -30, 0);
    db[0] = d1;
    db[3] = d2;
    db[8] = d3;
    fill_gaps(db, 9);
    print_delta(db, 9);
    destroy_buffer(db, 9);
    return 0;
}