#pragma once
#include <Arduino.h>
#include "EvoDisplayCompat.h"

struct EvoUiState {
  String station;
  String track;
  String codec;
  String bitrate;
  String sampleRate;
  String bits;
  int volume;
  int bank;
  int vuL;
  int vuR;
  int peakL;
  int peakR;
  int wifiLevel;
  bool online;
};

void evoV12DrawPanel(uint8_t panelId, const EvoUiState& s);
void evoV12DrawOverviewPage1(const EvoUiState& s);
void evoV12DrawOverviewPage2(const EvoUiState& s);
