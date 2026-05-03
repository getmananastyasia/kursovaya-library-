#include "Library.h"
#include <iostream>

int main() {
    setlocale(LC_ALL, "");

    Library library;
    library.loadFromFile("library.dat");

    std::cout << "Библиотека загружена." << std::endl;
    std::cout << "Книг: " << library.getBookCount() << std::endl;
    std::cout << "Читателей: " << library.getReaderCount() << std::endl;
    std::cout << "Используйте веб-интерфейс: web/index.html" << std::endl;

    return 0;
}
