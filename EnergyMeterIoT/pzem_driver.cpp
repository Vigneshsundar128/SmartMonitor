#include "pzem_driver.h"
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
  lcd_set_cursor(4,0);
  lcd_print("Energy Meter");
  lcd_write_custom(4);
  lcd_set_cursor(0,1);
  lcd_printf("V:%.1fV", pzem_get_voltage());
  lcd_set_cursor(10,1);
  lcd_printf("C:%.2fA", pzem_get_current());
  lcd_set_cursor(0,2);
  lcd_printf("P:%.1fW", pzem_get_power());
  lcd_set_cursor(10,2);
  lcd_printf("E:%.1fkW", pzem_get_energy());
  lcd_set_cursor(0,3);
  lcd_printf("F:%.1fHz PF:%.2f", pzem_get_frequency(), pzem_get_pf());
  delay(1000);
}
