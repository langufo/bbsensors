#ifndef HTM2500LF_H
#define HTM2500LF_H

class HTM2500LF {
public:

    /**
     * Costruttore.
     * @param tempDividerVoltage Tensione in volt applicata ai capi del 
     *                           partitore per la misura della temperatura.
     * @param tempNonProbedR Resistenza in serie all'NTC; espressa in ohm.
     * @param humNonprobedR Resistenza del partitore per RH collegata al 
     *                      sensore; espressa in ohm.
     * @param humProbedHumR Resistenza del partitore per RH collegata a terra; 
     *                      espressa in ohm.
     */
    HTM2500LF(double tempDividerVoltage, double nonprobedTempR, 
              double nonProbedHumR, double probedHumR);

    /**
     * Effettua una lettura della temperatura e ne restituisce il risultato in 
     * gradi celsius.
     * @param voltage Tensione in volt per l'NTC.
     * @return La temperatura attuale in gradi celsius.
     */
    double getTemp(double voltage);

    /**
     * Effettua una lettura della temperatura e ne restituisce il risultato in 
     * punti percentuali.
     * @param voltage Tensione in volt per il sensore di umidità.
     * @param temp Valore di temperatura in gradi celsius per cui correggere la 
     *             lettura di RH; se non è indicato non viene apportata alcuna 
     *             correzione per la temperatura.
     * @return L'umidità relativa attuale in punti percentuali.
     */
    double getHum(double voltage, double temp = 23);

private:

    /**
     * Tensione in volt applicata al partitore con l'NTC.
     */
    double tempDividerVoltage;

    /**
     * Resistenza in ohm in serie all'NTC.
     */
    double tempNonprobedR;

    /**
     * Resistenza in ohm del partitore per RH collegata al sensore.
     */
    double humNonprobedR;

    /**
     * Resistenza in ohm del partitore per collegata a terra.
     */
    double humProbedR;
};

#endif
