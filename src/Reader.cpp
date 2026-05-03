#include "Reader.h"

// Вспомогательные функции записи/чтения строк в бинарный файл 

static void writeStr(std::ofstream& out, const std::string& str) {
    size_t len = str.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(str.c_str(), len);
}

static std::string readStr(std::ifstream& in) {
    size_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string str(len, '\0');
    in.read(&str[0], len);
    return str;
}

//  ReaderNode 

ReaderNode::ReaderNode(int id, const std::string& name, const std::string& phone)
    : id(id), name(name), phone(phone), prev(nullptr), next(nullptr) {}

void ReaderNode::writeToBinary(std::ofstream& out) const {
    out.write(reinterpret_cast<const char*>(&id), sizeof(id));
    writeStr(out, name);
    writeStr(out, phone);
}

ReaderNode* ReaderNode::readFromBinary(std::ifstream& in) {
    int id;
    in.read(reinterpret_cast<char*>(&id), sizeof(id));
    if (!in.good()) return nullptr;

    std::string name = readStr(in);
    std::string phone = readStr(in);

    return new ReaderNode(id, name, phone);
}

//  IssueRecord 

IssueRecord::IssueRecord(int bookId, int readerId, const std::string& date)
    : bookId(bookId), readerId(readerId), issueDate(date),
      returnDate(""), prev(nullptr), next(nullptr) {}

void IssueRecord::writeToBinary(std::ofstream& out) const {
    out.write(reinterpret_cast<const char*>(&bookId), sizeof(bookId));
    out.write(reinterpret_cast<const char*>(&readerId), sizeof(readerId));
    writeStr(out, issueDate);
    writeStr(out, returnDate);
}

IssueRecord* IssueRecord::readFromBinary(std::ifstream& in) {
    int bId, rId;
    in.read(reinterpret_cast<char*>(&bId), sizeof(bId));
    if (!in.good()) return nullptr;
    in.read(reinterpret_cast<char*>(&rId), sizeof(rId));

    std::string issueDate = readStr(in);
    std::string returnDate = readStr(in);

    IssueRecord* rec = new IssueRecord(bId, rId, issueDate);
    rec->returnDate = returnDate;
    return rec;
}
