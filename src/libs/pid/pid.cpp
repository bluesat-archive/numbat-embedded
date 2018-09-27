#include "pid.h"
#include <cmath>

PID::PID(double *input, double *setpoint, double *output, double Ts, double Kp, 
        double Ki, double Kd, double N) {
    this->input = input;
    this->setpoint = setpoint;
    this->output = output;
    this->Ts = Ts;
    this->Kp = Kp;
    this->Ki = Ki;
    this->Ai = Ki * Ts;
    this->Kd = Kd;
    this->N = N;
    // precompute constant factors for derivative calculation
    this->Ad = 1.0/(1+N*Ts);
    this->Bd = Kd*N/(1+N*Ts);
    output_min = 0;
    output_max = 100;
    integral = 0;
    derivative = 0;
    error_prev = 0;
    clamping = false;
}

void PID::set_saturation_limits(double output_min, double output_max) {
    this->output_max = output_max;
    this->output_min = output_min;
}

void PID::compute_output() {
    double error = *setpoint - *input;
    double proportional = Kp * error;
    // only integrate if not clamped
    if (!clamping) {
        integral += Ai * error;
    }
    // lowpass filter on derivative
    derivative = Ad * derivative + Bd * (error - error_prev);
    double out = proportional + integral + derivative;
    // check if we need to saturate and clamp
    if (out > output_max) {
        out = output_max;
        clamp(out, error);
    } else if (out < output_min) {
        out = output_min;
        clamp(out, error);
    }
    *output = out;
    error_prev = error;
}

void PID::clamp(double output, double error) {
    // stop integration if output and error have same sign 
    if (std::signbit(output) == std::signbit(error)) {
        clamping = true;
    } else { // otherwise re-enable integration
        clamping = false;
    }
}

void PID::set_Kp(double Kp) {
    this->Kp = Kp;
}

void PID::set_Ki(double Ki) {
    this->Ki = Ki;
    this->Ai = Ki * Ts;

}

void PID::set_Kd(double Kd) {
    this->Kd = Kd;
    this->Bd = Kd*N/(1+N*Ts);
}

void PID::set_N(double N) {
    this->N = N;
    this->Ad = 1.0/(1+N*Ts);
    this->Bd = Kd*N/(1+N*Ts);
} 

void PID::set_input(double *input) {
    this->input = input;
}

void PID::set_setpoint(double *setpoint) {
    this->setpoint = setpoint;
}

void PID::set_output(double *output) {
    this->output = output;
}