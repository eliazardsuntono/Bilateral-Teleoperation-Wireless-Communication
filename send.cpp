#include <SPI.h>
#include <ACAN2517FD.h>
#include <Moteus.h>

// ================================
// CHANGE THESE FOR YOUR HARDWARE
// ================================
static const byte CAN_CS_PIN  = 10;   // chip select for MCP2517FD board
static const byte CAN_INT_PIN = 2;    // interrupt pin from MCP2517FD board

// Use the SPI bus your Teensy is wired to.
// Usually just SPI on Teensy.
ACAN2517FD can(CAN_CS_PIN, SPI, CAN_INT_PIN);

// moteus servo ID (default is often 1, but use whatever yours is configured to)
Moteus moteus1(can, []() {
  Moteus::Options options;
  options.id = 2;
  return options;
}());

Moteus::PositionMode::Command cmd;
Moteus::PositionMode::Format fmt;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting moteus test...");

  SPI.begin();

  // Most official examples use 1 Mbps arbitration and 1 Mbps data
  // CHANGE OSC_20MHz to OSC_40MHz if CAN-FD board uses a 40 MHz crystal.
  ACAN2517FDSettings settings(
    ACAN2517FDSettings::OSC_20MHz,   // or OSC_40MHz
    1000UL * 1000UL,                 // 1 Mbps arbitration rate
    DataBitRateFactor::x1            // 1 Mbps data rate
  );

  settings.mArbitrationSJW = 2;
  settings.mDriverTransmitFIFOSize = 1;
  settings.mDriverReceiveFIFOSize = 2;

  const uint32_t errorCode = can.begin(settings, [] { can.isr(); });

  if (errorCode != 0) {
    Serial.print("CAN init failed, error = 0x");
    Serial.println(errorCode, HEX);
    while (true) { delay(1000); }
  }

  Serial.println("CAN initialized.");

  // Clear any existing faults / stop state
  moteus1.SetStop();
  delay(100);

  // Ask moteus to return some telemetry fields
  fmt.position = Moteus::kFloat;
  fmt.velocity = Moteus::kFloat;
  fmt.torque = Moteus::kFloat;

  // Optional motion limits
  fmt.velocity_limit = Moteus::kFloat;
  fmt.accel_limit = Moteus::kFloat;

  cmd.velocity_limit = 1.0;   // rev/s
  cmd.accel_limit = 2.0;      // rev/s^2

  Serial.println("Setup complete.");
}

void loop() {
  cmd.position = 0.25;
  moteus1.SetPosition(cmd, &fmt);

  printTelemetry("Moved to +0.25");
  delay(2000);

  cmd.position = 0.0;
  moteus1.SetPosition(cmd, &fmt);

  printTelemetry("Moved to 0.0");
  delay(2000);
}

void printTelemetry(const char* label) {
  const auto& values = moteus1.last_result().values;

  Serial.print(label);
  Serial.print(" | pos=");
  Serial.print(values.position);
  Serial.print(" | vel=");
  Serial.print(values.velocity);
  Serial.print(" | torque=");
  Serial.println(values.torque);
}
