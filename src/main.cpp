/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Carson Prolly                                             */
/*    Created:      8/13/2025, 2:17:15 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//HOW TO COMMIT:

// first: git add .   
// second: git commit -m "title"
//third: git push

#include "vex.h"
#include "robotConfig.h"
#include "library.h"
#include "autos.h"
#include <cmath>

using namespace vex;

// A global instance of competition
competition Competition;


/*---------------------------------------------------------------------------*/
/*                                   Tasks                                   */
/*---------------------------------------------------------------------------*/
int videoTask() {

  const char* frames[] = {"funny_001.bmp", "funny_002.bmp", "funny_003.bmp", "funny_004.bmp", "funny_005.bmp", "funny_006.bmp", "funny_007.bmp", "funny_008.bmp", "funny_009.bmp", "funny_010.bmp", "funny_011.bmp", "funny_012.bmp", "funny_013.bmp"};
    int numFrames = sizeof(frames)/sizeof(frames[0]);
  while (true) {
    for (int i = 0; i < numFrames; i++) {
      Brain.Screen.drawImageFromFile(frames[i], 0, 0);
      this_thread::sleep_for(20);
    }
  }
  return 0;
}

volatile bool colorsortingEnabled = true;
const char* myword = ""; 
int printoncontroller() {
  static double startTime = Brain.timer(seconds);
  while (true) {
    double left_temp= leftdrive.temperature(fahrenheit);
    double right_temp = rightdrive.temperature(fahrenheit);
    double bottom_temp_ = intakebottom.temperature(fahrenheit);
    double score_temp_ = intakescoring.temperature(fahrenheit);
    if (colorsortingEnabled){
      myword = "T";
    } else {
      myword = "F";
    }

    Controller1.Screen.clearScreen();
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("L: %.0f R: %.0f CS: %s", left_temp, right_temp, myword);
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("B: %.0f S: %.0f", bottom_temp_, score_temp_);

    double runtime = Brain.timer(seconds) - startTime;
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("(%.1f deg, %d:%02d, %d%%)", Inertial.heading(), (int)(runtime)/60, (int)(runtime) % 60, (int)Brain.Battery.capacity());
    this_thread::sleep_for(500);
  }
  return 0;
}

volatile bool parkTaskActive = false;
volatile bool firsttime = true;
int parkTask() {
  parkTaskActive = true;

  // Set brakes at the start
  rightfront.setBrake(vex::brakeType::hold);
  rightmid.setBrake(vex::brakeType::hold);
  rightback.setBrake(vex::brakeType::hold);
  leftfront.setBrake(vex::brakeType::hold);
  leftmid.setBrake(vex::brakeType::hold);
  leftback.setBrake(vex::brakeType::hold);
  intakebottom.setBrake(vex::brakeType::hold);

  while (parkTaskActive) {

    if (distanceSensor.objectDistance(vex::distanceUnits::mm) <= 60 && distanceSensor.objectDistance(vex::distanceUnits::mm) >= 40 && firsttime) {
      firsttime = false;
      bottomintake.spinTo(bottomintake.position(deg)-92, deg, 20, vex::velocityUnits::pct, true);
      bottomintake.stop();
      leftdrive.stop();
      rightdrive.stop();
      parkmech.set(true);
      parkTaskActive = false;
    } else{
      if (leftdrive.velocity(pct) < 1) {
        firsttime = true;
        intakescoring.spin(reverse, 100, percent);
        bottomintake.spin(reverse, 40, percent);
      };
    }
    this_thread::sleep_for(20);
  }
  return 0;
}

volatile bool myTaskActive = false;
int firePistonTask() {
  myTaskActive = true;
  intakescoring.spin(reverse, 100, percent); 
  wait(50, msec);
  myTaskActive = false;
  return 0;
}

int endgameAlertTask() {
  // Wait for 85 seconds (for a 1:45 match, endgame is last 20s)
  wait(85, seconds);
  Controller1.rumble("- -"); // Two short rumbles
  return 0;
}

volatile bool driveBackTaskActive = false;
int drivebacktask() {
  while (driveBackTaskActive) {
    leftdrive.spin(reverse, 100, percent);
    rightdrive.spin(reverse, 100, percent);
  }
  return 0;
}


/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Calibrate inertial sensor
  Inertial.calibrate(3);
  TopOpticalSensor.setLightPower(50, pct);
  Brain.Screen.setFillColor(285);
  Brain.Screen.drawRectangle(0, 0, 480, 240); 

  // SD card check
  if (!Brain.SDcard.isInserted()) {
    Brain.Screen.printAt(10, 40, "ERROR: SD card not detected!");
    Brain.Screen.printAt(10, 60, "Peak is disabled.");
  }
}

/*---------------------------------------------------------------------------*/
/*                              Autonomous Task                              */
/*---------------------------------------------------------------------------*/

//option+arrows to move selection up
void autonomous(void) {
  colorsortcolor = vex::color::green;
  autoleftfinals();
  autorighthook(); //done tested (cm)
  autolefthook(); //done tested (CM)
  autorightsimple();
  solosig();
  autoleftsimple();
  autostest();
  autorightcomplex();
  autosSkills();
  autorightnine();
 
  //^^auton selector (put the code u wanna run up top.)
}

/*---------------------------------------------------------------------------*/
/*                              User Control Task                            */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // --- Variable definitions ---
  bool intakeJammed = false;
  uint32_t lastJamTime = 0;
  const uint32_t jamReverseDuration = 200; // ms to reverse to clear jam
  const uint32_t jamDetectCooldown = 1000; // ms between jam checks
  vex::timer slowTimer;

  vex::color colorsortcolor = vex::color::red; // THIS IS THE COLOR IT SPITS OUT. CHANGE HERE. ####################################################
  uint32_t intakeStartTime = 0;
  bool lastIntakePressed = false;

  // --- Task callupons ---
  vex::task endgameTask(endgameAlertTask);
  vex::task tempPrintThread(printoncontroller);
  vex::task* videoThread = nullptr;
  vex::task* driveBackThread = nullptr;
  
  if (Brain.SDcard.isInserted()) {
    videoThread = new vex::task(videoTask);
  }

  // --- Toggle states ---
  bool lilwill = false;
  if (lilwillpiston.value()){
    lilwill = true;
  }

  bool winglift = false;
  if (wing1.value()){
    winglift = true;
  }
  
  bool colorsort = false;
  if (colorsortpiston.value()){
    colorsort = true;
  }

  //checking hood state after auto
  bool hoodstate = false;
  if (hood.value()){
    hoodstate = true;
  }

  bool lastPressA = false;
  bool parkmechs = false;
  bool lastPressB = false;
  bool lastPressDown = false;
  bool lastPressX = false;
  bool lastPressL2 = false;
  bool lastPressPark = false;
  bool lastPressWing = false;
  bool midgoallast = false;

  //set brake types for redundancy
  rightfront.setBrake(vex::brakeType::coast);
  rightmid.setBrake(vex::brakeType::coast);
  rightback.setBrake(vex::brakeType::coast);
  leftfront.setBrake(vex::brakeType::coast);
  leftmid.setBrake(vex::brakeType::coast);
  leftback.setBrake(vex::brakeType::coast);

  while (true) {

    // if (colorsortingEnabled) {
    //         // --- Color Sorting ---
    //     if (TopOpticalSensor.color() == colorsortcolor && TopOpticalSensor.isNearObject()) {
    //           vex::task firePistonThread(firePistonTask);
    //         }
    //   } else {
    //     Controller1.rumble("-                         ");
    //   }

    bool intakeButtonPressed = Controller1.ButtonR1.pressing() || Controller1.ButtonL1.pressing();
      if (intakeButtonPressed) {
        if (!lastIntakePressed) {
          intakeStartTime = Brain.timer(msec);
        }
        lastIntakePressed = true;
      }

    // --- Drive Controls ---
    if (!parkTaskActive) {
      if (Controller1.ButtonR1.pressing()) {    
        // Only check for jams after 1000ms of running
        if (!intakeJammed && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
          intakeJammed = true;
          lastJamTime = Brain.timer(msec);
        }

        if (intakeJammed) {
          // Reverse intake to clear jam
          bottomintake.spin(reverse, 100, percent);
          // After reverse duration, stop reversing
          if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
            intakeJammed = false;
            lastJamTime = Brain.timer(msec);
          }
        } else {
            bottomintake.spin(forward, 100, percent);
            if (midgoallast) {
              slowTimer.reset();
              midgoallast = false;
            }
          if ((!TopOpticalSensor.isNearObject() or hood.value()) && (!myTaskActive)) {
            intakescoring.spin(fwd, 100, pct);
            slowTimer.reset();

            if (hood.value()) {
              Controller1.rumble("---            ");
            }

          } else {
            // Object detected → delayed slow
            if (slowTimer.time(msec) <= 120 and !myTaskActive) {
              intakescoring.spin(fwd, 100, pct);
            } else {
              intakescoring.spin(fwd, .001, pct);
            }
          }

        }

      } else if (Controller1.ButtonR2.pressing()) {
        bottomintake.spin(reverse,100,pct);
        intakescoring.spin(reverse,100,pct);
        midgoallast = true;

      } else if (Controller1.ButtonL1.pressing()) {

        if (!intakeJammed && (Brain.timer(msec) - intakeStartTime > 1000) && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
          intakeJammed = true;
          lastJamTime = Brain.timer(msec);
        }

        if (intakeJammed) {
          // Reverse intake to clear jam
          intake.spin(reverse, 100, percent);
          // After reverse duration, stop reversing
          if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
            intakeJammed = false;
            lastJamTime = Brain.timer(msec);
          }
        } else {
          bottomintake.spin(fwd,80,pct);
          intakescoring.spin(reverse, 100, percent);
          midgoallast = true;
          
     
        } 

      } else if (Controller1.ButtonDown.pressing()) {
        bottomintake.spin(fwd,85,pct);
        intakescoring.spin(reverse,35,pct);
        slowTimer.reset();


      } else {
        lastIntakePressed = false;
        intakeStartTime = 0;
        bottomintake.stop();
        intakescoring.stop();

        // if (!myTaskActive) {
        //   bottomintake.stop();
        // }
      }
    if (!driveBackTaskActive) {
      float rightspeed = (Controller1.Axis2.value() * abs(Controller1.Axis2.value())) / 100;
      float leftspeed = (Controller1.Axis3.value() * abs(Controller1.Axis3.value())) / 100;
      leftdrive.spin(forward, leftspeed, percent);
      rightdrive.spin(forward, rightspeed, percent);
    }
  }

      //toggle for color sorting
     if (Controller1.ButtonX.pressing()) {
        if (!lastPressX) {
          colorsortingEnabled = !colorsortingEnabled;
          lastPressX = true;   // mark as handled
        }
      } else {
        lastPressX = false;    // reset on release
      }

    // --- Little Will dropdown code ---
    if (Controller1.ButtonL2.pressing()) {
      if (!lastPressL2) {
        lilwill = !lilwill;
        lilwillpiston.set(lilwill);
      }
      lastPressL2 = true;
    } else {
      lastPressL2 = false;
    }

    // --- Toggle for wing pistons ---
    if (Controller1.ButtonY.pressing()) {
      if (!lastPressWing) {
        winglift = !winglift;
        wing1.set(winglift);
      }
      lastPressWing = true;
    } else {
      lastPressWing = false;
    }

    // --- Toggle for parking mechanism ---
    static vex::task* parkTaskThread = nullptr;
    if (Controller1.ButtonRight.pressing()) {
      if (!lastPressPark) {
        if (parkTaskThread == nullptr) {
          // Start the parking task
          parkTaskThread = new vex::task(parkTask);
          parkmechs = true;
        } else {
          // Stop the parking task
          parkTaskActive = false; // Signal the task to stop
          parkTaskThread->stop();
          delete parkTaskThread;
          parkTaskThread = nullptr;
          parkmechs = false;
          parkmech.close();
          // Set drive and intake motors back to coast
          rightfront.setBrake(vex::brakeType::coast);
          rightmid.setBrake(vex::brakeType::coast);
          rightback.setBrake(vex::brakeType::coast);
          leftfront.setBrake(vex::brakeType::coast);
          leftmid.setBrake(vex::brakeType::coast);
          leftback.setBrake(vex::brakeType::coast);
          intakebottom.setBrake(vex::brakeType::coast);
        }
        lastPressPark = true;
      }
    } else {
      lastPressPark = false;
    }

if (Controller1.ButtonB.pressing()) {
      if (!lastPressB) {
        hoodstate = !hoodstate;
        hood.set(hoodstate);
      }
      lastPressB = true;
    } else {
      lastPressB = false;
    }

    if (Controller1.ButtonA.pressing()) {
      if (!lastPressA) {
        colorsort = !colorsort;
        colorsortpiston.set(colorsort);
      }
      lastPressA = true;
    } else {
      lastPressA = false;
    }
// --- Start drive backward task with Down Arrow ---
if (Controller1.ButtonDown.pressing()) {
  if (!lastPressDown) {
    intaketop.spin(fwd,100,pct);
    intakescoring.spin(reverse,50,pct);
    intakescoring.spin(fwd,100,pct);
    lastPressDown = true;
  }
} else {
  // Stop drive backward when button released
  if (driveBackThread != nullptr) {
    driveBackTaskActive = false;
    driveBackThread->stop();
    delete driveBackThread;
    driveBackThread = nullptr;
    leftdrive.stop();
    rightdrive.stop();
  }
  lastPressDown = false;
}

    wait(20, msec);
  }
}


int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
  while (true) {
    wait(100, msec);
  }
}
