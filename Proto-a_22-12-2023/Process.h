//#####################################################

#include "AkaPID.h"

PIDdata pidRateRoll;
PIDdata pidRatePitch;
PIDdata pidRateYaw;

PIDdata pidStableRateRoll;
PIDdata pidStableRatePitch;

PIDdata pidStableRoll;
PIDdata pidStablePitch;

//##########################################################################
//  Fonction de controle du drone
//  - pas de paramètre d'entrée
//  - pas de valeur retournée
//##########################################################################
void controleurSetup() 
{
//--------------------------------------------------------------------------
  //PID VALUES
  pidRateRoll.P = ROLL_PITCH_P;
  pidRateRoll.I = ROLL_PITCH_I;
  pidRateRoll.D = ROLL_PITCH_D;
  pidRateRoll.lastError = 0;
  pidRateRoll.previousPIDTime = 0;
  pidRateRoll.integratedError = 0;
  pidRateRoll.pGuard = ROLL_PITCH_P_GUARD;
  pidRateRoll.windupGuard = ROLL_PITCH_I_GUARD;

  pidRatePitch.P = ROLL_PITCH_P;
  pidRatePitch.I = ROLL_PITCH_I;
  pidRatePitch.D = ROLL_PITCH_D;
  pidRatePitch.lastError = 0;
  pidRatePitch.previousPIDTime = 0;
  pidRatePitch.integratedError = 0;
  pidRatePitch.pGuard = ROLL_PITCH_P_GUARD;
  pidRatePitch.windupGuard = ROLL_PITCH_I_GUARD;

  pidRateYaw.P = YAW_P;
  pidRateYaw.I = YAW_I;
  pidRateYaw.D = YAW_D;
  pidRateYaw.lastError = 0;
  pidRateYaw.previousPIDTime = 0;
  pidRateYaw.integratedError = 0;
  pidRateYaw.pGuard = 1000;
  pidRateYaw.windupGuard = 1000;

//--------------------------------------------------------------------------
  //Stable mode rates
  pidStableRateRoll.P = ROLL_PITCH_P;
  pidStableRateRoll.I = 0;
  pidStableRateRoll.D = ROLL_PITCH_D;
  pidStableRateRoll.lastError = 0;
  pidStableRateRoll.previousPIDTime = 0;
  pidStableRateRoll.integratedError = 0;
  pidStableRateRoll.pGuard = ROLL_PITCH_P_GUARD;
  pidStableRateRoll.windupGuard = ROLL_PITCH_I_GUARD;

  pidStableRatePitch.P = ROLL_PITCH_P;
  pidStableRatePitch.I = 0;
  pidStableRatePitch.D = ROLL_PITCH_D;
  pidStableRatePitch.lastError = 0;
  pidStableRatePitch.previousPIDTime = 0;
  pidStableRatePitch.integratedError = 0;
  pidStableRatePitch.pGuard = 100;
  pidStableRatePitch.windupGuard = 500;

  pidStableRoll.P = 1;
  pidStableRoll.I = 0;
  pidStableRoll.D = 0;
  pidStableRoll.lastError = 0;
  pidStableRoll.previousPIDTime = 0;
  pidStableRoll.integratedError = 0;
  pidStableRoll.pGuard = 100;
  pidStableRoll.windupGuard = 500;

  pidStablePitch.P = 1;
  pidStablePitch.I = 0;
  pidStablePitch.D = 0;
  pidStablePitch.lastError = 0;
  pidStablePitch.previousPIDTime = 0;
  pidStablePitch.integratedError = 0;
  pidStablePitch.pGuard = 100;
  pidStablePitch.windupGuard = 500;
}

//##########################################################################
double getExpo(long input, float expo, int deviation) 
{
  float expoPitch = map(input, 1000, 2000, -1000, 1000) / 1000.0;
  if (expoPitch >= 0) return pow(abs(expoPitch), expo) * deviation;
  return -pow(abs(expoPitch), expo) * deviation;
}

//##########################################################################
void controllerLoop()
{
  // Commutation de mode accro/stable
//  if ( Aux1.getScaledValue() < 1400) stableMode = false;
//  else stableMode = true;

  if ( Aux2.getScaledValue() < 1400) camMode = HIGH;
  else camMode = LOW;
  digitalWrite(CAM_PIN, camMode);

  if (signalPresent) 
  {
  // Apply exponentials, more sensitive controls around centered sticks
  targetYaw = getExpo(Yaw.getValue(), EXPO_YAW, 60);
  targetPitch = getExpo(Pitch.getValue(), EXPO_ROLL_PITCH, 60);
  targetRoll = getExpo(Roll.getValue(), EXPO_ROLL_PITCH, 60);
  }
  else 
  {
    stableMode = 1;
    targetYaw = 0;
    targetPitch = 0;
    targetRoll = 0;
  }

  if (stableMode) 
  {
    //Stable mode input
    outputRoll = updatePID(targetRoll, yprAngle[2], &pidStableRoll);
    outputPitch = updatePID(targetPitch, yprAngle[1], &pidStablePitch);
//    outputYaw = updatePID(targetYaw, yprAngle[0], &pidStableYaw);
    //Gyro
    outputRoll = updatePID(outputRoll, gx, &pidStableRateRoll);
    outputPitch = updatePID(outputPitch, -gy, &pidStableRatePitch);
//    outputYaw = updatePID(targetYaw, gz, &pidStableRateYaw);

    //Reset error on rate mode
    pidRateRoll.integratedError = 0;
    pidRatePitch.integratedError = 0;
    pidRateYaw.integratedError = 0;

  } else 
  {
    outputRoll = updatePID(targetRoll, -gx, &pidRateRoll);
    outputPitch = updatePID(targetPitch, gy, &pidRatePitch);
    outputYaw = updatePID(targetYaw, -gz, &pidRateYaw);
  }
  
  outputRoll = constrain(outputRoll, -500, 500);
  outputPitch = constrain(outputPitch, -500, 500);
  outputYaw = constrain(outputYaw, -500, 500);
}

//##########################################################################
