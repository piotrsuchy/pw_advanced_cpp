#include <iostream>
#include <string>
// importy do użytego string i cout w main
// skoro jest użyte string, a nie std::string:
using namespace std;

class COczko {
public:
  COczko() { kolor = "szary"; }
  string dajKolor() { return kolor; }
private:
  string kolor;
};
class CMisio {
public:
  COczko lewe;
  COczko prawe;
};

int main() {
  CMisio misio;
  cout << "Lewe oczko misia ma kolor: " << misio.lewe.dajKolor() << endl;
  cout << "Prawe oczko misia ma kolor: " << misio.prawe.dajKolor() << endl;
};

/* Komentarz z wywołania
pw_advanced_c++/playground on  main [$?]
❯ clang++ -o main_misio misia_oczko.cpp

pw_advanced_c++/playground on  main [$?]
❯ ./main_misio
Lewe oczko misia ma kolor: szary
Prawe oczko misia ma kolor: szary
*/