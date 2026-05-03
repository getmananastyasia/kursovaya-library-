#include "Book.h"
#include <cstring>

//  Преобразование Genre <-> string 
std::string genreToString(Genre genre) {
    switch (genre) {
        case Genre::Fiction:     return "Художественная";
        case Genre::Science:     return "Наука";
        case Genre::History:     return "История";
        case Genre::Philosophy:  return "Философия";
        case Genre::Programming: return "Программирование";
        case Genre::Poetry:      return "Поэзия";
        case Genre::Other:       return "Другое";
    }
    return "Другое";
}

Genre stringToGenre(const std::string& str) {
    if (str == "Художественная") return Genre::Fiction;
    if (str == "Наука")          return Genre::Science;
    if (str == "История")        return Genre::History;
    if (str == "Философия")      return Genre::Philosophy;
    if (str == "Программирование") return Genre::Programming;
    if (str == "Поэзия")         return Genre::Poetry;
    return Genre::Other;
}

Genre indexToGenre(int index) {
    switch (index) {
        case 1: return Genre::Fiction;
        case 2: return Genre::Science;
        case 3: return Genre::History;
        case 4: return Genre::Philosophy;
        case 5: return Genre::Programming;
        case 6: return Genre::Poetry;
        default: return Genre::Other;
    }
}

// BookNode 

BookNode::BookNode(int id, const std::string& title, const std::string& author,
                   int year, Genre genre)
    : id(id), title(title), author(author), year(year),
      genre(genre), isAvailable(true), prev(nullptr), next(nullptr) {}

// Запись строки в бинарный файл: сначала длина (size_t), затем символы
static void writeString(std::ofstream& out, const std::string& str) {
    size_t len = str.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(str.c_str(), len);
}

// Чтение строки из бинарного файла
static std::string readString(std::ifstream& in) {
    size_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string str(len, '\0');
    in.read(&str[0], len);
    return str;
}

void BookNode::writeToBinary(std::ofstream& out) const {
    out.write(reinterpret_cast<const char*>(&id), sizeof(id));
    writeString(out, title);
    writeString(out, author);
    out.write(reinterpret_cast<const char*>(&year), sizeof(year));
    int g = static_cast<int>(genre);
    out.write(reinterpret_cast<const char*>(&g), sizeof(g));
    out.write(reinterpret_cast<const char*>(&isAvailable), sizeof(isAvailable));
}

BookNode* BookNode::readFromBinary(std::ifstream& in) {
    int id;
    in.read(reinterpret_cast<char*>(&id), sizeof(id));
    if (!in.good()) return nullptr;

    std::string title = readString(in);
    std::string author = readString(in);

    int year;
    in.read(reinterpret_cast<char*>(&year), sizeof(year));

    int g;
    in.read(reinterpret_cast<char*>(&g), sizeof(g));

    bool avail;
    in.read(reinterpret_cast<char*>(&avail), sizeof(avail));

    BookNode* node = new BookNode(id, title, author, year, static_cast<Genre>(g));
    node->isAvailable = avail;
    return node;
}
