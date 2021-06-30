# Control Award (Software Version)

- roadrunner, odemetry
- tensorflow ring dection,

---

- Autonomous objectives:

  - find how many rings are in a stack
  - select what path to use depending on how many rings are in a stack
  - then collect up to three rings from the stack
  - stoping on the white line and placing the wobble goal
  - and aligning the robot with the goal and shooting all the rings in the shooter

---

- Key algorithms:

  - we use tensorflow for ring Identification

---

- Driver controlled enhancements:

  - odemetry is used for auto alignment for shooting at power shots in teleop

---

- Sensors Used:
  - camera is used for ring Identification on the field
  - camera is used for Identifiying what path we should run in auto
  - encoders connected to a wheel are odemetry pod's are used for recording our position on the field

---

```text
at the start of the match the robot detects starter stack

and depending on the amount will effect what path the robot will run.

then the robot will collect up to three rings, if a stack

has more then 3 it will leave the fourth ring for later then it will drive up to target zone and deposit the wobble goal then it will

drive backwards until it is behind the scoring line then

it will rev up the fly wheels in the shooter while also using PID to make sure the fly wheels are at a spcific speed if so then it will shoot all the rings

into the goal, then it will drive towards the last ring and collect it and rotate facing the goal

then we drive up to the scoring line and shoot the last ring. for autonomous vision tracking we

use tensorflow and what tensorflow does is it allows us to track all the rings and where there located on the field

also for positioning of the drive train and auto aim we use odemetry pods with roadrunner which is more accurate than drive base encoders

because the only torque they experience is from ground travel so the robot can find where it is on the field at all times and a
```
