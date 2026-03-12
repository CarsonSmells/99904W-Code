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
    wait(80, msec);
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

static bool done = false;
static int lastjam = 0;
void lowgoal() {
  lilwillpiston.set(true);
  intakelift.set(true);
  done = false;
  vex::timer lowgoaltimer;
  bottomintake.stop();
  intakescoring.spin(reverse,100,pct);
  while (!done) {
    if (lowgoaltimer <= 700) {
      bottomintake.spin(reverse,100,pct);
    } else if (lowgoaltimer <= 2000 && lowgoaltimer > 700) {
      if (std::abs(intakebottom.velocity(vex::velocityUnits::pct)) < 2 && lowgoaltimer - lastjam > 300) {
        bottomintake.spin(fwd,100,pct);
        wait(100, msec);
        lastjam = lowgoaltimer.time(msec);
      } else {
        bottomintake.spin(reverse,30,pct);
      }
    } else if (lowgoaltimer > 2000 && lowgoaltimer <= 4000) {
      bottomintake.spin(reverse,20,pct);

    }
    if (lowgoaltimer >= 4500) {
      lilwillpiston.set(false);
      Drive.moveDistance(-8.5, 100, 1,true,true);
      intakelift.set(false);
      done = true;
    }
  }
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
  hook.set(true);
  fireDropdownDelayed(450,2300);
  Drive.moveDistance(30.2,100,1.1,true,true);
  Drive.turn(90,80,.55,true);
  loadball();
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(22,50,.8,true);
  Drive.turn(92,100,.4);
  bottomintake.spin(fwd,40,pct);
  intakescoring.stop();
  hood.set(true);
  fireIntakeDelayed(435);
  Drive.moveDistance(-40,95,1,true,true);
  Drive.moveDistance(-20,20,.575,true);
  bottomintake.stop();
  hood.set(false);
  bottomintake.spin(fwd,80,pct);
  Drive.turn(185,100,.75,true);
  loadball();
  fireDropdownDelayed(1000,1600);
  Drive.swing(61.5,90,170,1.5);
  Drive.turn(137.5,100,.45,true);
  bottomintake.spin(fwd,30,pct);
  Drive.moveDistance(34.5,70,1,true,true);
  intake.stop();
  Drive.turn(90,100,.45,true);
  hood.set(true);
  fireIntakeDelayed(250);
  Drive.moveDistance(-24,60,.95,true);
  fireDropdownDelayed(500, 1500);
  wait(.65,sec);
  hood.set(false);
  loadball();
  Drive.moveDistance(55,40,1.45,true);
  Drive.moveDistance(-6.5,100,.75,true,true);
  Drive.turn(135,100,.6,true);
  intake.spin(reverse,5,pct);
  Drive.moveDistance(-56.5,90,1.3,true,true);
  intakescoring.spin(reverse,80,pct);
  bottomintake.spin(fwd,80,pct);
  Drive.moveDistance(-1000,10,1000,true);





  // bottomintake.spin(reverse,25,pct);
  // intakescoring.spin(reverse, 30, percent);
  // Drive.moveDistance(-19,80,.6,true,true); 
  // intakescoring.spin(reverse, 100, percent);
  // bottomintake.spin(fwd,80,pct);
  // Drive.turn(138, 80, .7);
  // bottomintake.stop();
  // wait(.4,sec); 
  // loadball();
  // fireDropdownDelayed(500,2250);
  // bottomintake.spin(reverse,12,pct);
  // Drive.moveDistance(53, 80, 1.55, true, true);
  // Drive.turn(90,100,.65,true);
  // bottomintake.spin(fwd,100,pct);
  // Drive.moveDistance(67, 60, .8, true);
  // intakescoring.stop();
  // bottomintake.spin(fwd,50,pct);
  // Drive.turn(89.5,100,.35);
  // hood.set(true);
  // fireIntakeDelayed(450);
  // Drive.moveDistance(-47,80,1,true,true);
  // wait(.5,sec);

//Amninder and Argamond Were here frfr sigma
  wait(15, sec);
 }

void autorightsplit(){
  fireDropdownDelayed(475 , 1200);
  bottomintake.spin(forward, 100, percent);
  loadball();
  Drive.swing(31, 80, 40, 1.3);
  Drive.turn(-47.5, 100, .6,true);
  intake.spin(reverse,100,pct);
  Drive.moveDistance(11,80,.8,true,true);
  intake.spin(reverse,50,pct);
  wait(1.5,sec);
  Drive.moveDistance(-11,100,.55,true,true);
  Drive.turn(140, 100, 1.25,true);
  bottomintake.spin( forward, 100, percent);
  Drive.moveDistance(43.5, 100, 1.4 ,true);
  lilwillpiston.set(true);
  Drive.turn(180,85,.7,true);
  bottomintake.spin(forward, 100, percent);
  Drive.moveDistance(30, 50, .6);
  Drive.turn(180,100,.4);
  bottomintake.spin(forward, 50, percent);
  Drive.moveDistance(-45, 100, .75, true,true);
  hood.set(true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-10000,20,1.4);
  lilwillpiston.set(false);
  hood.set(false);
  bottomintake.stop();
  hood.set(false);
  Drive.moveDistance(5,80,.4);
  Drive.moveDistance(-10000,30,10000,true);
  wait(15, sec);  
}


void autolefthook(){
  hook.set(true);
  fireDropdownDelayed(635, 1000); // Drop after 0.75 seconds, pullback after 10 seconds
  bottomintake.spin(forward, 100, percent);
  loadball();
  Drive.swing(45, 100, -35, 1.7);
  bottomintake.spin(fwd,50,pct);
  Drive.turn(-132, 80, 1,true);
  intakescoring.spin(reverse, 20, percent);
  bottomintake.spin(reverse,18,pct);
  Drive.moveDistance(-19,80,.75,true); 
  intakescoring.spin(reverse, 95, percent);
  bottomintake.spin(fwd,85,pct);
  Drive.turn(-130, 80, 1.15,true);
  intake.stop();
  Drive.moveDistance(49.75,80,1.6, true); //match loader 
  lilwillpiston.set(true);
  bottomintake.spin(fwd,100,pct); 
  fireDropdownDelayed(450,1100);
  Drive.turn(-180,55,.75);
  loadball();
  Drive.moveDistance(35, 65, .75);
  Drive.turn(-182, 85, .65);
  bottomintake.spin(fwd,30,pct);
  hood.set(true);
  lilwillpiston.set(false);
  Drive.moveDistance(-45, 100, .75, true,true);
  hood.set(true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-10000,20,1.25);
  hood.set(false);
  bottomintake.stop();
  Drive.turn(-90,85,.6); 
  hood.set(false);
  hook.set(false);
  intakescoring.stop();
  Drive.moveDistance(5.5,80,.5,true,true);
  Drive.turn(-170,85,.5);
  Drive.moveDistance(-18,67,1000000000000,true);
  
  // Drive.swing(11,80,-10,1);
  //Drive.turn(-180,85,.5);
  //Drive.moveDistance(-22,100,100);
  //intake.stop();
  wait(15, sec);
}


void autorighthook(){
  hook.set(true);  
  fireDropdownDelayed(650, 1000);
  fireIntakeDelayed(450);
  Drive.swing(42, 85, 40, 1.5);
  bottomintake.stop();
  intakescoring.stop();
  Drive.turn(-44, 65,.8);
  bottomintake.spin(reverse,70,pct);
  Drive.moveDistance(9.5,80,.8,true);
  wait(.5,sec);
  bottomintake.spin(reverse,100,pct);
  Drive.moveDistance(-5.75,80,.55,true);
  Drive.turn(132, 85,.7);
  fireDropdownDelayed(1300,10000000);
  loadball();
  Drive.moveDistance(41,90,1.4,true); //1.2
  bottomintake.spin(fwd,100,pct);
  Drive.turn(180,85,.7);
  Drive.moveDistance(50, 50, .9, true);
  Drive.turn(181, 100, .4);
  intake.stop();
  bottomintake.spin(fwd,45,pct);
  hood.set(true);
  fireIntakeDelayed(640) ; 
  Drive.moveDistance(-37.5, 60, .75, true);
  lilwillpiston.set(false);
  Drive.moveDistance(-36, 20, .9);
  hook.set(false);
  Drive.turn(270,85,.6); 
  Drive.moveDistance(5.75,80,.4);
  Drive.turn(190,85,.5);
  hood.set(false);
  Drive.moveDistance(-13,50,.5,true);
  Drive.turn(155,100,10000);
  wait(15, sec);
}

void autorightsimple(){
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
}

void autorightseven(){
  fireDropdownDelayed(540, 7000);
  bottomintake.spin(forward, 100, percent);
  loadball();
  Drive.swing(34, 80, 40, 1.3);
  Drive.turn(140, 100, .6,true);
  Drive.moveDistance(42.15, 100, 1.4,true);
  Drive.turn(181,85,.7,true);
  bottomintake.spin(forward, 100, percent);
  Drive.moveDistance(30, 50, .7);
  Drive.turn(180,100,.4);
  bottomintake.spin(forward, 50, percent);
  Drive.moveDistance(-45, 100, .75, true,true);
  hood.set(true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-10000,20,1.5);
  hood.set(false);
  bottomintake.stop();
  hood.set(false);
  Drive.moveDistance(5,80,.4);
  Drive.moveDistance(-10000,30,10000,true);
  wait(15, sec);  
}

void autoleftseven(){
  fireDropdownDelayed(550, 7000);
  bottomintake.spin(forward, 100, percent);
  loadball();
  Drive.swing(34, 80, -40, 1.3);
  Drive.turn(-140, 100, .6,true);
  bottomintake.spin(fwd,50,pct);
  Drive.moveDistance(42.15, 100, 1.4,true);
  Drive.turn(-180,85,.7,true);
  bottomintake.spin(forward, 100, percent);
  Drive.moveDistance(30, 50, .7);
  Drive.turn(-180,100,.4);
  bottomintake.spin(forward, 50, percent);
  Drive.moveDistance(-45, 100, .75, true,true);
  hood.set(true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(-10000,20,1.5);
  hood.set(false);
  bottomintake.stop();
  hood.set(false);
  Drive.moveDistance(5,80,.4);
  Drive.moveDistance(-10000,30,10000,true);
  wait(15, sec);  
}


void autoleftfinals(){
  fireDropdownDelayed(725,2750);
  Drive.moveDistance(32.5,95,1.3,true);
  Drive.turn(-90,100,.6,true);
  fireIntakeDelayed(200);
  loadball();
  Drive.moveDistance(40,50,.7,true);
  loadball();
  Drive.turn(-89.5,100,.4);
  bottomintake.spin(fwd,60,pct);
  Drive.moveDistance(-39,85,.8,true,true);
  hood.set(true);
  intake.spin(fwd,100,pct);
  Drive.moveDistance(-40,10,.75,true);
  Drive.turn(-190,100,.9,true);
  hood.set(false);
  fireDropdownDelayed(190,1200);
  Drive.swing(17, 50, -200, .8);
  loadball();
  Drive.turn(-44,100,.9,true);
  bottomintake.stop();
  Drive.moveDistance(-16.95, 85, .6, true, true);
  bottomintake.spin(fwd,80,pct);
  intakescoring.spin(reverse, 80, percent);
  Drive.moveDistance(-16.95, 15, .95, true); //.75
  Drive.swing(26.35,100,85,.9);
  intake.stop();
  hood.set(false);
  Drive.moveDistance(10,100,.5,true,true);
  Drive.swing(3.5,60, 150,1000);
  wait(15,sec);
}

void autoleftfinalsalt(){
  fireDropdownDelayed(725,2750);
  hook.set(true);
  Drive.moveDistance(32.5,95,1.3,true);
  Drive.turn(-90,100,.6,true);
  fireIntakeDelayed(200);
  loadball();
  Drive.moveDistance(40,50,.7,true);
  loadball();
  Drive.turn(-90,100,.4);
  bottomintake.spin(fwd,60,pct);
  Drive.moveDistance(-39,85,.8,true,true);
  hood.set(true);
  intake.spin(fwd,100,pct);
  Drive.moveDistance(-40,10,.7,true);
  bottomintake.stop();
  fireDropdownDelayed(850,1200);
  Drive.turn(-190,100,.9,true);
  hood.set(false);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(15,100,1,true);
  loadball();
  Drive.moveDistance(-11,100,1.4,true,true);
  Drive.turn(adjustang(60),100,.75,true);
  hook.set(false);
  bottomintake.stop();
  hook.set(false); //7.54 sec
  wait(0,sec); //ADJUST TS BUM AHH
  Drive.swing(25,100,110,1); 
  Drive.turn(adjustang(90),100,.8,true);
  hook.set(true);
  Drive.turn(adjustang(230),100,.6,true);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(18,100,1.1,true);
  intake.spin(reverse, 5, percent);
  Drive.turn(adjustang(-45),100,.8,true);
  Drive.moveDistance(-22, 85, .9, true, true);
  bottomintake.spin(fwd,80,pct);
  intakescoring.spin(reverse, 80, percent);
  Drive.moveDistance(-16.95, 15, .95, true); // change, .6
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
  fireDropdownDelayed(500, 500000);
  fireIntakeDelayed(1500); 
  loadball();
  Drive.moveDistance(30.5, 100, 1.2,true,true);
  Drive.turn(-90,80, .7, true);
  Drive.moveDistance(19.5,50,2,true);
  Drive.moveDistance(-7,100,.8,true,true);
  bottomintake.spin(fwd,60,pct);
  Drive.turn(-145,100,.4,true);
  lilwillpiston.set(false);
  intake.stop();
  Drive.moveDistance(-25,60,.75,true,true);
  Drive.turn(-91,100,.6, true);
  Drive.moveDistance(-62.5,80,1.4,true,true);
  Drive.turn(-45,100,.5, true);
  Drive.moveDistance(-19,100,1.1,true,true);
  Drive.turn(adjustang(-270),100,.75,true);
  fireIntakeDelayed(500);
  Drive.moveDistance(-29,100,.5,false,true);
  hood.set(true);
  lilwillpiston.set(true);
  Drive.moveDistance(-100,20,1.1,true);

  Drive.turn(adjustang(-270),100,.8);
  loadball();
  hood.set(false);
  Drive.moveDistance(63,45,1.9,true);
  Drive.turn(adjustang(-269),100,.65);
  bottomintake.spin(fwd,60,pct);
  fireIntakeDelayed(900);
  Drive.moveDistance(-39, 80, .9, true, true);
  hood.set(true);
  Drive.moveDistance(-1000, 20, 1.25);
  Drive.turn(adjustang(-245),100,1,true);
  lilwillpiston.set(false);
  Drive.swing(30,100,adjustang(-205),1);
  hood.set(false);
  fireDropdownDelayed(410,875);
  Drive.moveDistance(20,40,1,true,true);
  bottomintake.spin(fwd,100,pct);
  loadball();
  Drive.moveDistance(23,60,1.75,true);
  loadball();
  Drive.moveDistance(43,47.5,2,true,true);
  loadball();
  Drive.turn(adjustang(-178),100,.75,true);
  Drive.moveDistance(-40,25,1.25);
  wait(100,msec);
  Inertial.setRotation(180, degrees); 
  Drive.turn(adjustang(-87),100,1,true);
  Drive.moveDistance(35,80,1.5,true);
  Drive.turn(adjustang(-9.5),100,1,true);
  Drive.moveDistance(37,80,1.5,true);
  Drive.turn(225,100,.75,true);
  Drive.moveDistance(30,40,1.5,true,true);
  lowgoal();
  waitUntil(done);
  Drive.turn(adjustang(-210),80,.75,true);
  bottomintake.spin(fwd,100,pct);
  hood.set(true);
  intake.spin(forward, 100, percent);
  Drive.moveDistance(72,80,2,true);
  lilwillpiston.set(true);
  Drive.turn(adjustang(90),100,1,true);
  hood.set(false);
  loadball();
  Drive.moveDistance(17.25,40,2,true);
  Drive.moveDistance(-7,100,.8,true,true);
  bottomintake.spin(fwd,60,pct);
  Drive.turn(35,100,.4,true);
  lilwillpiston.set(false);
  intake.stop();
  Drive.moveDistance(-25,60,.75,true,true);
  Drive.turn(89,100,.6, true);
  Drive.moveDistance(-62.5,80,1.4,true,true);
  Drive.turn(135,100,.5, true);
  Drive.moveDistance(-19,100,1.1,true,true);
  Drive.turn(adjustang(270),100,.75,true);
  fireIntakeDelayed(500);
  Drive.moveDistance(-30,85,.5,false,true);
  hood.set(true);
  lilwillpiston.set(true);
  Drive.moveDistance(-100,20,.75,true);
  Drive.turn(adjustang(270),100,.75);
  hood.set(false);
  loadball();
  Drive.moveDistance(55,45,1.8,true);
  Drive.turn(adjustang(271),100,.75);
  bottomintake.spin(fwd,60,pct);
  fireIntakeDelayed(900);
  Drive.moveDistance(-40.5, 80, .9, true);
  hood.set(true);
  lilwillpiston.set(false);
  wait(200,msec);
  Drive.moveDistance(-60, 20, 1.05);
  Drive.turn(adjustang(-65),100,1,true);
  hood.set(false);
  fireDropdownDelayed(900,1100);
  Drive.swing(35,100,adjustang(-5),1);
  intake.spin(reverse, 100, percent);
  Drive.moveDistance(50,60,4,true);
  wait(10000, sec);
};

void autostest(){
  Drive.moveDistance(35,30,1.5,true,true);
  lowgoal();
  waitUntil(done);
  wait (10000,sec);
}