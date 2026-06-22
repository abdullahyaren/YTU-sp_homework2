# 📚 Library Automation System

A menu-driven **library management system** built in ANSI C (C89). It manages author, book, and student records; tracks book borrow/return operations; and stores all data persistently in CSV and binary files.

> Yıldız Technical University – Computer Engineering
> **Structured Programming** course term project

---

## 📖 About the Project

This program is a console-based application that manages a library's **authors**, **books**, and **members (students)**. Since all data is written to files, records are preserved when the program is closed and reopened.

The system is built around three core entities:

| Entity | Data Structure | Ordering |
|--------|----------------|----------|
| **Author** | Singly linked list | Ascending by ID |
| **Book** | Singly linked list | By title (then ISBN) |
| **Student** | Dynamic array | Insertion order |

Book–author mappings and borrow/return transaction records are kept in dynamic arrays.

---

## ✨ Features

### 👤 Author Operations
- Add author (automatic ID assignment)
- Delete author (related book–author mappings are flagged as `-1`)
- Update author
- List all authors
- Display author info along with the books written by that author

### 📕 Book Operations
- Add book (automatic copy-tag generation: `ISBN_1`, `ISBN_2`, …)
- Delete book
- Update book
- Display book info and copy statuses
- List books currently on the shelf
- List overdue books
- Match book–author
- Update a book's author

### 🎓 Student Operations
- Add student (starting score of **100**)
- Delete student
- Update student
- Display student info and transaction history by ID or name-surname
- List students who have not yet returned books
- List penalized students (those with a score below 100)
- List all students
- **Borrow / return books**

### 🔄 Borrow / Return Logic
- Each borrowed copy is assigned to a student, and the transaction date is recorded
- On a **late return** (more than 15 days), **10 points** are deducted from the student
- A student with a negative score cannot borrow books
- Date differences are computed with custom-written date functions

---

## 🛠️ Technical Concepts Used

This project demonstrates the core structured-programming concepts required by the course:

- **Linked lists:** Author and book management via sorted insertion
- **Dynamic arrays:** Student, mapping, and transaction arrays that grow with `malloc` / `realloc`
- **`union` usage:** Copy status (`CopyStatus`) holds either the student ID or the shelf information
- **Function pointers:** CRUD functions invoked through arrays in the menus via the `EntityFunc` type
- **File operations:** Reading/writing both text (CSV) and binary files
- **Dynamic memory management:** All memory is released with `freeLibrary` at program exit
- **Custom string function:** `myStrCaseCmp` for case-insensitive comparison

---

## 💾 Data Files

The following files are created/updated automatically when the program runs:

| File | Format | Content |
|------|--------|---------|
| `authors.csv` | Text (CSV) | Author records |
| `students.csv` | Text (CSV) | Student records |
| `books.csv` | Text (CSV) | Book records |
| `copyInfo.csv` | Text (CSV) | Status of book copies |
| `bookAuthor.bin` | **Binary** | Book–author mappings |
| `borrowRecords.csv` | Text (CSV) | Borrow/return transaction history |

> On startup, the program attempts to read these files; if they don't exist, it starts with an empty library.

---

## 🚀 Build and Run

The project conforms to the **ANSI C (C89)** standard and requires no external libraries.

### Build
```bash
gcc -ansi -pedantic -Wall -o library sp_homework2.c
```

### Run
```bash
./library
```

On Windows:
```bash
gcc -ansi -pedantic -Wall -o library.exe sp_homework2.c
library.exe
```

---

## 🗂️ Menu Structure

```
LIBRARY AUTOMATION SYSTEM
│
├── 1. Student Operations
│   ├── Add / Delete / Update
│   ├── Display student info
│   ├── List students with unreturned books
│   ├── List penalized students
│   ├── List all students
│   └── Borrow / return book
│
├── 2. Book Operations
│   ├── Add / Delete / Update
│   ├── Display book info
│   ├── List books on shelf
│   ├── List overdue books
│   ├── Match book–author
│   └── Update book's author
│
├── 3. Author Operations
│   ├── Add / Delete / Update
│   ├── List all authors
│   └── Display author info
│
└── 0. Exit
```

---

## ⚙️ Constants (Rules)

| Constant | Value | Description |
|----------|-------|-------------|
| Starting score | `100` | Score of a newly added student |
| `LATE_DAYS` | `15` days | Overdue threshold |
| `LATE_PENALTY` | `10` points | Penalty applied on a late return |

---

## 📌 Coding Constraints Followed

- ✅ **ANSI C (C89)** standard
- ✅ **No** global / static variables (all data is carried through the `LibraryData` struct by pointer)
- ✅ `break` / `continue` used only inside `switch-case`
- ✅ **lowerCamelCase** naming
- ✅ Dynamic memory allocation with proper deallocation

---

## 👨‍💻 Author

A student project for Yıldız Technical University – Computer Engineering.
