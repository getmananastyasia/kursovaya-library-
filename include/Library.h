#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "Reader.h"
#include <string>
#include <vector>

// Facade-класс: инкапсулирует всю бизнес-логику библиотеки
// Управляет тремя двусвязными списками: книги, читатели, записи выдачи
class Library {
private:
    BookNode*    booksHead;
    BookNode*    booksTail;
    ReaderNode*  readersHead;
    ReaderNode*  readersTail;
    IssueRecord* issuesHead;
    IssueRecord* issuesTail;

    int nextBookId;
    int nextReaderId;

    // Вспомогательные методы для merge sort на двусвязном списке
    // Сортировка разделяет список пополам, рекурсивно сортирует и сливает
    BookNode* mergeSortByAuthor(BookNode* head);
    BookNode* mergeSortByYear(BookNode* head);
    BookNode* splitList(BookNode* head);
    BookNode* mergeByAuthor(BookNode* a, BookNode* b);
    BookNode* mergeByYear(BookNode* a, BookNode* b);
    void updateTail();

public:
    Library();
    ~Library();

    // Книги (CRUD)
    void addBook(const std::string& title, const std::string& author,
                 int year, Genre genre);
    bool removeBook(int id);
    BookNode* findBookById(int id) const;
    std::vector<BookNode*> searchByTitle(const std::string& query) const;
    std::vector<BookNode*> searchByAuthor(const std::string& query) const;
    BookNode* getBooksHead() const;
    int getBookCount() const;

    // Читатели (CRUD) 
    void addReader(const std::string& name, const std::string& phone);
    bool removeReader(int id);
    ReaderNode* findReaderById(int id) const;
    ReaderNode* getReadersHead() const;
    int getReaderCount() const;

    //  Выдача / Возврат
    bool issueBook(int bookId, int readerId, const std::string& date);
    bool returnBook(int bookId, const std::string& date);
    std::vector<IssueRecord*> getActiveIssuesForReader(int readerId) const;
    IssueRecord* getIssuesHead() const;

    //  Сортировка 
    void sortByAuthor();
    void sortByYear();

    // Статистика 
    int countByGenre(Genre genre) const;
    int getAvailableCount() const;
    int getIssuedCount() const;

    // Файловый I/O (бинарный формат .dat) 
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif // LIBRARY_H
