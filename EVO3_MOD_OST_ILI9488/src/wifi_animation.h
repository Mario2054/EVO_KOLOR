#include "EvoDisplayCompat.h"
#ifndef WIFI_ANIMATION_H
#define WIFI_ANIMATION_H

#include <LovyanGFX.hpp>

// Funkcja animacji gwiaździstego nieba podczas łączenia WiFi
void wifiStarsAnimation(LGFX *display, int duration_ms = 2000);

// Funkcja animacji trwającej do momentu połączenia z WiFi
void wifiStarsAnimationUntilConnected(LGFX *display, unsigned long maxDuration_ms = 180000);

// Funkcja pomocnicza - pojedyncza klatka animacji
void drawStarField(LGFX *display, int frameNumber);

#endif // WIFI_ANIMATION_H
