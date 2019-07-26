#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

#include "AM335xGPIO.h"

using std::invalid_argument;
using std::runtime_error;

/**
 * Costruttore dell'interfaccia a un pin GPIO.
 * @param controller Numero di controller GPIO; segue "GPIO" nel nome del pin.
 * @param line Linea GPIO del controller; segue "_" nel nome del pin.
 */
AM335xGPIO::AM335xGPIO(int controller, int line) {
    if (controller < 0 || controller > 3) {
        throw new invalid_argument("invalid controller number");
    }
    if (line < 0 || line > 32) {
        throw new invalid_argument("invalid line number");
    }

    AM335xGPIO::line = line;

    mem = open("/dev/mem", O_RDWR);
    if (mem < 0) {
        throw new runtime_error("couldn't access memory");
    }
    reg = (unsigned char*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem, GPIO[controller]);
}

/**
 * Distruttore dell'interfaccia a un pin GPIO.
 */
AM335xGPIO::~AM335xGPIO() {
    munmap(reg, 4096);
    close(mem);
}

/**
 * Legge il livello del pin GPIO.
 * @returns 0 se l'ingresso è basso, 1 se l'ingresso è alto.
 */
int AM335xGPIO::read() {
    *((unsigned int*)(reg+GPIO_OE)) |= 1<<line;
    return (*((unsigned int*)(reg+GPIO_DATAIN)) & 1<<line) != 0;
}

/**
 * Imposta il livello in uscita dal pin GPIO.
 * @param bit Il livello da impostare; deve essere 0 o 1.
 */
void AM335xGPIO::write(int bit) {
    if (bit == 0) {
        *((unsigned int*)(reg+GPIO_CLEARDATAOUT)) |= 1<<line;
    } else if (bit == 1) {
        *((unsigned int*)(reg+GPIO_SETDATAOUT)) |= 1<<line;
    } else {
        return;
    }

    *((unsigned int*)(reg+GPIO_OE)) &= 0xFFFFFFFF - 1<<line;
}