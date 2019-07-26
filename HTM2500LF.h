#ifndef HTM2500LF_H
#define HTM2500LF_H

class HTM2500LF {
public:

    /**
     * Costruttore.
     * @param nonprobedTempR Resistenza in serie all'NTC; espressa in ohm.
     * @param nonprobedHumR Resistenza del partitore per RH collegata al 
     *                      sensore; espressa in ohm.
     * @param probedHumR Resistenza del partitore per RH collegata a terra; 
     *                   espressa in ohm.
     */
    HTM2500LF(double nonprobedTempR, double nonProbedHumR, double probedHumR);

    /**
     * Effettua una lettura della temperatura e ne restituisce il risultato in 
     * gradi celsius.
	 * @param adcValue La lettura dell'ADC per l'NTC.
     * @return La temperatura attuale in gradi celsius.
     */
    double getTemp(double adcValue);

    /**
     * Effettua una lettura della temperatura e ne restituisce il risultato in 
     * punti percentuali.
     * @param adcValue La lettura dell'ADC per il sensore di umidità.
     * @param temp Valore di temperatura in gradi celsius per cui correggere la 
	 *             lettura di RH; se non è indicato non viene apportata alcuna 
	 *             correzione per la temperatura.
     * @return L'umidità relativa attuale in gradi celsius.
     */
    double getHum(double adcValue, double temp = 23);

private:

    /**
     * La resistenza in serie all'NTC; espressa in ohm.
     */
    double nonprobedTempR;

    /**
     * La resistenza del partitore per RH collegata al sensore; espressa in ohm.
     */
    double nonprobedHumR;

    /**
     * La resistenza del partitore per collegata a terra; espressa in kiloohm.
     */
    double probedHumR;
};

#endif
