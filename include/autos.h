#pragma once

extern vex::color colorsortcolor;
void solosig();
void autorightsplit();
void autorightseven();
void autorighthook();
void autolefthook();
void autorightsimple();
void autoleftseven();
void autosSkills();
void autostest();
void autoleftfinals();
void autorightfinals();
void autoleftfinalsalt();

struct DropdownPullbackArgs {
    int dropdownWait;
    int pullbackWait;
};

struct PistonTaskArgs {
    int waitBeforeFire; // ms to wait before firing
    int fireDuration;   // ms to keep piston open
};