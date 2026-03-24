// FILE: system_manager.h
#pragma once
#include "can_driver.h"
#include "can_ring.h"
#include "gvret.h"

class SystemManager
{
public:
    void setup();
    void loop();

private:
    CANDriver driver;
    CANRing<1024> rxRing;
    GVRET gvret;
};