#ifndef PID_H
#define PID_H
/**
 * This PID algorithn uses clamping for integral anti-windup and lowpass filtering on
 * derivative control.
 * 
 * Timing is not accounted for so the code assumes execution within a real-time task.
 */

class PID {
    public:
        /**
         * Constructor for a PID controller
         * 
         * Output saturation limits default to 0 and 100
         * 
         * @param input pointer to feedback signal
         * @param setpoint pointer to setpoint
         * @param output pointer to actuation output 
         * @param Ts sampling period in seconds
         * @param Kp proportional gain
         * @param Ki integral gain
         * @param Kd derivative gain
         * @param N filter time constant for derivative
         */
        PID(double *input, double *setpoint, double *output, double Ts, double Kp, 
            double Ki, double Kd, double N = 10);

        /**
         * Modify saturation limits for the control output
         * 
         * @param output_min minimum saturation value for the control output
         * @param output_out maximum saturation value for the control output
         */
        void set_saturation_limits(double output_min, double output_out);

        /**
         * Computes the actuator output of the current timestep
         */
        void compute_output();

        void set_Kp(double Kp);

        void set_Ki(double Ki);

        void set_Kd(double Kd);

        void set_N(double N);

        /**
         * Modify the feedback variable for the computation of the error signal
         */
        void set_input(double *input);

        /**
         * Modify the variable containing the desired output value
         */
        void set_setpoint(double *setpoint);

        /**
         * Modify the variable containing the actuator output
         */
        void set_output(double *output);

    private:
        void clamp(double output, double error);

        double *input;
        double *setpoint;
        double *output;
        double output_max;
        double output_min;
        double Ts;
        double Kp;
        double Ki;
        double Ai;
        double Kd;
        double N;
        double Ad;
        double Bd;

        double integral;    
        double derivative;
        double input_prev;
        double output_prev;
        double error_prev;

        bool clamping;
};



#endif