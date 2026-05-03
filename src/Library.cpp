#include "Library.h"
#include <algorithm>
#include <cctype>

Library::Library()
    : booksHead(nullptr), booksTail(nullptr),
      readersHead(nullptr), readersTail(nullptr),
      issuesHead(nullptr), issuesTail(nullptr),
      nextBookId(1), nextReaderId(1) {}

Library::~Library() {
    // Освобождаем память всех трёх списков
    while (booksHead) {
        BookNode* tmp = booksHead;
        booksHead = booksHead->next;
        delete tmp;
    }
    while (readersHead) {
        ReaderNode* tmp = readersHead;
        readersHead = readersHead->next;
        delete tmp;
    }
    while (issuesHead) {
        IssueRecord* tmp = issuesHead;
        issuesHead = issuesHead->next;
        delete tmp;
    }
}

//  КНИГИ — CRUD

void Library::addBook(const std::string& title, const std::string& author,
                      int year, Genre genre) {
    BookNode* node = new BookNode(nextBookId++, title, author, year, genre);
    // Вставка в конец двусвязного списка за O(1)
    if (!booksHead) {
        booksHead = booksTail = node;
    } else {
        booksTail->next = node;
        node->prev = booksTail;
        booksTail = node;
    }
}

bool Library::removeBook(int id) {
    BookNode* cur = booksHead;
    while (cur) {
        if (cur->id == id) {
            if (!cur->isAvailable) return false; // нельзя удалить выданную книгу

            // Перешивка указателей двусвязного списка
            if (cur->prev) cur->prev->next = cur->next;
            else booksHead = cur->next;

            if (cur->next) cur->next->prev = cur->prev;
            else booksTail = cur->prev;

            delete cur;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

BookNode* Library::findBookById(int id) const {
    BookNode* cur = booksHead;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}

// Поиск подстроки без учёта регистра (для латиницы)
static bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    std::string h = haystack, n = needle;
    // Приводим к нижнему регистру только ASCII-символы
    // Для кириллицы используем точное вхождение подстроки
    if (h.find(n) != std::string::npos) return true;
    std::transform(h.begin(), h.end(), h.begin(), ::tolower);
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);
    return h.find(n) != std::string::npos;
}

std::vector<BookNode*> Library::searchByTitle(const std::string& query) const {
    std::vector<BookNode*> results;
    BookNode* cur = booksHead;
    while (cur) {
        if (containsIgnoreCase(cur->title, query))
            results.push_back(cur);
        cur = cur->next;
    }
    return results;
}

std::vector<BookNode*> Library::searchByAuthor(const std::string& query) const {
    std::vector<BookNode*> results;
    BookNode* cur = booksHead;
    while (cur) {
        if (containsIgnoreCase(cur->author, query))
            results.push_back(cur);
        cur = cur->next;
    }
    return results;
}

BookNode* Library::getBooksHead() const { return booksHead; }

int Library::getBookCount() const {
    int count = 0;
    BookNode* cur = booksHead;
    while (cur) { ++count; cur = cur->next; }
    return count;
}

//  ЧИТАТЕЛИ — CRUD

void Library::addReader(const std::string& name, const std::string& phone) {
    ReaderNode* node = new ReaderNode(nextReaderId++, name, phone);
    if (!readersHead) {
        readersHead = readersTail = node;
    } else {
        readersTail->next = node;
        node->prev = readersTail;
        readersTail = node;
    }
}

bool Library::removeReader(int id) {
    // Нельзя удалить читателя с активными выдачами
    if (!getActiveIssuesForReader(id).empty()) return false;

    ReaderNode* cur = readersHead;
    while (cur) {
        if (cur->id == id) {
            if (cur->prev) cur->prev->next = cur->next;
            else readersHead = cur->next;

            if (cur->next) cur->next->prev = cur->prev;
            else readersTail = cur->prev;

            delete cur;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

ReaderNode* Library::findReaderById(int id) const {
    ReaderNode* cur = readersHead;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}

ReaderNode* Library::getReadersHead() const { return readersHead; }

int Library::getReaderCount() const {
    int count = 0;
    ReaderNode* cur = readersHead;
    while (cur) { ++count; cur = cur->next; }
    return count;
}

//  ВЫДАЧА / ВОЗВРАТ

bool Library::issueBook(int bookId, int readerId, const std::string& date) {
    BookNode* book = findBookById(bookId);
    ReaderNode* reader = findReaderById(readerId);

    if (!book || !reader || !book->isAvailable) return false;

    book->isAvailable = false;

    IssueRecord* rec = new IssueRecord(bookId, readerId, date);
    if (!issuesHead) {
        issuesHead = issuesTail = rec;
    } else {
        issuesTail->next = rec;
        rec->prev = issuesTail;
        issuesTail = rec;
    }
    return true;
}

bool Library::returnBook(int bookId, const std::string& date) {
    // Ищем активную запись выдачи (без даты возврата)
    IssueRecord* cur = issuesHead;
    while (cur) {
        if (cur->bookId == bookId && cur->returnDate.empty()) {
            cur->returnDate = date;
            BookNode* book = findBookById(bookId);
            if (book) book->isAvailable = true;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

std::vector<IssueRecord*> Library::getActiveIssuesForReader(int readerId) const {
    std::vector<IssueRecord*> results;
    IssueRecord* cur = issuesHead;
    while (cur) {
        if (cur->readerId == readerId && cur->returnDate.empty())
            results.push_back(cur);
        cur = cur->next;
    }
    return results;
}

IssueRecord* Library::getIssuesHead() const { return issuesHead; }
//  СОРТИРОВКА — Merge Sort на двусвязном списке
//  Merge Sort идеален для связных списков:
//  - Не требует произвольного доступа (в отличие от Quick Sort)
//  - O(n log n) гарантированно
//  - O(1) дополнительной памяти (только переброска указателей)

// Разделение списка на две половины (метод "быстрого и медленного указателя")
BookNode* Library::splitList(BookNode* head) {
    BookNode* slow = head;
    BookNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    BookNode* secondHalf = slow->next;
    slow->next = nullptr;
    if (secondHalf) secondHalf->prev = nullptr;
    return secondHalf;
}

BookNode* Library::mergeByAuthor(BookNode* a, BookNode* b) {
    if (!a) return b;
    if (!b) return a;

    if (a->author <= b->author) {
        a->next = mergeByAuthor(a->next, b);
        if (a->next) a->next->prev = a;
        a->prev = nullptr;
        return a;
    } else {
        b->next = mergeByAuthor(a, b->next);
        if (b->next) b->next->prev = b;
        b->prev = nullptr;
        return b;
    }
}

BookNode* Library::mergeByYear(BookNode* a, BookNode* b) {
    if (!a) return b;
    if (!b) return a;

    if (a->year <= b->year) {
        a->next = mergeByYear(a->next, b);
        if (a->next) a->next->prev = a;
        a->prev = nullptr;
        return a;
    } else {
        b->next = mergeByYear(a, b->next);
        if (b->next) b->next->prev = b;
        b->prev = nullptr;
        return b;
    }
}

BookNode* Library::mergeSortByAuthor(BookNode* head) {
    if (!head || !head->next) return head;
    BookNode* second = splitList(head);
    head = mergeSortByAuthor(head);
    second = mergeSortByAuthor(second);
    return mergeByAuthor(head, second);
}

BookNode* Library::mergeSortByYear(BookNode* head) {
    if (!head || !head->next) return head;
    BookNode* second = splitList(head);
    head = mergeSortByYear(head);
    second = mergeSortByYear(second);
    return mergeByYear(head, second);
}

// После сортировки обновляем указатель на хвост
void Library::updateTail() {
    if (!booksHead) { booksTail = nullptr; return; }
    BookNode* cur = booksHead;
    while (cur->next) cur = cur->next;
    booksTail = cur;
}

void Library::sortByAuthor() {
    booksHead = mergeSortByAuthor(booksHead);
    updateTail();
}

void Library::sortByYear() {
    booksHead = mergeSortByYear(booksHead);
    updateTail();
}

//  СТАТИСТИКА

int Library::countByGenre(Genre genre) const {
    int count = 0;
    BookNode* cur = booksHead;
    while (cur) {
        if (cur->genre == genre) ++count;
        cur = cur->next;
    }
    return count;
}

int Library::getAvailableCount() const {
    int count = 0;
    BookNode* cur = booksHead;
    while (cur) {
        if (cur->isAvailable) ++count;
        cur = cur->next;
    }
    return count;
}

int Library::getIssuedCount() const {
    int count = 0;
    BookNode* cur = booksHead;
    while (cur) {
        if (!cur->isAvailable) ++count;
        cur = cur->next;
    }
    return count;
}
//  ФАЙЛОВЫЙ I/O — Бинарный формат
//
//  Формат файла library.dat:
//  [bookCount][book1][book2]...[readerCount][reader1]...[issueCount][issue1]...
//  Строки хранятся как: [длина (size_t)][символы]

bool Library::saveToFile(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    // Сохраняем счётчики ID для корректного продолжения нумерации
    out.write(reinterpret_cast<const char*>(&nextBookId), sizeof(nextBookId));
    out.write(reinterpret_cast<const char*>(&nextReaderId), sizeof(nextReaderId));

    // Книги
    int bookCount = getBookCount();
    out.write(reinterpret_cast<const char*>(&bookCount), sizeof(bookCount));
    BookNode* bCur = booksHead;
    while (bCur) {
        bCur->writeToBinary(out);
        bCur = bCur->next;
    }

    // Читатели
    int readerCount = getReaderCount();
    out.write(reinterpret_cast<const char*>(&readerCount), sizeof(readerCount));
    ReaderNode* rCur = readersHead;
    while (rCur) {
        rCur->writeToBinary(out);
        rCur = rCur->next;
    }

    // Записи выдачи
    int issueCount = 0;
    IssueRecord* iCur = issuesHead;
    while (iCur) { ++issueCount; iCur = iCur->next; }
    out.write(reinterpret_cast<const char*>(&issueCount), sizeof(issueCount));
    iCur = issuesHead;
    while (iCur) {
        iCur->writeToBinary(out);
        iCur = iCur->next;
    }

    return out.good();
}

bool Library::loadFromFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;

    // Считываем счётчики ID
    in.read(reinterpret_cast<char*>(&nextBookId), sizeof(nextBookId));
    in.read(reinterpret_cast<char*>(&nextReaderId), sizeof(nextReaderId));

    // Книги
    int bookCount;
    in.read(reinterpret_cast<char*>(&bookCount), sizeof(bookCount));
    for (int i = 0; i < bookCount; ++i) {
        BookNode* node = BookNode::readFromBinary(in);
        if (!node) return false;
        if (!booksHead) {
            booksHead = booksTail = node;
        } else {
            booksTail->next = node;
            node->prev = booksTail;
            booksTail = node;
        }
    }

    // Читатели
    int readerCount;
    in.read(reinterpret_cast<char*>(&readerCount), sizeof(readerCount));
    for (int i = 0; i < readerCount; ++i) {
        ReaderNode* node = ReaderNode::readFromBinary(in);
        if (!node) return false;
        if (!readersHead) {
            readersHead = readersTail = node;
        } else {
            readersTail->next = node;
            node->prev = readersTail;
            readersTail = node;
        }
    }

    // Записи выдачи
    int issueCount;
    in.read(reinterpret_cast<char*>(&issueCount), sizeof(issueCount));
    for (int i = 0; i < issueCount; ++i) {
        IssueRecord* rec = IssueRecord::readFromBinary(in);
        if (!rec) return false;
        if (!issuesHead) {
            issuesHead = issuesTail = rec;
        } else {
            issuesTail->next = rec;
            rec->prev = issuesTail;
            issuesTail = rec;
        }
    }

    return in.good();
}
