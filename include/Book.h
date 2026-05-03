#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <fstream>

// Перечисление жанров книг
enum class Genre {
    Fiction,       // Художественная литература
    Science,       // Наука
    History,       // История
    Philosophy,    // Философия
    Programming,   // Программирование
    Poetry,        // Поэзия
    Other          // Другое
};

// Преобразование жанра в строку и обратно
std::string genreToString(Genre genre);
Genre stringToGenre(const std::string& str);
Genre indexToGenre(int index);

// Узел двусвязного списка книг
struct BookNode {
    // Данные книги
    int         id;
    std::string title;
    std::string author;
    int         year;
    Genre       genre;
    bool        isAvailable;  // true = в библиотеке, false = выдана

    // Указатели двусвязного списка
    BookNode* prev;
    BookNode* next;

    BookNode(int id, const std::string& title, const std::string& author,
             int year, Genre genre);

    // Сериализация в бинарный файл
    void writeToBinary(std::ofstream& out) const;
    static BookNode* readFromBinary(std::ifstream& in);
};

#endif // BOOK_H
