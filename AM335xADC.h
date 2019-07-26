#ifndef AM335XADC_H
#define AM335XADC_H

#include <map>
#include <vector>

using std::map;
using std::vector;

class AM335xADC {
public:
    typedef unsigned char CHN;
    static const CHN CHN0 = 1;
    static const CHN CHN1 = 2;
    static const CHN CHN2 = 4;
    static const CHN CHN3 = 8;
    static const CHN CHN4 = 16;
    static const CHN CHN5 = 32;
    static const CHN CHN6 = 64;
    static const CHN CHN7 = 128;

    /**
     * Costruttore.
     * @param channels Maschera di bit che individua i canali da campionare. Si 
     *                 può costruire a partire dai membri statici CHN0, CHN1, 
     *                 CHN2... combinabili attraverso bitwise xor.
     * @param samples Numero di campionamenti da effettuare alla volta.
     */
    AM335xADC(CHN channels, unsigned int samples);
    ~AM335xADC();

    /**
     * Campiona.
     */
    void sample();

    /**
     * Restituisce un riferimento a un vector contenente i valori campionati 
     * per il canale indicato.
     * @param channel Canale di cui restituire i valori campionati.
     * @return Un vector contenente i valori campionati.
     */
    const std::vector<unsigned int>& getADCValues(unsigned int channel);

    /**
     * Restituisce la tensione media per il canale indicato.
     * @param channel Canale di cui restituire la tensione media.
     * @return La tensione media per il canale indicato.
     */
    double getAverage(unsigned int channel);

    /**
     * Restituisce l'errore standard della tensione media per il canale 
     * indicato.
     * @param channel Canale di cui restituire l'errore standard sulla tensione 
     *                media.
     * @return Errore standard sulla tensione media del canale indicato.
     */
    double getStandardError(unsigned int channel);

private:
    unsigned int samples;

    int fd;

    unsigned int nChannels;
    unsigned char* data;

    map<unsigned int, unsigned int> index;
    vector<vector<unsigned int>> values;
};

#endif