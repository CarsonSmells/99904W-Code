#pragma once

extern vex::color colorsortcolor;
void autosTest();
void autorightcomplex();
void autorightseven();
void autoleftcomplex();
void autorightsimple();
void autoleftsimple();
void autosSkills();

struct DropdownPullbackArgs {
    int dropdownWait;
    int pullbackWait;
};

struct PistonTaskArgs {
    int waitBeforeFire; // ms to wait before firing
    int fireDuration;   // ms to keep piston open
};