#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <complex>
#include <cmath>
#include <matplot/matplot.h>
#include <sndfile.h>
#include <algorithm>
#include <string>

namespace py = pybind11;
namespace m = matplot;

const double PI = 3.14159;

void mplot(std::vector<double> p1, std::vector<double> p2, std::string s1, std::string s2, std::string s3)
{
    auto o = m::figure(true);
    o->size(1200, 800);
    m::plot(p1, p2);
    m::title(s1);
    m::xlabel(s2);
    m::ylabel(s3);
    m::show();
}

std::vector<std::complex<double>> dft(const std::vector<double>& O) {
    int N = O.size();
    std::vector<std::complex<double>> D(N);

    for (int k = 0; k < N; k++) {
        std::complex<double> suma(0, 0);
        for (int n = 0; n < N; n++) {
            double zesp = 2 * PI * n * k / N;
            suma += O[n] * std::complex<double>(cos(zesp), -sin(zesp));
        }
        D[k] = suma;
    }

    return D;
}

std::vector<std::complex<double>> idft(const std::vector<std::complex<double>>& D, int g) {
    int N = D.size();
    std::vector<std::complex<double>> O(N);

    for (int n = 0; n < N; n++) {
        std::complex<double> suma(0, 0);
        for (int k = g; k < N; k++) {
            double zesp = 2 * PI * n * k / N;
            suma += D[k] * std::complex<double>(cos(zesp), sin(zesp));
        }
        O[n] = suma / static_cast<double>(N);
    }

    return O;
}

int otwarcie(const std::string& plik) {
    SF_INFO info;
    SNDFILE* plik_wav = sf_open(plik.c_str(), SFM_READ, &info);
    if (plik_wav == NULL)  return 0;
    else
    {

        sf_close(plik_wav);
        return 1;

    }


}


std::pair<std::vector<double>, int> wczytaj_wav(const std::string& wavplik ) {
    SF_INFO info;
    SNDFILE* plik = sf_open(wavplik.c_str(), SFM_READ, &info);
    std::vector<double> wartosci(info.frames * info.channels);
    sf_read_double(plik, wartosci.data(), info.frames * info.channels);
    sf_close(plik);

    return { wartosci, info.samplerate };
}

void sygnaly(int fr, double a) {
    auto o = m::figure(true);
    o->size(1200, 600);

    std::vector<double> x = m::linspace(0, 2 * PI, 1000);
    std::vector<double> y;

    // Sinus

    m::subplot(2, 2, 1);
    y = m::transform(x, [fr,a](double t) { return a*sin(t * fr); });
    m::plot(x, y);
    m::title("Wykres Sinus");

    // Cosinus
    m::subplot(2, 2, 2);
    y = m::transform(x, [fr, a](double t) { return a*cos(t * fr); });
    m::plot(x, y);
    m::title("Wykres Cosinus");

    // Piła
    m::subplot(2, 2, 3);
    y = m::transform(x, [fr, a](double t) { return a*2.0 * (t * fr / (2 * PI) - floor(0.5 + t * fr / (2 * PI))); });
    m::plot(x, y);
    m::title("Wykres Piłowy");

    // Prostokąt
    m::subplot(2, 2, 4);
    y = m::transform(x, [fr, a](double t) { return std::copysign(a, sin(t * fr)); });
    m::plot(x, y);
    m::title("Wykres Prostokątny");

    m::show();
}

void sygnal_wav(const std::string& wavplik) {
    auto [wartosci, probki] = wczytaj_wav(wavplik);

    std::vector<double> czas(wartosci.size());
    for (size_t i = 0; i < wartosci.size(); ++i) {
        czas[i] = static_cast<double>(i) / probki;
    }

    mplot(czas, wartosci, "Sygnał WAV", "Czas [s]", "Amplituda");
}

void dft1(const std::string& wavplik) {
    auto [wartosci, probki] = wczytaj_wav(wavplik);

    auto D = dft(wartosci);
    int N = D.size();
    std::vector<double> fr(N);
    for (int i = 0; i < N; i++) {
        fr[i] = i * probki / N;
    }

    std::vector<double> am(N);
    for (int i = 0; i < N; i++) {
        am[i] = std::abs(D[i]);
    }

    mplot(fr, am, "DFT sygnału WAV", "Częstotliwość [Hz]", "Amplituda");
}

void idft1(const std::string& wavplik) {
    auto [wartosci, probki] = wczytaj_wav(wavplik);

    auto D = dft(wartosci);
    auto Oi = idft(D,0);

    std::vector<double> czas(Oi.size());
    for (int i = 0; i < Oi.size(); i++) {
        czas[i] = static_cast<double>(i) / probki;
    }
   
    std::vector<double> am_idft(Oi.size());
    for (int i = 0; i < Oi.size(); i++) {
        am_idft[i] = Oi[i].real();
    }

    mplot(czas, am_idft, "Sygnał po IDFT", "Czas [s]", "Amplituda");
}

void usuwanie(const std::string& wavplik, float q) {
    auto [wartosci, probki] = wczytaj_wav(wavplik);

    auto D = dft(wartosci);
    int N = D.size();
    double qn = q * N;
    int qn2 = floor(qn);
    for (int i = 0; i < qn; i++) {
        D[i] = 0;
    }

    auto Oi = idft(D,qn2);
    int on = Oi.size();
    std::vector<double> czas(wartosci.size());
    for (int i = 0; i < wartosci.size(); i++) {
        czas[i] = static_cast<double>(i) / probki;
    }

    std::vector<double> am_idft(on);
    for (int i = 0; i < on; ++i) {
        am_idft[i] = Oi[i].real();
    }

    mplot(czas, am_idft, "Sygnał po usunięciu niskich częstotliwości", "Czas [s]", "Amplituda");
}

PYBIND11_MODULE(cmake_example, m) {
    m.def("sygnaly", &sygnaly);
    m.def("sygnal_wav", &sygnal_wav);
    m.def("dft1", &dft1);
    m.def("idft1", &idft1);
    m.def("usuwanie", &usuwanie);
    m.def("otwarcie", &otwarcie);
}
