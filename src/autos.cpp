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

void fireDropdownDelayed(int dropdownWait, int pullbackWait) {
    auto* args = new DropdownPullbackArgs{dropdownWait, pullbackWait};
    vex::task t(delayedDropdownTask, args);
}

void fireIntakeDelayed(int waitTime) {
    auto* ptr = new int(waitTime);
    vex::task t(delayedIntakeTask, ptr);
}

//Function definitions:
    //Drive.moveDistance(distance (in), maxSpeed (pct), timeout (sec), bool correctHeading)
    //Drive.turn(angle (deg), maxSpeed (pct), timeout (sec))
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
  wing1.set(true);
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
  Drive.turn(190,100,.9);
  fireDropdownDelayed(950,1600);
  Drive.swing(59.85,90,170,1.8);
  intakescoring.stop();
  // Drive.moveDistance(20.1,100,.8,true);
  // Drive.turn(176,85,.4);
  // intakescoring.stop();
  // Drive.moveDistance(39.5,100,1.35,true);
  Drive.turn(133,100,.7);
  intakescoring.spin(reverse, 18, percent);
  bottomintake.spin(reverse,30,pct);
  Drive.moveDistance(-19.3,80,.4,true);
  intakescoring.spin(reverse, 95, percent);
  bottomintake.spin(fwd,80,pct);
  Drive.turn(135, 80, 1.1);
  bottomintake.stop();
  wait(.2,sec);
  hood.set(true);
  bottomintake.spin(fwd,30,pct);
  fireDropdownDelayed(700,2000);
  intakescoring.stop();
  Drive.moveDistance(50, 100, 1.25, true);
  hood.set(false);
  Drive.turn(90,80,.65);
  bottomintake.spin(fwd,100,pct);
  intakescoring.spin(fwd,10,pct);
  Drive.moveDistance(67, 60, 1, true);
  intakescoring.stop();
  bottomintake.spin(fwd,70,pct);
  Drive.turn(89,100,.35);
  hood.set(true);
  fireIntakeDelayed(375);
  Drive.moveDistance(-70,90,1.65);

//Amninder and Argamond Were here frfr sigma
  wait(15, sec);
 };

void autorightcomplex(){
  wing1.set(true);
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
  wing1.set(false);
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
  wing1.set(true);
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
  wing1.set(false);
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
  wing1.set(true);  
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
  wing1.set(false);
  Drive.turn(270,85,.6); 
  Drive.moveDistance(5.25,80,.4);
  Drive.turn(190,85,.5);
  Drive.moveDistance(-13,50,.5,true);
  Drive.turn(155,100,10000);
  wait(15, sec);
}

void autorightsimple(){
  vex::task colorSortThread(colorSortTask, nullptr);
  // vex::task colorSortThread(colorSortTask, nullptr);
  wing1.set(true);
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
  wing1.set(false);
  Drive.turn(180,85,.6); 
  Drive.moveDistance(5.15,80,.4);
  Drive.turn(100,85,.5);
  Drive.moveDistance(-13,50,.5,true);
  Drive.turn(55,100,10000);

  wait(15, sec);
};

void autoleftsimple(){
  vex::task colorSortThread(colorSortTask, nullptr);
  wing1.set(true);
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
  Drive.moveDistance(-36, 20, 1.3, true);
  Drive.moveDistance(10.5,50,.75);
  Drive.moveDistance(-1000, 100, 2, true);

  wait(15, sec);
};


void autoleftfinals(){
  fireDropdownDelayed(725,2300);
  Drive.moveDistance(32.7,100,1.1,true);
  //could cut (cut .2)
  Drive.turn(-90,80,.6);
  bottomintake.spin(fwd,100,pct);
  Drive.moveDistance(30,40,.8,true);
  Drive.turn(-92,100,.45);
  bottomintake.spin(fwd,65,pct);
  hood.set(true);
  fireIntakeDelayed(500);
  Drive.moveDistance(-90,85,1.65,true);
  intakescoring.stop();
  hood.set(false);
  bottomintake.spin(fwd,100,pct);
  Drive.turn(-190,100,.9);
  fireDropdownDelayed(190,750);
  Drive.swing(17,50,-225,1.4);
  Drive.turn(-45,100,1);
  intake.spin(reverse, 10, percent);
  Drive.moveDistance(-17.75, 40, 1.2, true);
  bottomintake.spin(fwd,80,pct);
  intakescoring.spin(reverse, 100, percent);
  Drive.moveDistance(-3,5,.9);
  hood.set(true);
  intake.stop();
  Drive.swing(26,100,80,1.2);
  Drive.moveDistance(10,50,.5,true);
  Drive.turn(135,100,10000);
  wait(15,sec);
}

void autorightfinals(){



  wait(15,sec);
}

void autosSkills(){

  //MATCH LOADER 1
  //12.85 sec
  wing1.set(true);
  Drive.moveDistance(46.75, 75, 1.5,true);
  intake.spin(fwd,100,pct);
  Drive.turn(-90,100, .5);
  lilwillpiston.set(true);
  wait(150,msec);
  Drive.moveDistance(100,80.5,3);
  //Drive.turn(-90,100,.25);
  double Ydist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(FrontDis.objectDistance(mm)/25.4+5.25)); //21 13 100
  double Xdist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(RightDis.objectDistance(mm)/25.4+4.25));
  double temphead=adjustang(((180/M_PI)*atan2((24-Xdist), (21-Ydist)))-90); 
  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1,1);
  Controller1.Screen.print(Xdist);
  Controller1.Screen.newLine();
  Controller1.Screen.print(Ydist);

  Controller1.Screen.newLine();
  Controller1.Screen.print(adjustang(temphead));
  Drive.turn(adjustang(temphead), 90, .8);
  Drive.moveDistance(-1*sqrt(pow(24-Xdist, 2)+pow(21-Ydist, 2)), 100,1.5,true); //read the documentation.
  //Drive.moveDistance(-7,100,.75,true);
  Drive.turn(-65,100,.4);
  Drive.moveDistance(-35,100,1.5,true);
  Drive.turn(-90,85,.6);
  Drive.moveDistance(-48,100,1.65,true);
  lilwillpiston.set(false);
  intake.stop();
  Drive.turn(-145,100,.5);
  Drive.moveDistance(-16.75,100,1.25,true);
  Drive.turn(90,100,1.3);

  //SCORING1
  //3.2 sec
  fireIntakeDelayed(850);
  wing1.set(false);
  hood.set(true);
  Ydist = 141-std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(FrontDis.objectDistance(mm)/25.4+5.25)); //21 13 100
  Xdist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(LeftDis.objectDistance(mm)/25.4+4.25));
  temphead=adjustang(((180/M_PI)*atan2((22-Xdist), (21-Ydist)))-90); 
   Drive.moveDistance(-1*sqrt(pow(22-Xdist, 2)+pow(97-Ydist, 2)),50,2.5);
  //Drive.moveDistance(-25,65,3.2);

  //MATCH LOADER
  //5.6 sec
  intake.spin(fwd,100,pct);
  lilwillpiston.set(true);
  Drive.moveDistance(23,100,1.5,true);
  Drive.turn(90,80,.5);
  wing1.set(true);
  hood.set(false);
  Drive.moveDistance(1000, 60, 3.2);
  //Drive.turn(90,100,.6);


  //SCORING2
  //4.25 sec
 //// Drive.moveDistance(-22,100,1.25,true);
  lilwillpiston.set(false);
  wing1.set(false);
  hood.set(true);
  //Drive.moveDistance(-25,50,3);
  Ydist = 141-std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(FrontDis.objectDistance(mm)/25.4+5.25)); //21 13 100
   Xdist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(LeftDis.objectDistance(mm)/25.4+4.25));
   temphead=adjustang(((180/M_PI)*atan2((24-Xdist), (97-Ydist)))-90); 
   Drive.turn(temphead, 65,.5);
   Drive.moveDistance(-1*sqrt(pow(24-Xdist, 2)+pow(97-Ydist, 2)),60,1.5);
  //GOING TO BLUE PARK ZONE
  //7.3 sec
  intake.spin(fwd,100,pct);
  Drive.moveDistance(.1,100,.1);
  Drive.turn(110,100,.75);
  Drive.swing(35,100,180,1.5);
  wing1.set(true);
  hood.set(false);
  Drive.turn(173,100,.45);
  fireDropdownDelayed(90,350);
  fireDropdownDelayed(1450,3400);
  Drive.moveDistance(69,57.5,3,true);
  Drive.moveDistance(-13,15,1.5); //ZEROING ON ZONE

  //MID GOAL
  //6.35 sec
  Drive.turn(271, 100,.8);
  Drive.moveDistance(42.375,80,1.2,true);
  intake.stop();
  Drive.turn(135,80,.75);
  intake.spin(reverse,100,pct);
  wait(.1,sec);
  intake.stop();
  Drive.moveDistance(-14.65,60,1.5,true);
  intakescoring.spin(reverse,80,pct);
  intakebottom.spin(fwd,100,pct);
  intaketop.spin(fwd,100,pct);
  wait(1500,msec);
  intakescoring.spin(reverse,60,pct);
  wait(500,msec);
  lilwillpiston.set(true);
  wing1.set(true);
  hood.set(false);

  //MATCH LOAD 3
  //13.45
  intake.spin(fwd,100,pct);
  Drive.moveDistance(52.8,100,1.75);
  Drive.turn(90,100,.5);
  wait(200,msec);
  Drive.moveDistance(20,75,3);
  Drive.moveDistance(-7,100,.75,true);
  Drive.turn(115,100,.75);
  Drive.moveDistance(-35,100,1.5,true);
  Drive.turn(90,100,.6);
  Drive.moveDistance(-48,100,1.65,true);
  lilwillpiston.set(false);
  intake.stop();
  Drive.turn(35,100,.5);
  Drive.moveDistance(-22.5,100,1.5,true);
  Drive.turn(-90,100,1.3);

  //SCORING 3
  //4.55 sec
  hood.set(true);
   Ydist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(FrontDis.objectDistance(mm)/25.4+5.25)); //21 13 100
   Xdist = 141-std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(LeftDis.objectDistance(mm)/25.4+4.25));
   temphead=adjustang(((180/M_PI)*atan2((117-Xdist), (47-Ydist)))-90); 
   Drive.turn(temphead, 65,.5);
   Drive.moveDistance(-1*sqrt(pow(117-Xdist, 2)+pow(47-Ydist, 2)),50,3);
  //Drive.moveDistance(-20,100,1.25,true);
  lilwillpiston.set(false);
  wing1.set(false);
  intake.spin(fwd,100,pct);
  Drive.moveDistance(-25,50,3);
  Drive.turn(-90,100,.3);

  //MATCH LOADER
  //5.1 sec
  intake.spin(fwd,100,pct);
  lilwillpiston.set(true);
  Drive.moveDistance(20,100,1);
  Drive.turn(-90,100,.3);
  wing1.set(true);
  hood.set(false);
  Drive.moveDistance(1000, 90,3.2);
  Drive.turn(-90,100,.6);

  //SCORING 4
  //4.55 sec
  hood.set(true);
  Ydist = std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(FrontDis.objectDistance(mm)/25.4+5.25)); //21 13 100
   Xdist = 141-std::abs((sin(Inertial.heading(degrees)*M_PI/180))*(LeftDis.objectDistance(mm)/25.4+4.25));
   temphead=adjustang(((180/M_PI)*atan2((117-Xdist), (47-Ydist)))-90); 
   Drive.turn(temphead, 65,.5);
   Drive.moveDistance(-1*sqrt(pow(117-Xdist, 2)+pow(47-Ydist, 2)),50,3);
  //Drive.moveDistance(-20,100,1.25,true);
  lilwillpiston.set(false);
  wing1.set(false);
  intake.spin(fwd,100,pct);
  Drive.moveDistance(-25,50,3);
  Drive.turn(-90,100,.3);

  //GOING TO BLUE PARK ZONE
  //5.8 sec
  intake.spin(fwd,100,pct);
  Drive.moveDistance(.1,100,.1);
  Drive.turn(-70,100,.75);
  Drive.swing(35,100,0,1.5);
  wing1.set(true);
  hood.set(false);
  Drive.turn(-7,100,.45);
  fireDropdownDelayed(90,1000);
  Drive.moveDistance(45,57.5,3,true);










  
  wait(10000, sec);
  //autoskills
};

void autostest(){
  Drive.moveDistance(36,100,100,true,true);
  Drive.turn(90,100,10);

wait (10000,sec);
}