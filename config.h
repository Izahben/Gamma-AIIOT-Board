#pragma once

// Secrets (Wi-Fi password etc.) live in include/secrets.h, which is git-ignored.
// If it doesn't exist yet, the build falls back to the safe placeholder values.
#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #include "secrets.example.h"
#endif

#define GAMMA_FW_VERSION "gamma-v0.1"

// ---- Access point ----
#define AP_SSID         "Gamma"
// AP_PASS comes from secrets.h
#define AP_CHANNEL      6
#define AP_MAX_CLIENTS  4             // dashboard IP is always http://192.168.4.1

// ---- Devices ----
#define MAX_DEVICES     24
#define NAME_MAX_LEN    23
#define LED_ACTIVE_LOW  0             // 1 if your LED is wired GPIO -> LED -> 3V3
#define RELAY_ACTIVE_LOW 0            // 1 if your relay module switches ON when its IN pin is LOW

// ---- Board ----
// 1 = module has octal PSRAM (N8R8, N16R8...): GPIO33-37 are reserved.
// 0 = quad/no PSRAM (N4, N8, N16R2...): GPIO33-37 become usable.
#define OCTAL_PSRAM     1
