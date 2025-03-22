#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>

class Component 
{
    protected:
        std::unique_ptr<double> time;       // time array
        std::unique_ptr<double> voltage;    // voltage array
        std::unique_ptr<double> current;    // current array
    public:
        Component(std::unique_ptr<double> time, std::unique_ptr<double> voltage, std::unique_ptr<double> current)
            : time(std::move(time)), voltage(std::move(voltage)), current(std::move(current)) {}
        ~Component() {}

        void setTime(std::unique_ptr<double> time)
        {
            this->time = std::move(time);
        }

        void setVoltage(std::unique_ptr<double> voltage)
        {
            this->voltage = std::move(voltage);
        }

        void setCurrent(std::unique_ptr<double> current)
        {
            this->current = std::move(current);
        }

        double getTime() {return *time;}
        double getVoltage() {return *voltage;}
        double getCurrent() {return *current;}
};

class Capacator: public Component
{
    private:
        std::unique_ptr<double> C; //capacitance value
    public:
        Capacator(std::unique_ptr<double> time, std::unique_ptr<double> voltage, std::unique_ptr<double> current, std::unique_ptr<double> C) 
            : Component(std::move(time), std::move(voltage), std::move(current)), C(std::move(C)) {}
        ~Capacator() {}

        void setC(std::unique_ptr<double> C)
        {
            this->C = std::move(C);
        }

        double getC() {return *C;}

        double calculateCurrent(double dVdt)
        {
            return (*C) * dVdt;
        }

        void updateVoltage(double I, double deltaT)
        {
            if (voltage && C) {
                *voltage = *voltage + I * deltaT * (1 / *C);
            } else {
                std::cerr << "Error: Voltage or Capacitance is not initialized." << std::endl;
            }
        }
};

class Resister : public Component
{
    private:
    std::unique_ptr<double> r; //Resistance value
    public:
    Resister(std::unique_ptr<double> time, std::unique_ptr<double> voltage, std::unique_ptr<double> current, std::unique_ptr<double> r) 
    : Component(std::move(time), std::move(voltage), std::move(current)), r(std::move(r)) {}
    ~Resister() {}
    
    void setR(std::unique_ptr<double> r)
    {
        this->r = std::move(r);
    }
    
    double getR() {return *r;}

    void updateCurrent(double deltaT, double C)
        {
            if (current && r) {
                *current = *current - (*current / (*r * C)) * deltaT;
            } else {
                std::cerr << "Error: Current or Resistance is not initialized." << std::endl;
            }
        }
};

double calculateDerivative(double V1, double V2, double t1, double t2)
{
    return (V2 - V1) / (t2 - t1);
}

int main(int argc, char const *argv[])
{
    std::unique_ptr<double> time = std::make_unique<double>(0.0);
    std::unique_ptr<double> voltage = std::make_unique<double>(0.0); // Initial voltage is 0
    std::unique_ptr<double> current = std::make_unique<double>(1e-2); // Constant current supply
    std::unique_ptr<double> capacitance = std::make_unique<double>(100e-12); // Capacitance value
    std::unique_ptr<double> resistance = std::make_unique<double>(1e3); // Resistance value

    Capacator cap(std::move(time), std::move(voltage), std::move(current), std::move(capacitance));
    Resister res(std::make_unique<double>(0.0), std::make_unique<double>(0.0), std::make_unique<double>(1e-2), std::move(resistance));

    double deltaT = 1e-10; // Time step size
    double finalTime = 5e-6; // Final time
    int numTimeSteps = (finalTime / deltaT);

    for (int i = 0; i < numTimeSteps; ++i)
    {
        cap.updateVoltage(cap.getCurrent(), deltaT);
        res.updateCurrent(deltaT, cap.getC());
        if (i % 200 == 0)
        {
            std::cout << "Time: " << i * deltaT << " s | Voltage: " << cap.getVoltage() << " V | Current: " << res.getCurrent() << " A" << std::endl;
        }
    }

    return 0;
}