#ifndef RTD_H
#define RTD_H

class RTD {
public:
    /**
     * Costruttore.
     * @param alpha Parametro di dipendenza dalla temperatura dell'RTD.
     * @param rtd0R Resistenza dell'RTD a 0°C in ohm.
     * @param currentR Resistenza tra terminale invertente dell'OPA227 e terra, 
     *                 per fissare la corrente che scorre nell'RTD, espressa in 
     *                 ohm.
     * @param currentV Tensione in volt applicata al terminale non invertente 
     *                 dell'OPA227.
     * @param ampGainR Resistenza per la selezione del guadagno dell'INA122 
     *                 espressa in ohm.
     * @param ampRefV Tensione cui l'output dell'INA122 è riferito; espressa in 
     *                volt.
     * @param inverting True se l'INA122 è montato come invertente, altrimenti  
     *                  false.
     */
    RTD(double alpha, double rtd0R, 
        double currentR, double currentV, 
        double ampGainR, double ampRefV, bool inverting);

    /**
     * Restituisce la temperatura in gradi celsius.
     * @param adcInV La tensione in uscita dall'INA122 letta dall'ADC.
     * @return La temperatura in gradi celsius.
     */
    double getTemp(double adcInV);
private:
    double alpha;
    double ampRefV;
    double k;
};

#endif