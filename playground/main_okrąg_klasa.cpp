#include <iostream>
#include <stdexcept>
#include <cmath>

class COkrąg {
private:
    double x;
    double y;

    // prywatna metoda pomocnicza do walidacji
    void validate(double newX, double newY) const {
        if ((newX * newX) + (newY * newY) > 1.0) {
            throw std::invalid_argument("Punkt wykracza poza obręb okręgu jednostkowego.");
        }
    }

public:
    // konstruktor inicjalizujący
    COkrąg(double x = 0.0, double y = 0.0) {
        validate(x, y);
        this->x = x;
        this->y = y;
    }

    // settery z walidacją
    void setX(double newX) {
        validate(newX, this->y);
        x = newX;
    }

    void setY(double newY) {
        validate(this->x, newY);
        y = newY;
    }

    // gettery
    double getX() const { return x; }
    double getY() const { return y; }
};

int main() {
    COkrąg p(0.5, 0.5);
    std::cout << "Punkt: (" << p.getX() << ", " << p.getY() << ")" << std::endl;
    p.setX(0.1);
    p.setY(0.2);
    std::cout << "Punkt: (" << p.getX() << ", " << p.getY() << ")" << std::endl;
    p.setX(0.3);
    p.setY(0.4);
    std::cout << "Punkt: (" << p.getX() << ", " << p.getY() << ")" << std::endl;
    try {
        p.setX(1.1);
    } catch (const std::invalid_argument &e) {
        std::cerr << "Blad: " << e.what() << std::endl;
    }
    try {
        p.setY(1.1);
    } catch (const std::invalid_argument &e) {
        std::cerr << "Blad: " << e.what() << std::endl;
    }
    return 0;
}