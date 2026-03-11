#include <iostream>

class Generator {
    private:
        int Aktualny;
    public:
        Generator(int i = 10) : Aktualny(i) {}
        int operator()() {
            return Aktualny += 10;
        }
};

int main() {
    Generator g(10);
    do {
        std::cout <<'.';
    } while (g() < 101);
}