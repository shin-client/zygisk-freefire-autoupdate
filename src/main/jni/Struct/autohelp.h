#pragma once

#include "../Unity/Color.h"

static Color FromHSB(float hue, float saturation, float brightness) {
  float h = hue == 1.0f ? 0 : hue * 6.0f;
  float f = h - (int)h;
  float p = brightness * (1.0f - saturation);
  float q = brightness * (1.0f - saturation * f);
  float t = brightness * (1.0f - (saturation * (1.0f - f)));
  if (h < 1) {
    return Color((unsigned char)(brightness * 255), (unsigned char)(t * 255), (unsigned char)(p * 255));
  } else if (h < 2) {
    return Color((unsigned char)(q * 255), (unsigned char)(brightness * 255), (unsigned char)(p * 255));
  } else if (h < 3) {
    return Color((unsigned char)(p * 255), (unsigned char)(brightness * 255), (unsigned char)(t * 255));
  } else if (h < 4) {
    return Color((unsigned char)(p * 255), (unsigned char)(q * 255), (unsigned char)(brightness * 255));
  } else if (h < 5) {
    return Color((unsigned char)(t * 255), (unsigned char)(p * 255), (unsigned char)(brightness * 255));
  } else {
    return Color((unsigned char)(brightness * 255), (unsigned char)(p * 255), (unsigned char)(q * 255));
  }
}