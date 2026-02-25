/*
 * ============================================================
 * CUSTOM MODULE - TU Delft MAV Course Group Addition
 * This is NOT a standard Paparazzi module.
 * ============================================================
 *
 * @file modules/computer_vision/ground_segment.c
 * @brief Ground segmentation using onboard camera in YUV422 format.
 *
 * Registers a callback with the video thread via cv_add_to_device().
 * Each frame is analyzed pixel-by-pixel using YUV color thresholds:
 *   - Y (luminance) must be above gs_y_min
 *   - U (blue chroma) must be below gs_u_max
 *   - V (red chroma) must be below gs_v_max
 *
 * Pixels that do NOT match the ground criteria are colored red
 * in the output image (when gs_draw is enabled), making the
 * segmentation visible in the RTP video stream.
 *
 * Thresholds can be tuned at runtime via the GCS settings panel.
 */

#include "modules/computer_vision/ground_segment.h"
#include "modules/computer_vision/cv.h"
#include <stdio.h>

/* Runtime-tunable parameters, initialized from compile-time defines */
uint8_t gs_y_min = GROUND_SEGMENT_Y_MIN;
uint8_t gs_u_max = GROUND_SEGMENT_U_MAX;
uint8_t gs_v_max = GROUND_SEGMENT_V_MAX;
bool gs_draw = true;

/* Result: percentage of image classified as ground */
float ground_segment_pct = 0.0f;

/*
 * Callback invoked by the video_thread for each camera frame.
 *
 * Image format: UYVY (YUV 4:2:2)
 *   Every 4 bytes encode 2 pixels: [U][Y0][V][Y1]
 *   - U,V are chrominance (color), shared between the 2 pixels
 *   - Y0,Y1 are luminance (brightness), one per pixel
 *
 * Returns:
 *   img  - the (possibly modified) image continues to the RTP stream
 *   NULL - image is consumed and not forwarded
 */
static struct image_t *ground_segment_cb(struct image_t *img,
                                          uint8_t camera_id __attribute__((unused)))
{
  uint8_t *buf = (uint8_t *)img->buf;
  uint32_t width = img->w;
  uint32_t height = img->h;

  uint32_t ground_count = 0;
  uint32_t total_pixels = width * height;

  /* Iterate over every pixel pair (4 bytes = 2 pixels in UYVY) */
  for (uint32_t row = 0; row < height; row++) {
    for (uint32_t col = 0; col < width; col += 2) {
      uint32_t idx = (row * width + col) * 2;

      uint8_t u  = buf[idx + 0];  /* chrominance blue  */
      uint8_t y0 = buf[idx + 1];  /* luminance pixel 1 */
      uint8_t v  = buf[idx + 2];  /* chrominance red   */
      uint8_t y1 = buf[idx + 3];  /* luminance pixel 2 */

      /*
       * Ground detection:
       *   - Both pixels bright enough (Y > threshold)
       *   - Color is not too blue (U < threshold, neutral = 128)
       *   - Color is not too red  (V < threshold, neutral = 128)
       *
       * These defaults work for green/brown ground surfaces.
       * Tune via gs_y_min, gs_u_max, gs_v_max in the GCS.
       */
      int is_ground = (y0 > gs_y_min && y1 > gs_y_min &&
                        u < gs_u_max &&
                        v < gs_v_max);

      if (is_ground) {
        ground_count += 2;
      } else if (gs_draw) {
        /* Color non-ground pixels red for visualization */
        /* Red in YUV: Y=76, U=84, V=255 */
        buf[idx + 0] = 84;   /* U */
        buf[idx + 1] = 76;   /* Y0 */
        buf[idx + 2] = 255;  /* V */
        buf[idx + 3] = 76;   /* Y1 */
      }
    }
  }

  /* Store result for other modules to read */
  ground_segment_pct = (float)ground_count / (float)total_pixels * 100.0f;

  return img;
}

void ground_segment_init(void)
{
  /* Register our callback with the video system.
   *
   * Arguments:
   *   &GROUND_SEGMENT_CAMERA  - camera device (defined in airframe XML)
   *   ground_segment_cb       - function called per frame
   *   GROUND_SEGMENT_FPS      - max rate (0 = camera rate)
   *   0                       - listener id
   */
  cv_add_to_device(&GROUND_SEGMENT_CAMERA, ground_segment_cb, GROUND_SEGMENT_FPS, 0);
}
