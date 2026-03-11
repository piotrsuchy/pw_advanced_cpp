// dodatkowe include + using, aby użyty cout działał
#include <iostream>
#include <string>

using std::cout;

class CTowar {
private:
  int ilosc;
  std::string nazwa;
public:
  CTowar() { nazwa = ""; ilosc = 0; }
  void setNazwa(std::string _n) { nazwa = _n; }
  void setIlosc(int _i) { ilosc = _i; }
  void pisz() { cout << nazwa << ", ilosc: " << ilosc; }
  CTowar operator+(int dodatek) {
    CTowar wynik;
    wynik.nazwa = this->nazwa;
    wynik.ilosc = this->ilosc + dodatek;
    return wynik;
  }
};

int main() {
  CTowar a;
  a.setNazwa("jaja");
  a.setIlosc(5);
  a = a+5;
  a.pisz();
};