/*** file hall_rpm.h ***/

#ifndef HALL_RPM_H
#define HALL_RPM_H

#include "driver/pcnt.h"

void hall_rpm_init(void);
int hall_rpm_get_rpm(void);

#endif
