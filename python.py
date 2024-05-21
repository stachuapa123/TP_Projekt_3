from audioop import *

from build.Debug.cmake_example import *
#kopiuj wklej      ahem_x.wav
while 1:
    
    c = int(input("Podaj funkcje: 1 - wizualizacja sygnalu, 2 - DFT, 3 - IDFT, 4 - usuwanie czestotliwosci, 5 - wybrane wykresy 6 - koniec "))
    
    if c<5:
        s = (input("Podaj plik:  "))
        if otwarcie(s) == 0:
            print("Nie ma takiego pliku")
            continue
    if c == 1:
       print(sygnal_wav(s))
    elif c == 2:
        print(dft1(s))
    elif c == 3:
        print(idft1(s))
    elif c == 4:
        q = float(input("Podaj jakie czestotliwosci chcesz usunac: "))
        print(usuwanie(s,q))
    elif c==5:
        f = int(input("Podaj czestotliwosc: "  ))
        a = float(input("Podaj Amplitude: "))
        print(sygnaly(f,a))
    elif c == 6:
        break
#w konsoli wpisac kolejno: cd build, cmake .., cd .., cmake --build build
