#ifndef MAXIM1WIRE_H
#define MAXIM1WIRE_H

#include <set>
#include <termios.h>

/**
 * Implementazione del master di un bus 1-Wire.
 */
class Maxim1WireBus {
public:
    /**
     * Costruttore. Richiede come argomento il percorso dell'interfaccia 
     * seriale nel file system.
     * @param ttyPath Percorso dell'interfaccia seriale.
     */
    Maxim1WireBus(const char* ttyPath);

    ~Maxim1WireBus();

    /**
     * Trasmette un impulso di reset sul bus e restituisce 1 se almeno un 
     * dispositivo ha segnalato la propria presenza, altrimenti 0.
     * @return 1 se almeno un dispositivo ha segnalato la propria presenza, 
     *         altrimenti 0.
     */
    int reset();

    /**
     * Legge un bit dal bus e lo restituisce.
     * @return Il bit letto dal bus.
     */
    int read();

    /**
     * Scrive bits bit della rappresentazione binaria di data sul bus, partendo 
     * dal meno significativo.
     * @param data Contiene i bit da inviare sul bus; il primo bit inviato è il 
     *             meno significativo.
     * @param bits Numero di bit di data da trasmettere sul bus; deve essere 
     *             almeno pari a 1 e al più 64.
     */
    void write(unsigned long long data, int bits = 1);

    /**
     * Scopre i codici delle ROM dei dispositivi collegati al bus e li 
     * restituisce all'interno di un set.
     * @return Un set contenente i codici delle ROM
     */
    std::set<unsigned long long> findDevices();
private:

    /**
     * File descriptor associato all'interfaccia seriale.
     */
    int ttyFile;

    /**
     * Struttura che contiene la configurazione dell'interfaccia seriale.
     */
    struct termios ttyConfig;
};

#endif
