#pragma once

void pzem_begin();
void pzem_update();
void energy_display();

float pzem_get_voltage();
float pzem_get_current();
float pzem_get_power();
float pzem_get_energy();
float pzem_get_frequency();
float pzem_get_pf();

