/*
 * ============================================================
 * CUSTOM MODULE - TU Delft MAV Course Group Addition
 * This is NOT a standard Paparazzi module.
 * ============================================================
 *
 * Ground segmentation module header.
 * Segments ground from non-ground using YUV color thresholds.
 */

#ifndef GROUND_SEGMENT_H
#define GROUND_SEGMENT_H

#include <stdint.h>
#include <stdbool.h>

/* Default camera if not defined in airframe XML */
#ifndef GROUND_SEGMENT_CAMERA
#define GROUND_SEGMENT_CAMERA front_camera
#endif

/* Default FPS (0 = run at camera rate) */
#ifndef GROUND_SEGMENT_FPS
#define GROUND_SEGMENT_FPS 0
#endif

/* Default YUV thresholds for ground detection */
#ifndef GROUND_SEGMENT_Y_MIN
#define GROUND_SEGMENT_Y_MIN 50
#endif

#ifndef GROUND_SEGMENT_U_MAX
#define GROUND_SEGMENT_U_MAX 130
#endif

#ifndef GROUND_SEGMENT_V_MAX
#define GROUND_SEGMENT_V_MAX 130
#endif

/* Runtime-tunable parameters (exposed via settings) */
extern uint8_t gs_y_min;
extern uint8_t gs_u_max;
extern uint8_t gs_v_max;
extern bool gs_draw;

/* Ground coverage result (percentage of frame that is ground) */
extern float ground_segment_pct;

extern void ground_segment_init(void);

#endif /* GROUND_SEGMENT_H */
