#ifndef DS18B20_H
#define DS18B20_H

#include "Maxim1WireBus.h"

class DS18B20 {
public:
    /**
     * Costruttore.
     * @param deviceID Codice seriale del dispositivo.
     * @param bus Bus al quale il sensore è collegato.
     */
    DS18B20(unsigned long long deviceID, Maxim1WireBus& bus);

    /**
     * Legge la temperatura convertita.
     * @return La temperatura convertita in gradi celsius.
     */
    double getMeasuredTemp();

    /**
     * Richiede a tutti i dispositivi collegati al bus fornito di effettuare la 
     * conversione della temperatura.
     * @param bus Il bus sul quale effettuare la richiesta.
     */
    static void convertT(Maxim1WireBus& bus);
private:
    unsigned long long deviceID;
    Maxim1WireBus& bus;
};

#endif