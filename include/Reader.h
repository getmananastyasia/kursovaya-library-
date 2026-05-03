#ifndef READER_H
#define READER_H

#include <string>
#include <fstream>

// Узел двусвязного списка читателей
struct ReaderNode {
    int         id;
    std::string name;
    std::string phone;

    ReaderNode* prev;
    ReaderNode* next;

    ReaderNode(int id, const std::string& name, const std::string& phone);

    void writeToBinary(std::ofstream& out) const;
    static ReaderNode* readFromBinary(std::ifstream& in);
};

// Запись о выдаче книги — связывает книгу и читателя
struct IssueRecord {
    int bookId;
    int readerId;
    std::string issueDate;   // дата выдачи (формат ДД.ММ.ГГГГ)
    std::string returnDate;  // дата возврата ("" если не возвращена)

    IssueRecord* prev;
    IssueRecord* next;

    IssueRecord(int bookId, int readerId, const std::string& date);

    void writeToBinary(std::ofstream& out) const;
    static IssueRecord* readFromBinary(std::ifstream& in);
};

#endif // READER_H
