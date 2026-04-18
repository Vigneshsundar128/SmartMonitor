#include "pzem_driver.h"
#include "lcd_display.h"
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

static SoftwareSerial pzemSerial(D6, D7); // RX, TX
static PZEM004Tv30 pzem(pzemSerial);

// internal storage
static float g_voltage=0, g_current=0, g_power=0, g_energy=0, g_frequency=0, g_pf=0;

void pzem_begin() {
  pzemSerial.begin(9600);
  // optionally verify communication here
}

void pzem_update() {
  // read periodically
  g_voltage   = pzem.voltage();
  g_current   = pzem.current();
  g_power     = pzem.power();
  g_energy    = pzem.energy();
  g_frequency = pzem.frequency();
  g_pf        = pzem.pf();
}

float pzem_get_voltage() { return g_voltage; }
float pzem_get_current() { return g_current; }
float pzem_get_power() { return g_power; }
float pzem_get_energy() { return g_energy; }
float pzem_get_frequency() { return g_frequency; }
float pzem_get_pf() { return g_pf; }

// wrapper used by display
void energy_display() {
  char line[21];

  lcd_write_line(0, String("Energy Meter ") + char(4));

  snprintf(line, sizeof(line), "V:%5.1f C:%5.2f", pzem_get_voltage(), pzem_get_current());
  lcd_write_line(1, line);

  snprintf(line, sizeof(line), "P:%5.1f E:%5.1f", pzem_get_power(), pzem_get_energy());
  lcd_write_line(2, line);

  snprintf(line, sizeof(line), "F:%4.1f PF:%4.2f", pzem_get_frequency(), pzem_get_pf());
  lcd_write_line(3, line);
}
