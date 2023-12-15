#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    double kp;   // Proportional gain
    double ki;   // Integral gain
    double kd;   // Derivative gain
    double prev_error; // Previous error
    double integral;   // Integral term
    double max_output; // Max output limit (Anti-windup)
    double min_output; // Min output limit (Anti-windup)
    bool first_run;    // Flag to check if it's the first run (for D term)
} PIDController;

// Initialize the PID controller
void initPID(PIDController *pid, double kp, double ki, double kd, double min_output, double max_output) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->prev_error = 0.0;
    pid->integral = 0.0;
    pid->max_output = max_output;
    pid->min_output = min_output;
    pid->first_run = true;
}

// Update the PID controller
double updatePID(PIDController *pid, double setpoint, double measured_value, double dt) {
    double error = setpoint - measured_value;
    double Pout, Iout, Dout;

    // Proportional term
    Pout = pid->kp * error;

    // Integral term with Anti-windup
    pid->integral += error * dt;
    Iout = pid->ki * pid->integral;
    if (Iout > pid->max_output) Iout = pid->max_output;
    else if (Iout < pid->min_output) Iout = pid->min_output;

    // Derivative term
    if (!pid->first_run) {
        Dout = pid->kd * (error - pid->prev_error) / dt;
    } else {
        Dout = 0.0;
        pid->first_run = false;
    }

    // Total output with Clamping
    double output = Pout + Iout + Dout;
    if (output > pid->max_output) output = pid->max_output;
    else if (output < pid->min_output) output = pid->min_output;

    // Save error for next iteration
    pid->prev_error = error;

    return output;
}

#endif // PID_CONTROLLER_H

