#include <iostream>
#include <map>

int main() {
    std::map<std::string, int> dane;
    // przykładowe dane
    dane = {{"1-jeden", 1}, {"2-dwa", 2}, {"3-trzy", 3}};
    // C++14, używam iteracji z auto i korzystam z std::pair
    for (const auto& para : dane) {
        std::cout << para.first << ": " << para.second << std::endl;
    }
    return 0;
}

/* Komentarz z wywołania
pw_advanced_c++/playground on  main [$?]
❯ clang++ -o main_loop main_loop.cpp

pw_advanced_c++/playground on  main [$?]
❯ ./main_loop
1-jeden: 1
2-dwa: 2
3-trzy: 3
*/