#ifndef AM335XADC_H
#define AM335XADC_H

#include <map>
#include <vector>

using std::map;
using std::vector;

class AM335xADC {
public:
    static const unsigned char CHN0 = 1;
    static const unsigned char CHN1 = 2;
    static const unsigned char CHN2 = 4;
    static const unsigned char CHN3 = 8;
    static const unsigned char CHN4 = 16;
    static const unsigned char CHN5 = 32;
    static const unsigned char CHN6 = 64;
    static const unsigned char CHN7 = 128;

    /**
     * Costruttore. La curva di calibrazione predefinita per tutti i canali è 
     * la retta per (ADC = 0, V = 0) e (ADC = 4095, V = 1.8).
     * @param channels Maschera di bit che individua i canali da campionare. Si 
     *                 può costruire a partire dai membri statici CHN0, CHN1, 
     *                 CHN2... combinabili attraverso bitwise or.
     * @param samples Numero di campionamenti da effettuare alla volta.
     */
    AM335xADC(unsigned char channels, int samples);
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
    const vector<unsigned int>& getADCValues(int channel);

    /**
     * Restituisce la tensione media per il canale indicato.
     * @param channel Canale di cui restituire la tensione media.
     * @return La tensione media per il canale indicato.
     */
    double getAverage(int channel);

    /**
     * Restituisce l'errore standard della tensione media per il canale 
     * indicato.
     * @param channel Canale di cui restituire l'errore standard sulla tensione 
     *                media.
     * @return L'errore standard sulla tensione media del canale indicato.
     */
    double getStandardError(int channel);

    /**
     * Carica il punto indicato alla curva di calibrazione per il canale 
     * selezionato. Un punto precedentemente inserito che abbia un valore 
     * dell'ADC pari a quello fornito a questo metodo viene eliminato.
     * @param channel Canale la cui curva di calibrazione viene modificata.
     * @param voltage Tensione in volt in ingresso al canale selezionato.
     * @param adcValue Valore campionato dall'ADC.
     */
    void addCalibPoint(int channel, double voltage, double adcValue);

    /**
     * Restituisce la tensione associata al valore dell'ADC fornito attraverso 
     * la curva di calibrazione per il canale indicato.
     * @param channel Canale dell'ADC della misura.
     * @param adcValue Valore dell'ADC da convertire.
     * @return Il valore in tensione convertito.
     */
    double convertADCValue(int channel, double adcValue);

private:
    map<double, double> calibration[8];

    int samples;

    int fd;

    unsigned char* buffer;
    unsigned int bufferSize;

    map<int, vector<unsigned int>> values;
    map<int, double> averages;
    map<int, double> standardErrors;
};

#endif
