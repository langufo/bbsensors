#ifndef AM335XGPIO_H
#define AM335XGPIO_H

class AM335xGPIO {
public:
    AM335xGPIO(int controller, int line);
    ~AM335xGPIO();
    int read();
    void write(int bit);
private:
    const int GPIO[4] = {0x44e07000, 0x4804c000, 0x481ac000, 0x481ae000};
    const int GPIO_OE = 0x134;
    const int GPIO_DATAIN = 0x138;
    const int GPIO_CLEARDATAOUT = 0x190;
    const int GPIO_SETDATAOUT = 0x194;

    int line;

    int mem;
    unsigned char* reg;
};

#endif