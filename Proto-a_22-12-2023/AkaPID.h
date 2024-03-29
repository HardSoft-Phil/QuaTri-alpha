#ifndef _PID_h
#define _PID_h

//// PID Variables
struct PIDdata 
{
  double P, I, D;
  double lastError;
  // AKA experiments with PID
  double previousPIDTime;
  double integratedError;
  double windupGuard;
  double pGuard;
};

//##########################################################################
// AKA PID experimentation
//##########################################################################
double updatePID(double targetPosition, double currentPosition, struct PIDdata *PIDparameters) {

  // AKA PID experiments
  const double deltaPIDTime = (currentTime - PIDparameters->previousPIDTime) / 1000000.0;

  PIDparameters->previousPIDTime = currentTime;
  double error = targetPosition - currentPosition;

  if (etat == MARCHE) 
  {
    PIDparameters->integratedError += error * deltaPIDTime;
  } else 
  {
    PIDparameters->integratedError = 0.0;
    PIDparameters->lastError = 0.0;
  }

  PIDparameters->integratedError = constrain(PIDparameters->integratedError, -PIDparameters->windupGuard, PIDparameters->windupGuard);
  double dTerm = PIDparameters->D * (currentPosition - PIDparameters->lastError) / (deltaPIDTime * 100); // dT fix from Honk
  PIDparameters->lastError = currentPosition;

  return constrain(PIDparameters->P * error, -PIDparameters->pGuard, PIDparameters->pGuard) + (PIDparameters->I * PIDparameters->integratedError) + dTerm;
}

//##########################################################################
#endif
