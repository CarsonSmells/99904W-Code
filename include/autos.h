#pragma once

extern vex::color colorsortcolor;
void solosig();
void autorightcomplex();
void autorightnine();
void autorighthook();
void autolefthook();
void autorightsimple();
void autoleftsimple();
void autosSkills();
void autostest();
void autoleftfinals();
void autorightfinals();

struct DropdownPullbackArgs {
    int dropdownWait;
    int pullbackWait;
};

struct PistonTaskArgs {
    int waitBeforeFire; // ms to wait before firing
    int fireDuration;   // ms to keep piston open
};