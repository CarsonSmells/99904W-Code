#include "vex.h"
#include "robotConfig.h"
#include "autos.h"
#include <cmath>

double adjustang(double ang)
{
  if(ang-Inertial.rotation(degrees)>180)
  {
    while (ang-Inertial.rotation(degrees)>180)

    {
      ang-=360;
    }
  }else if(ang-Inertial.rotation(degrees)<-180)
  {
    while (ang-Inertial.rotation(degrees)<-180)
    {
      ang+=360;
    }
    
  }
  return ang;
}
  


int intakeJamTask(void* args) {
    static bool intakeJammed = false;
    uint32_t lastJamTime = 0;
    const uint32_t jamReverseDuration = 500; // ms to reverse to clear jam
    const uint32_t jamDetectCooldown = 600; // ms between jam checks

    while (true) {
        // Only check for jams after 200ms of running
        if (!intakeJammed && (Brain.timer(msec) - lastJamTime > jamDetectCooldown) && !LowerOpticalSensor.isNearObject() && std::abs(intakebottom.velocity(vex::velocityUnits::pct)) <= 4) {
            intakeJammed = true;
            lastJamTime = Brain.timer(msec);
        }   

        if (intakeJammed) {
            // Reverse intake to clear jam
            intakebottom.spin(reverse, 100, percent);
            // After reverse duration, stop reversing and restart intake forward
            if (Brain.timer(msec) - lastJamTime > jamReverseDuration) {
                intakeJammed = false;
                lastJamTime = Brain.timer(msec);
                intakebottom.spin(forward, 100, percent); // Restart intake forward
            }
        }

        wait(20, msec);
    }
    return 0;
}

// int hoodtask(void* args) {
//   static bool ballinhood;
//   while (true) {
//   if (TopOpticalSensor.isNearObject() && intakescoring.velocity(pct) >= 70){
//     intakescoring.spin(fwd,10,pct); // hi carson, you smell -chase
//   }
//   };
//   wait(20,msec);
// }

volatile bool TaskActive = false;
int colorsortpistontask() {
  TaskActive = true;
  intakebottom.spin(forward, 100, percent);
  colorsortpiston.open();     // Open the piston
  wait(350, msec);             // Keep it open for 350ms
  colorsortpiston.close();
  TaskActive = false;
  return 0;
}

int colorSortTask(void* args) {
    uint32_t lastColorSortTime = 0;
    const uint32_t colorSortCooldown = 350; // ms between sorts

    while (true) {
        if (LowerOpticalSensor.color() == colorsortcolor &&
            (Brain.timer(msec) - lastColorSortTime > colorSortCooldown) &&
            LowerOpticalSensor.isNearObject()) {
            vex::task firePistonThread(colorsortpistontask); // Fire piston
            lastColorSortTime = Brain.timer(msec);
        }
        wait(20, msec);
    }
    return 0;
}

int delayedIntakeTask(void* waitTimePtr) {
    int waitTime = *(int*)waitTimePtr; // Dereference the pointer
    wait(waitTime, msec);
    intake.spin(forward, 100, percent);
    return 0;
}

int delayedDropdownTask(void* argsPtr) {
    DropdownPullbackArgs* args = (DropdownPullbackArgs*)argsPtr;
    wait(args->dropdownWait, msec);
    lilwillpiston.set(true);   // Fire dropdown
    // Now wait for the pullback delay BEFORE retracting
    wait(args->pullbackWait, msec); // Wait before retracting
    lilwillpiston.set(false);  // Retract dropdown
    delete args; // Clean up if allocated with new
    return 0;
}

int balltopscoring() {
  intakescoring.spin(fwd, 100, pct);
  if (!TopOpticalSensor.isNearObject()) {
    waitUntil(TopOpticalSensor.isNearObject() || hood.value());
    wait(100,msec);
  }
  intakescoring.stop();
  return 0;
}

void fireDropdownDelayed(int dropdownWait, int pullbackWait) {
    auto* args = new DropdownPullbackArgs{dropdownWait, pullbackWait};
    vex::task t(delayedDropdownTask, args);
}

void fireIntakeDelayed(int waitTime) {
    auto* ptr = new int(waitTime);
    vex::task t(delayedIntakeTask, ptr);
}

void loadball() {
  vex::task loadBallThread(balltopscoring);
}

//Function definitions:
    //Drive.moveDistance(distance (in), maxSpeed (pct), timeout (sec), bool correctHeading, bool autoskip)
    //Drive.turn(angle (deg), maxSpeed (pct), timeout (sec), bool autoskip)
    //drive.swing(targetDistance (in),  maxSpeed (pct),  targetAngle (deg), timeout (sec))

//Tasks
 
    //delayeddropdown
    //   fireDropdownDelayed(1500, 1000); // Wait 1.5s, then fire dropdown, then wait 1s before pullback

    // delayedintake
    //   fireIntakeDelayed(2000); // Wait 2 seconds, then fire intake
  

//change color here!!!!
vex::color colorsortcolor;

void solosig(){
  vex::task colorSortThread(colorSortTask, nullptr);
  hook.set(true);
  fireDropdownDelayed(650,2300);
  Drive.moveDistance(32,100,1.2,true,true);
  //could cut (cut .2)
  Drive.turn(90,100,.7,true);
  loadball();
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(18.25,50,.6,true);
  Drive.turn(90.5,100,.4);
  bottomintake.spin(fwd,60,pct);
  intakescoring.stop();
  hood.set(true);
  fireIntakeDelayed(400);
  Drive.moveDistance(-35,85,1,true,true);
  wait(.65,sec);
  intake.stop();
  hood.set(false);
  bottomintake.spin(fwd,100,pct);
  Drive.turn(186,100,.9,true);
  loadball();
  fireDropdownDelayed(950,1600);
  Drive.swing(60,85,170,1.8);
  intakescoring.stop();
  // Drive.moveDistance(20.1,100,.8,true);
  // Drive.turn(176,85,.4);
  // intakescoring.stop();
  // Drive.moveDistance(39.5,100,1.35,true);
  Drive.turn(133,100,.7);
  bottomintake.spin(reverse,25,pct);
  intakescoring.spin(reverse, 30, percent);
  Drive.moveDistance(-19.35,80,.4,true,true);
  intakescoring.spin(reverse, 100, percent);
  bottomintake.spin(fwd,80,pct);
  Drive.turn(135, 80, .7);
  bottomintake.stop();
  wait(.4,sec);
  loadball();
  fireDropdownDelayed(500,2250);
  bottomintake.spin(reverse,10,pct);
  Drive.moveDistance(52, 80, 1.25, true, true);
  Drive.turn(90,100,.65,true);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(67, 60, .7, true);
  intakescoring.stop();
  bottomintake.spin(fwd,50,pct);
  Drive.turn(89.5,100,.35);
  hood.set(true);
  fireIntakeDelayed(450);
  Drive.moveDistance(-45,80,1,true,true);
  wait(.5,sec);

//Amninder and Argamond Were here frfr sigma
  wait(15, sec);
 };

void autorightcomplex(){
  hook.set(true);
  Drive.moveDistance(43.9,85,1.2,true);
  bottomintake.spin(fwd,100,pct);
  lilwillpiston.set(true);
  Drive.turn(90,80,.8);
  //cut
  Drive.moveDistance(18,65,.75,true);
  Drive.turn(88,100,.5);
  bottomintake.spin(fwd,70,pct);
  intakescoring.stop();
  hood.set(true);
  fireIntakeDelayed(550);
  //from 600
  Drive.moveDistance(-42,65,2);
  //can cut(cut.45)
  lilwillpiston.set(false);
  intake.stop();
  hood.set(false);
  hook.set(false);
  Drive.turn(145,100,.5);
  Drive.moveDistance(11,80,.7);
  Drive.turn(90,85,.5);
  Drive.moveDistance(-26,60,10,true);
  wait(15, sec);
};

void autorightnine(){
  fireDropdownDelayed(650, 1250);
  intake.spin(forward, 100, percent);
  intakescoring.spin(fwd,60,pct);
  Drive.swing(42.4, 100, 40, 1.5);
  Drive.turn(57, 100, .6);
  fireDropdownDelayed(700,1250);
  Drive.moveDistance(25, 42, 1.2, true);
  Drive.swing(-22.2, 78, 0, 1.6);
  Drive.turn(124, 100, .7);
  fireDropdownDelayed(300, 10000000);
  Drive.swing(41, 55, 180, 1.75);
  Drive.moveDistance(25, 90, 1.2, true);
  Drive.turn(180, 100, .5);
  intake.stop();
  hood.set(true);
  Drive.moveDistance(-25, 100, .8, true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-27, 30, 3.5, true);
  intake.stop();
  Drive.moveDistance(7, 75, .5, true);
  Drive.moveDistance(-10, 100, 100, true);
  wait(15, sec);  
};

void autolefthook(){
  vex::task colorSortThread(colorSortTask, nullptr);
  hook.set(true);
  fireDropdownDelayed(635, 1000); // Drop after 0.75 seconds, pullback after 10 seconds
  bottomintake.spin(forward, 100, percent);
  Drive.swing(45, 100, -35, 1.7);
  bottomintake.spin(fwd,50,pct);
  Drive.turn(-132, 80, 1);
  intakescoring.spin(reverse, 20, percent);
  bottomintake.spin(reverse,18,pct);
  Drive.moveDistance(-19,80,.75,true); 
  intakescoring.spin(reverse, 95, percent);
  bottomintake.spin(fwd,85,pct);
  Drive.turn(-130, 80, 1.15);
  hood.set(true);
  intake.stop();
  Drive.moveDistance(49.75,80,1.6, true); //match loader 
  hood.set(false);
  lilwillpiston.set(true);
  bottomintake.spin(fwd,100,pct); 
   fireDropdownDelayed(450,1100);
  intakescoring.spin(fwd,55,pct);
  Drive.turn(-180,55,.75);
  Drive.moveDistance(35, 65, .75);
  Drive.turn(-182, 85, .65);
  intakescoring.spin(fwd,15,pct);
  bottomintake.spin(fwd,30,pct);
  hood.set(true);
  lilwillpiston.set(false);
  Drive.moveDistance(-38, 60, .75, true);
  fireIntakeDelayed(0) ; // intake after 0.1 seconds
  Drive.moveDistance(-36, 20, 1.3, true);
  hook.set(false);
  Drive.turn(-90,85,.6); 
  hood.set(false);
  intakescoring.stop();
  Drive.moveDistance(5.25,80,.4);
  Drive.turn(-170,85,.5);
  Drive.moveDistance(-18,67,1000000000000,true);

 // Drive.swing(11,80,-10,1);
  //Drive.turn(-180,85,.5);
  //Drive.moveDistance(-22,100,100);
  //intake.stop();
  wait(15, sec);
};


void autorighthook(){
  vex::task colorSortThread(colorSortTask, nullptr);
  hook.set(true);  
  fireDropdownDelayed(650, 1000);
  fireIntakeDelayed(450);
  Drive.swing(44, 85, 40, 1.5);
  bottomintake.stop();
  intakescoring.stop();
  Drive.turn(-45, 65,.8);
  bottomintake.spin(reverse,70,pct);
  Drive.moveDistance(9,80,.8,true);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(-5.25,80,.55,true);
  Drive.turn(132, 85,.7);
  fireDropdownDelayed(1300,10000000);
  Drive.moveDistance(39,100,1.4,true); //1.2
  bottomintake.spin(fwd,100,pct);
  Drive.turn(180,85,.7);
  Drive.moveDistance(35, 50, .8, true);
  Drive.turn(182, 100, .4);
  intake.stop();
  bottomintake.spin(fwd,45,pct);
  hood.set(true);
  fireIntakeDelayed(640) ; 
  Drive.moveDistance(-37.5, 60, .75, true);
  lilwillpiston.set(false);
  Drive.moveDistance(-36, 20, .9);
  hook.set(false);
  Drive.turn(270,85,.6); 
  Drive.moveDistance(5.25,80,.4);
  Drive.turn(190,85,.5);
  hood.set(false);
  Drive.moveDistance(-13,50,.5,true);
  Drive.turn(155,100,10000);
  wait(15, sec);
}

void autorightsimple(){
  vex::task colorSortThread(colorSortTask, nullptr);
  // vex::task colorSortThread(colorSortTask, nullptr);
  hook.set(true);
  fireDropdownDelayed(600,2300);
  Drive.moveDistance(31.5,100,.9);
  //could cut (cut .2)
  Drive.turn(90,80,.6);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(18.1,50,.65,true);
  Drive.turn(93,100,.5);
  bottomintake.spin(fwd,70,pct);
  intakescoring.stop();
  hood.set(true);
  fireIntakeDelayed(400);
  Drive.moveDistance(-90,85,1.65,true);
  intake.stop();
  hood.set(false);
  bottomintake.spin(fwd,100,pct);
  hook.set(false);
  Drive.turn(180,85,.6); 
  Drive.moveDistance(5.15,80,.4);
  Drive.turn(100,85,.5);
  Drive.moveDistance(-13,50,.5,true);
  Drive.turn(55,100,10000);

  wait(15, sec);
};

void autoleftsimple(){
  vex::task colorSortThread(colorSortTask, nullptr);
  hook.set(true);
  fireDropdownDelayed(600, 100000000); // Drop after 0.75 seconds, pullback after 10 seconds
  intake.spin(forward, 100, percent);
  intakescoring.spin(fwd,50,pct);
  Drive.swing(42.65, 100, -41, 1.8);
  intaketop.spin(fwd,50,pct);
  Drive.turn(-134, 80, 1);
  intakescoring.stop();
  Drive.moveDistance(-20.7, 35, 1.2, true);
  intake.spin(reverse, 100, percent);
  wait(.2, sec);
  bottomintake.spin(fwd,80,pct);
  intakescoring.spin(reverse, 100, percent);
  Drive.moveDistance(-3,5,.8);
  Drive.turn(-135, 80, 1);
  hood.set(true);
  fireIntakeDelayed(500);
  Drive.moveDistance(51.95,80,1.65,true);
  hood.set(false);
  Drive.turn(-180,55,.75);
  Drive.moveDistance(19, 60, 1.2);
  Drive.turn(-179, 85, .55);
  intakescoring.spin(fwd,15,pct);
  bottomintake.spin(fwd,40,pct);
  hood.set(true);
  lilwillpiston.set(false);
  Drive.moveDistance(-33, 60, .75, true);
  fireIntakeDelayed(0) ; // intake after 0.1 seconds
  Drive.moveDistance(-36, 20, 1.3, true,true);
  Drive.moveDistance(10.5,50,.75);
  Drive.moveDistance(-1000, 100, 1.5, true,true);
  Drive.swing(-20,80,90, 1);
  fireDropdownDelayed(60, 1000000);
  Drive.turn(0,85,.6);
  Drive.moveDistance(12,80,.75,true);
  wait(15, sec);
};


void autoleftfinals(){
  fireDropdownDelayed(725,2750);
  Drive.moveDistance(32.2,95,1.1,true,true);
  //could cut (cut .2)
  Drive.turn(-90,100,.6,true);
  fireIntakeDelayed(200);
  loadball();
  Drive.moveDistance(30,60,.4,true);
  loadball();
  Drive.turn(-91,100,.5);
  bottomintake.spin(fwd,60,pct);
  hood.set(true);
  fireIntakeDelayed(525);
  Drive.moveDistance(-47,85,1.3,false,true);
  wait(.35, sec);
  bottomintake.stop();
  Drive.turn(-190,100,.9,true);
  bottomintake.spin(fwd,100,pct);
  hood.set(false);
  fireDropdownDelayed(190,750);
  loadball();
  Drive.swing(17,50,-200,.8);
  Drive.turn(-45,100,.9,true);
  intake.spin(reverse, 3, percent);
  Drive.moveDistance(-16.65, 85, .8, true, true);
  bottomintake.spin(fwd,90,pct);
  intakescoring.spin(reverse, 100, percent);
  wait(.6,sec);
  Drive.swing(26.25,100,85,.9);
  hood.set(false);
  Drive.moveDistance(11.5,50,.5,true,true);
  Drive.turn(135,60,10000);
  wait(15,sec);
}

void autoleftfinalsalt(){
  fireDropdownDelayed(725,2750);
  hook.set(true);
  Drive.moveDistance(32.2,95,1.1,true,true);
  //could cut (cut .2)
  Drive.turn(-90,100,.6,true);
  fireIntakeDelayed(200);
  loadball();
  Drive.moveDistance(30,60,.45);
  loadball();
  Drive.turn(-91,100,.4);
  bottomintake.spin(fwd,60,pct);
  hood.set(true);
  fireIntakeDelayed(535);
  Drive.moveDistance(-47,80,1.3,false,true);
  wait(.325, sec);
  bottomintake.stop();
  Drive.turn(-190,100,.9,true);
  hood.set(false);
  loadball();
  bottomintake.spin(fwd,100,pct);
  fireDropdownDelayed(100,750);
  Drive.moveDistance(15,100,1.5,true,true);
  Drive.moveDistance(-11,100,1.5,true,true);
  Drive.turn(adjustang(65),100,.75,true);
  bottomintake.stop();
  hook.set(false); //7.54 sec
  wait(.0,sec); //ADJUST TS U BUM AHHS
  Drive.swing(22,100,110,1); 
  Drive.turn(adjustang(90),100,.8,true);
  hook.set(true);
  Drive.turn(adjustang(230),100,.6,true);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(16,100,.95,true,true);
  intake.spin(reverse, 5, percent);
  Drive.turn(adjustang(-45),100,.8,true);
  Drive.moveDistance(-21, 85, .9, true, true);
  bottomintake.spin(fwd,90,pct);
  intakescoring.spin(reverse, 100, percent);
  wait(.6,sec);
  hook.set(false);
  Drive.swing(26.5,100,92.5,.9);
  hood.set(false);
  wait(15,sec);
}

void autorightfinals(){



  wait(15,sec);
}

void autosSkills(){
  hook.set(true);
  intakescoring.setMaxTorque(100,pct);
  fireDropdownDelayed(0, 500000);
  fireIntakeDelayed(1500); 
  loadball();
  Drive.moveDistance(30, 100, 1.5,true,true);
  Drive.turn(-90,100, 1, true);
  Drive.moveDistance(17.5,50,1.7);
  intakescoring.stop();
  Drive.moveDistance(-7,100,.7,true,true);
  Drive.turn(-65,100,.4);
  Drive.moveDistance(-33,100,1.2,true,true);
  lilwillpiston.set(false);
  Drive.turn(-90.5,85,.6,true);
  Drive.moveDistance(-50,100,1.4,true,true);
  intake.stop();
  Drive.turn(-145,100,.5);
  Drive.moveDistance(-13,100,1.25,true,true);
  Drive.turn(-270,100,1.3,true);
  fireIntakeDelayed(400);
  hood.set(true);
  Drive.moveDistance(-26,65,.6,false,true);
  Drive.turn(-267,100,1.35);
  bottomintake.spin(fwd,100,pct);
  lilwillpiston.set(true);
  Drive.moveDistance(23,100,.8,true);
  loadball();
  hood.set(false);
  Drive.moveDistance(40,60,1,true);
  Drive.turn(-267,100,1);
  bottomintake.spin(fwd,10,pct);
  fireIntakeDelayed(825);
  hood.set(true);
  Drive.moveDistance(-29, 100, 1, true, true);
  Drive.moveDistance(-30, 60, .5);
  wait(.6,sec);
  Drive.turn(-245,100,1);
  lilwillpiston.set(false);
  hood.set(false);
  Drive.swing(30,100,-200,1);
  fireDropdownDelayed(450,1000);
  Drive.moveDistance(21.5,100,1.5,true,true);
  loadball();
  fireDropdownDelayed(1500,2200);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(65,60,3,true,true);
  Drive.turn(-150,100,1,true);
  Drive.swing(-11,30,-182,1); //reset
  Drive.moveDistance(-25,15,1);
  Drive.turn(-90,100,1,true);
  Drive.moveDistance(39.5,100,2,true,true);
  intake.stop();
  Drive.turn(-225,80,.75,true);
  intake.spin(reverse,7.5,pct);
  Drive.moveDistance(-14.75,60,1.5,true,true);
  bottomintake.spin(fwd,60,pct);
  intakescoring.setMaxTorque(75,pct);
  intakescoring.spin(reverse,80,pct);
  wait(500,msec);
  Drive.moveDistance(.5,20,.6);
  intakescoring.setMaxTorque(75,pct);
  intakescoring.spin(reverse,30,pct);
  wait(1500,msec);
  hood.set(true);
  fireDropdownDelayed(800,500000);
  Drive.moveDistance(52,100,2,true,true);
  hood.set(false);
  loadball();
  bottomintake.spin(fwd,100,pct);
  Drive.turn(adjustang(90),100,.5, true);
  Drive.moveDistance(20,60,2.5, true);
  Drive.moveDistance(-7,100,1, true, true);
  Drive.turn(adjustang(45),100,1, true);
  Drive.moveDistance(-21,90,1.6, true, true);
  lilwillpiston.set(false);
  Drive.turn(adjustang(90),100,1, true);
  intake.stop();
  Drive.moveDistance(-60,100,2.5,true,true);
  Drive.turn(adjustang(135),100,1, true);
  Drive.moveDistance(-20,100,1.5,true,true);
  Drive.turn(adjustang(-90),100,1.3, true);
  fireIntakeDelayed(400);
  hood.set(true);
  Drive.moveDistance(-26,65,.6);
  Drive.turn(adjustang(-87),100,1.35);
  fireDropdownDelayed(0,2500);
  Drive.moveDistance(23.5,100,1,true);
  loadball();
  hood.set(false);
  Drive.moveDistance(20,60,1,true);
  Drive.turn(-89,100,.9);
  bottomintake.spin(fwd,20,pct);
  hood.set(true);
  Drive.moveDistance(-28, 100, 1.2,true,true);
  fireIntakeDelayed(300);
  Drive.moveDistance(-12, 60, .5,true);
  wait(.75,sec);
  Drive.turn(-65,100,1);
  Drive.swing(35,100,-10,1);
  fireDropdownDelayed(450,10000);
  Drive.moveDistance(58,100,1.5,true,true);
  //Drive.turn(adjustang(10),100,1,true);
  //hood.set(false);
  //fireDropdownDelayed(100,600);
 // bottomintake.spin(fwd,80,pct);
  //Drive.swing(20,100,45,1.2);
  //intake.spin(reverse,20,pct);
 // Drive.moveDistance(10,80,1.2,true,true);
  //intake.spin(reverse,100,pct);



//   //SCORING2
//   //4.25 sec
//  //// Drive.moveDistance(-22,100,1.25,true);
//   lilwillpiston.set(false);
//   hood.set(true);
//   //Drive.moveDistance(-25,50,3);
  
//   //GOING TO BLUE PARK ZONE
//   //7.3 sec
//   intake.spin(fwd,100,pct);
//   Drive.moveDistance(.1,100,.1);
//   Drive.turn(110,100,.75);
//   hood.set(false);
//   Drive.turn(173,100,.45);
//   fireDropdownDelayed(90,350);
//   fireDropdownDelayed(1450,3400);
//   Drive.moveDistance(69,57.5,3,true);
//   Drive.moveDistance(-13,15,1.5); //ZEROING ON ZONE

//   //MID GOAL
//   //6.35 sec
//   Drive.turn(271, 100,.8);
//   Drive.moveDistance(42.375,80,1.2,true);
//   intake.stop();
//   Drive.turn(135,80,.75);
//   intake.spin(reverse,100,pct);
//   wait(.1,sec);
//   intake.stop();
//   Drive.moveDistance(-14.65,60,1.5,true);
//   intakescoring.spin(reverse,80,pct);
//   intakebottom.spin(fwd,100,pct);
//   intaketop.spin(fwd,100,pct);
//   wait(1500,msec);
//   intakescoring.spin(reverse,60,pct);
//   wait(500,msec);
//   lilwillpiston.set(true);
//   hood.set(false);

//   //MATCH LOAD 3
//   //13.45
//   intake.spin(fwd,100,pct);
//   Drive.moveDistance(52.8,100,1.75);
//   Drive.turn(90,100,.5);
//   wait(200,msec);
//   Drive.moveDistance(20,75,3);
//   Drive.moveDistance(-7,100,.75,true);
//   Drive.turn(115,100,.75);
//   Drive.moveDistance(-35,100,1.5,true);
//   Drive.turn(90,100,.6);
//   Drive.moveDistance(-48,100,1.65,true);
//   lilwillpiston.set(false);
//   intake.stop();
//   Drive.turn(35,100,.5);
//   Drive.moveDistance(-22.5,100,1.5,true);
//   Drive.turn(-90,100,1.3);

//   //SCORING 3
//   //4.55 sec
//   hood.set(true);
  
//   //Drive.moveDistance(-20,100,1.25,true);
//   lilwillpiston.set(false);
//   intake.spin(fwd,100,pct);
//   Drive.moveDistance(-25,50,3);
//   Drive.turn(-90,100,.3);

//   //MATCH LOADER
//   //5.1 sec
//   intake.spin(fwd,100,pct);
//   lilwillpiston.set(true);
//   Drive.moveDistance(20,100,1);
//   Drive.turn(-90,100,.3);
//   hood.set(false);
//   Drive.moveDistance(1000, 90,3.2);
//   Drive.turn(-90,100,.6);

//   //SCORING 4
//   //4.55 sec
//   hood.set(true);


//   Drive.moveDistance(-20,100,1.25,true);
//   lilwillpiston.set(false);
//   intake.spin(fwd,100,pct);
//   Drive.moveDistance(-25,50,3);
//   Drive.turn(-90,100,.3);

  // //GOING TO BLUE PARK ZONE
  // //5.8 sec
  // intake.spin(fwd,100,pct);
  // Drive.moveDistance(.1,100,.1);
  // Drive.turn(-70,100,.75);
  // Drive.swing(35,100,0,1.5);
  // hood.set(false);
  // Drive.turn(-7,100,.45);
  // fireDropdownDelayed(90,1000);
  // Drive.moveDistance(45,57.5,3,true);

  wait(10000, sec);
  //autoskills
};

void autostest(){
  Drive.turn(180,100,5,true);
  Drive.moveDistance(36,100,5,true,true);
  Drive.turn(0,100,10,true);
  Drive.moveDistance(36,100,5,true,true);
  Drive.turn(90,100,10,true);



wait (10000,sec);
}