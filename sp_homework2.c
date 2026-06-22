#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_ISBN 15
#define MAX_TAG 40
#define MAX_DATE 15
#define MAX_TITLE 100
#define INIT_CAP 10
#define LATE_PENALTY 10
#define LATE_DAYS 15

#define AUTHORS_FILE "authors.csv"
#define STUDENTS_FILE "students.csv"
#define BOOKS_FILE "books.csv"
#define COPIES_FILE "copyInfo.csv"
#define BOOKAUTHOR_FILE "bookAuthor.bin"
#define BORROW_FILE "borrowRecords.csv"

typedef union{
    int studentId;
    char shelf[12];
} CopyStatus;

typedef struct{
    char tagNo[MAX_TAG];
    CopyStatus status;
    int isBorrowed;
} BookCopy;

typedef struct BookNode{
    char title[MAX_TITLE];
    char isbn[MAX_ISBN];
    int quantity;
    BookCopy *copies;
    struct BookNode *next;
} BookNode;

typedef struct AuthorNode{
    int authorId;
    char name[MAX_NAME];
    char surname[MAX_NAME];
    struct AuthorNode *next;
} AuthorNode;

typedef struct{
    char name[MAX_NAME];
    char surname[MAX_NAME];
    int studentId;
    int score;
} Student;

typedef struct{
    char isbn[MAX_ISBN];
    int authorId;
} BookAuthor;

typedef struct{
    char tagNo[MAX_TAG];
    int studentId;
    int transType;
    char date[MAX_DATE];
} BorrowRecord;

typedef struct{
    AuthorNode *authorHead;
    int nextAuthorId;
    Student *students;
    int studentCount;
    int studentCapacity;
    BookNode *bookHead;
    BookAuthor *bookAuthors;
    int bookAuthorCount;
    int bookAuthorCapacity;
    BorrowRecord *borrowRecords;
    int borrowCount;
    int borrowCapacity;
} LibraryData;

typedef void (*EntityFunc)(LibraryData *);
void getCurrentDate(char *dateStr);
int dateToDays(const char *date);
int daysBetween(const char *from, const char *to);
void clearBuffer(void);
int myStrCaseCmp(const char *a, const char *b);
void initLibrary(LibraryData *lib);
void freeLibrary(LibraryData *lib);
void loadAllData(LibraryData *lib);
AuthorNode *createAuthorNode(int id, const char *name, const char *surname);
void insertAuthorSorted(LibraryData *lib, AuthorNode *node);
void saveAuthors(LibraryData *lib);
void loadAuthors(LibraryData *lib);
AuthorNode *findAuthorById(LibraryData *lib, int id);
AuthorNode *findAuthorByName(LibraryData *lib, const char *name, const char *surname);
void addAuthor(LibraryData *lib);
void deleteAuthor(LibraryData *lib);
void updateAuthor(LibraryData *lib);
void listAuthors(LibraryData *lib);
void displayAuthorInfo(LibraryData *lib);
void authorMenu(LibraryData *lib);
void saveStudents(LibraryData *lib);
void loadStudents(LibraryData *lib);
Student *findStudentById(LibraryData *lib, int id);
Student *findStudentByName(LibraryData *lib, const char *name, const char *surname);
void addStudent(LibraryData *lib);
void deleteStudent(LibraryData *lib);
void updateStudent(LibraryData *lib);
void listAllStudents(LibraryData *lib);
void listStudentsNotReturned(LibraryData *lib);
void listPenalizedStudents(LibraryData *lib);
void displayStudentInfo(LibraryData *lib);
void borrowReturnBook(LibraryData *lib);
void studentMenu(LibraryData *lib);
BookNode *createBookNode(const char *title, const char *isbn, int quantity);
void insertBookSorted(LibraryData *lib, BookNode *node);
void saveBooks(LibraryData *lib);
void saveCopies(LibraryData *lib);
void loadBooks(LibraryData *lib);
void loadCopies(LibraryData *lib);
BookNode *findBookByIsbn(LibraryData *lib, const char *isbn);
BookNode *findBookByTitle(LibraryData *lib, const char *title);
void addBook(LibraryData *lib);
void deleteBook(LibraryData *lib);
void updateBook(LibraryData *lib);
void displayBookInfo(LibraryData *lib);
void listBooksOnShelf(LibraryData *lib);
void listOverdueBooks(LibraryData *lib);
void matchBookAuthor(LibraryData *lib);
void updateBookAuthorMapping(LibraryData *lib);
void bookMenu(LibraryData *lib);
void saveBookAuthors(LibraryData *lib);
void loadBookAuthors(LibraryData *lib);
void saveBorrowRecords(LibraryData *lib);
void loadBorrowRecords(LibraryData *lib);
void mainMenu(LibraryData *lib);
void borrowBook(LibraryData *lib);
void returnBook(LibraryData *lib);

void getCurrentDate(char *dateStr){
    time_t t;
    struct tm *tmPtr;
    t=time(NULL);
    tmPtr=localtime(&t);
    sprintf(dateStr, "%02d.%02d.%04d", tmPtr->tm_mday, tmPtr->tm_mon+1, tmPtr->tm_year+1900);
}

int dateToDays(const char *date){
    int day, month, year, i, days;
    int months[13];
    months[0]=0;
    months[1]=31;
    months[2]=28; 
    months[3]=31;
    months[4]=30; 
    months[5]=31; 
    months[6]=30; 
    months[7]=31;
    months[8]=31; 
    months[9]=30; 
    months[10]=31; 
    months[11]=30;
    months[12]=31;
    sscanf(date, "%d.%d.%d", &day, &month, &year);
    days=year*365+day;
    for(i=1;i<month;i++){
        days+=months[i];
    }
    days+=year/4;
    return days;
}

int daysBetween(const char *from, const char *to){
    return dateToDays(to)-dateToDays(from);
}

void clearBuffer(void){
    int c;
    c=getchar();
    while(c!='\n'&&c!=EOF){
        c=getchar();
    }
}

int myStrCaseCmp(const char *a, const char *b){
    int ca, cb;
    ca=tolower((unsigned char)*a);
    cb=tolower((unsigned char)*b);
    while(ca!='\0' && cb!='\0' && ca==cb){
        a++;
        b++;
        ca=tolower((unsigned char)*a);
        cb=tolower((unsigned char)*b);
    }
    return ca-cb;
}

void initLibrary(LibraryData *lib) {
    lib->authorHead=NULL;
    lib->nextAuthorId=1;
    lib->students=(Student *)malloc(INIT_CAP*sizeof(Student));
    lib->studentCount=0;
    lib->studentCapacity=INIT_CAP;
    lib->bookHead=NULL;
    lib->bookAuthors=(BookAuthor *)malloc(INIT_CAP*sizeof(BookAuthor));
    lib->bookAuthorCount=0;
    lib->bookAuthorCapacity=INIT_CAP;
    lib->borrowRecords=(BorrowRecord *)malloc(INIT_CAP*sizeof(BorrowRecord));
    lib->borrowCount=0;
    lib->borrowCapacity=INIT_CAP;
}

void freeLibrary(LibraryData *lib){
    AuthorNode *aCurr, *aNext;
    BookNode *bCurr, *bNext;
    aCurr=lib->authorHead;
    while(aCurr!=NULL){
        aNext=aCurr->next;
        free(aCurr);
        aCurr=aNext;
    }
    bCurr=lib->bookHead;
    while(bCurr!= NULL){
        bNext=bCurr->next;
        if(bCurr->copies!=NULL){
            free(bCurr->copies);
        }
        free(bCurr);
        bCurr=bNext;
    }
    free(lib->students);
    free(lib->bookAuthors);
    free(lib->borrowRecords);
}

AuthorNode *createAuthorNode(int id, const char *name, const char *surname){
    AuthorNode *node=(AuthorNode *)malloc(sizeof(AuthorNode));
    node->authorId=id;
    strncpy(node->name, name, MAX_NAME-1);
    node->name[MAX_NAME-1]='\0';
    strncpy(node->surname, surname, MAX_NAME-1);
    node->surname[MAX_NAME-1]='\0';
    node->next=NULL;
    return node;
}

void insertAuthorSorted(LibraryData *lib, AuthorNode *newNode) {
    AuthorNode *curr, *prev;
    if(lib->authorHead==NULL || lib->authorHead->authorId > newNode->authorId){
        newNode->next=lib->authorHead;
        lib->authorHead=newNode;
    } else{
        prev=lib->authorHead;
        curr=lib->authorHead->next;
        while(curr!=NULL && curr->authorId < newNode->authorId){
            prev=curr;
            curr=curr->next;
        }
        newNode->next=curr;
        prev->next=newNode;
    }
}

void saveAuthors(LibraryData *lib){
    FILE *fp;
    AuthorNode *curr;
    fp=fopen(AUTHORS_FILE, "w");
    if (fp == NULL) {
        printf("ERROR: %s file could not be opened. \n", AUTHORS_FILE);
        return;
    }
    curr=lib->authorHead;
    while(curr!=NULL){
        fprintf(fp, "%d,%s,%s \n", curr->authorId, curr->name, curr->surname);
        curr=curr->next;
    }
    fclose(fp);
}

void loadAuthors(LibraryData *lib){
    FILE *fp;
    char line[200], name[MAX_NAME], surname[MAX_NAME];
    char *tok;
    int id;
    AuthorNode *node;
    fp=fopen(AUTHORS_FILE, "r");
    if(fp==NULL){
        return;
    }
    while(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]='\0';
        tok=strtok(line, ",");
        if(tok!=NULL) {
            id=atoi(tok);
            tok=strtok(NULL, ",");
            if(tok!=NULL){
                strncpy(name, tok, MAX_NAME - 1);
                name[MAX_NAME-1]='\0';
                tok=strtok(NULL, ",");
                if(tok!=NULL){
                    strncpy(surname, tok, MAX_NAME-1);
                    surname[MAX_NAME-1]='\0';
                    node=createAuthorNode(id, name, surname);
                    insertAuthorSorted(lib, node);
                    if(id >= lib->nextAuthorId){
                        lib->nextAuthorId=id+1;
                    }
                }
            }
        }
    }
    fclose(fp);
}

AuthorNode *findAuthorById(LibraryData *lib, int id){
    AuthorNode *curr=lib->authorHead;
    while(curr!=NULL && curr->authorId!=id){
        curr=curr->next;
    }
    return curr;
}

AuthorNode *findAuthorByName(LibraryData *lib, const char *name, const char *surname){
    AuthorNode *curr=lib->authorHead;
    while(curr!=NULL && !(myStrCaseCmp(curr->name, name)==0 && myStrCaseCmp(curr->surname, surname)==0)){
        curr=curr->next;
    }
    return curr;
}

void addAuthor(LibraryData *lib) {
    char name[MAX_NAME], surname[MAX_NAME];
    AuthorNode *node;
    printf("Author Name: \n");
    scanf(" %49[^\n]", name);
    printf("Author Surname: \n");
    scanf(" %49[^\n]", surname);
    clearBuffer();
    node=createAuthorNode(lib->nextAuthorId++, name, surname);
    insertAuthorSorted(lib, node);
    saveAuthors(lib);
    printf(">> Author added. ID: %d\n", node->authorId);
}

void deleteAuthor(LibraryData *lib) {
    int id, i;
    AuthorNode *curr, *prev;
    FILE *fp;
    printf("Author ID to delete: \n");
    scanf("%d", &id);
    clearBuffer();
    if(lib->authorHead==NULL){
        printf("No registered authors. \n");
        return;
    }
    if(lib->authorHead->authorId==id){
        curr=lib->authorHead;
        lib->authorHead=curr->next;
        free(curr);
    } else{
        prev=lib->authorHead;
        curr=prev->next;
        while(curr!=NULL && curr->authorId!=id){
            prev=curr;
            curr=curr->next;
        }
        if(curr==NULL){
            printf("Author not found. \n");
            return;
        }
        prev->next=curr->next;
        free(curr);
    }
    for(i=0;i < lib->bookAuthorCount;i++){
        if(lib->bookAuthors[i].authorId==id){
            lib->bookAuthors[i].authorId=-1;
        }
    }
    saveAuthors(lib);
    fp=fopen(BOOKAUTHOR_FILE, "wb");
    if(fp!=NULL){
        fwrite(lib->bookAuthors, sizeof(BookAuthor), lib->bookAuthorCount, fp);
        fclose(fp);
    }
    printf(">> Author deleted. Related mappings updated to -1. \n");
}

void updateAuthor(LibraryData *lib) {
    int id;
    AuthorNode *author;
    printf("Author ID to update: \n");
    scanf("%d", &id);
    clearBuffer();
    author=findAuthorById(lib, id);
    if (author == NULL) {
        printf("Author not found. \n");
        return;
    }
    printf("New Name (current: %s): \n", author->name);
    scanf(" %49[^\n]", author->name);
    printf("New Surname (current: %s): \n", author->surname);
    scanf(" %49[^\n]", author->surname);
    clearBuffer();
    saveAuthors(lib);
    printf(">> Author updated. \n");
}

void listAuthors(LibraryData *lib) {
    AuthorNode *curr=lib->authorHead;
    printf("\n%-6s %-25s %-25s\n", "ID", "Name", "Surname");
    printf("----------------------------------------------------------\n");
    if(curr==NULL){
        printf("No registered authors. \n");
        return;
    }
    while(curr!=NULL){
        printf("%-6d %-25s %-25s\n", curr->authorId, curr->name, curr->surname);
        curr=curr->next;
    }
}

void displayAuthorInfo(LibraryData *lib) {
    char name[MAX_NAME], surname[MAX_NAME];
    AuthorNode *author;
    BookNode *book;
    int i, found;
    printf("Author Name: \n");
    scanf(" %49[^\n]", name);
    printf("Author Surname: \n");
    scanf(" %49[^\n]", surname);
    clearBuffer();
    author=findAuthorByName(lib, name, surname);
    if(author==NULL){
        printf("Author not found. \n");
        return;
    }
    printf("\n=== AUTHOR INFORMATION ===\n");
    printf("ID: %d\n", author->authorId);
    printf("Name: %s\n", author->name);
    printf("Surname: %s\n", author->surname);
    printf("\nBooks by the author:\n");
    found = 0;
    for (i = 0; i < lib->bookAuthorCount; i++) {
        if (lib->bookAuthors[i].authorId == author->authorId) {
            book = findBookByIsbn(lib, lib->bookAuthors[i].isbn);
            if (book != NULL) {
                printf(" - %-40s ISBN: %-15s Quantity: %d\n",
                       book->title, book->isbn, book->quantity);
                found = 1;
            }
        }
    }
    if (!found) {
        printf("No book mapping found for this author. \n");
    }
}

void authorMenu(LibraryData *lib) {
    int choice;
    EntityFunc authorCrud[3];
    authorCrud[0]=addAuthor;
    authorCrud[1]=deleteAuthor;
    authorCrud[2]=updateAuthor;
    do {
        printf("\n=== AUTHOR OPERATIONS ===\n");
        printf("1. Add Author\n");
        printf("2. Delete Author\n");
        printf("3. Update Author\n");
        printf("4. List All Authors\n");
        printf("5. Display Author Info\n");
        printf("0. Main Menu\n");
        printf("Your choice: ");
        scanf("%d", &choice);
        clearBuffer();
        switch (choice) {
            case 1:
                authorCrud[0](lib);
                break;
            case 2:
                authorCrud[1](lib);
                break;
            case 3: 
                authorCrud[2](lib);
                break;
            case 4:
                listAuthors(lib);
                break;
            case 5:
                displayAuthorInfo(lib);
                break;
            case 0:
                break;
            default:
                printf("Invalid choice!\n");
        }
    }while(choice!=0);
}

void saveStudents(LibraryData *lib){
    FILE *fp;
    int i;
    fp=fopen(STUDENTS_FILE, "w");
    if(fp==NULL){
        printf("ERROR: %s file could not be opened.\n", STUDENTS_FILE);
        return;
    }
    for(i=0;i<lib->studentCount;i++){
        fprintf(fp, "%d,%s,%s,%d\n", lib->students[i].studentId, lib->students[i].name, lib->students[i].surname, lib->students[i].score);
    }
    fclose(fp);
}

void loadStudents(LibraryData *lib){
    FILE *fp;
    char line[200];
    char *tok;
    Student s;
    fp=fopen(STUDENTS_FILE, "r");
    if(fp==NULL){
        return;
    }
    while(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]='\0';
        tok=strtok(line, ",");
        if (tok!=NULL) {
            s.studentId=atoi(tok);
            tok=strtok(NULL, ",");
            if (tok!=NULL){
                strncpy(s.name, tok, MAX_NAME-1);
                s.name[MAX_NAME-1]='\0';
                tok=strtok(NULL, ",");
                if(tok!=NULL){
                    strncpy(s.surname, tok, MAX_NAME-1);
                    s.surname[MAX_NAME-1]='\0';
                    tok=strtok(NULL, ",");
                    if(tok!=NULL){
                        s.score=atoi(tok);
                        if (lib->studentCount >= lib->studentCapacity){
                            lib->studentCapacity*=2;
                            lib->students=(Student *)realloc(lib->students, lib->studentCapacity*sizeof(Student));
                        }
                        lib->students[lib->studentCount]=s;
                        lib->studentCount++;
                    }
                }
            }
        }
    }
    fclose(fp);
}

Student *findStudentById(LibraryData *lib, int id){
    int i;
    Student *result=NULL;
    for(i=0;i < lib->studentCount && result==NULL;i++){
        if(lib->students[i].studentId==id){
            result=&lib->students[i];
        }
    }
    return result;
}

Student *findStudentByName(LibraryData *lib, const char *name, const char *surname){
    int i;
    Student *result=NULL;
    for(i=0;i < lib->studentCount && result==NULL;i++){
        if (myStrCaseCmp(lib->students[i].name, name)==0 && myStrCaseCmp(lib->students[i].surname, surname)==0){
            result=&lib->students[i];
        }
    }
    return result;
}

void addStudent(LibraryData *lib) {
    Student s;
    printf("Student Name: \n");
    scanf(" %49[^\n]", s.name);
    printf("Student Surname: \n");
    scanf(" %49[^\n]", s.surname);
    printf("Student No (8 digits): \n");
    scanf("%d", &s.studentId);
    clearBuffer();
    if(findStudentById(lib, s.studentId)!=NULL){
        printf("This number is already registered! \n");
        return;
    }
    s.score=100;
    if(lib->studentCount >= lib->studentCapacity){
        lib->studentCapacity*=2;
        lib->students=(Student *)realloc(lib->students, lib->studentCapacity*sizeof(Student));
    }
    lib->students[lib->studentCount]=s;
    lib->studentCount++;
    saveStudents(lib);
    printf(">> Student added. Starting score: 100\n");
}

void deleteStudent(LibraryData *lib){
    int id, i, j, found;
    printf("Student No to delete: ");
    scanf("%d", &id);
    clearBuffer();
    found=0;
    i=0;
    while(i < lib->studentCount && !found){
        if (lib->students[i].studentId==id) {
            found=1;
            for(j=i;j < lib->studentCount-1;j++){
                lib->students[j]=lib->students[j+1];
            }
            lib->studentCount--;
            saveStudents(lib);
            printf(">> Student deleted. \n");
        } else {
            i++;
        }
    }
    if(!found){
        printf("Student not found. \n");
    }
}

void updateStudent(LibraryData *lib){
    int id;
    Student *s;
    printf("Student No to update: \n");
    scanf("%d", &id);
    clearBuffer();
    s=findStudentById(lib, id);
    if(s==NULL){
        printf("Student not found. \n");
        return;
    }
    printf("New Name (current: %s): \n", s->name);
    scanf(" %49[^\n]", s->name);
    printf("New Surname (current: %s): \n", s->surname);
    scanf(" %49[^\n]", s->surname);
    clearBuffer();
    saveStudents(lib);
    printf(">> Student updated. \n");
}

void listAllStudents(LibraryData *lib){
    int i;
    printf("\n%-12s %-22s %-22s %-6s\n", "No", "Name", "Surname", "Score");
    printf("--------------------------------------------------------------\n");
    if(lib->studentCount==0){
        printf("No registered students. \n");
        return;
    }
    for(i=0;i<lib->studentCount;i++){
        printf("%-12d %-22s %-22s %-6d\n", lib->students[i].studentId, lib->students[i].name, lib->students[i].surname, lib->students[i].score);
    }
}

void listStudentsNotReturned(LibraryData *lib){
    int i, j, headerPrinted;
    BookNode *book;
    printf("\n=== STUDENTS WITH UNRETURNED BOOKS ===\n");
    for(i=0;i<lib->studentCount;i++){
        headerPrinted=0;
        book=lib->bookHead;
        while(book!=NULL){
            for(j=0;j<book->quantity;j++){
                if(book->copies[j].isBorrowed && book->copies[j].status.studentId==lib->students[i].studentId){
                    if(!headerPrinted){
                        printf("\nStudent: %s %s (No: %d, Score: %d)\n", lib->students[i].name, lib->students[i].surname, lib->students[i].studentId, lib->students[i].score);
                        headerPrinted=1;
                    }
                    printf("Tag: %-20s  Book: %s \n", book->copies[j].tagNo, book->title);
                }
            }
            book=book->next;
        }
    }
}

void listPenalizedStudents(LibraryData *lib){
    int i;
    printf("\n === PENALIZED STUDENTS (Score < 100) === \n");
    printf("%-12s %-22s %-22s %-6s\n", "No", "Name", "Surname", "Score");
    printf("--------------------------------------------------------------\n");
    for(i=0;i < lib->studentCount;i++){
        if(lib->students[i].score < 100){
            printf("%-12d %-22s %-22s %-6d\n", lib->students[i].studentId, lib->students[i].name, lib->students[i].surname, lib->students[i].score);
        }
    }
}

void displayStudentInfo(LibraryData *lib){
    int choice, id, i;
    char name[MAX_NAME], surname[MAX_NAME];
    Student *s=NULL;
    printf("1. Search by ID \n2. Search by Name-Surname \nYour choice: \n");
    scanf("%d", &choice);
    clearBuffer();
    if(choice==1){
        printf("Student No: \n");
        scanf("%d", &id);
        clearBuffer();
        s=findStudentById(lib, id);
    } else{
        printf("Name: \n");
        scanf(" %49[^\n]", name);
        printf("Surname: \n");
        scanf(" %49[^\n]", surname);
        clearBuffer();
        s=findStudentByName(lib, name, surname);
    }
    if(s==NULL){
        printf("Student not found. \n");
        return;
    }
    printf("\n === STUDENT INFORMATION === \n");
    printf("No: %d \n", s->studentId);
    printf("Name: %s \n", s->name);
    printf("Surname: %s \n", s->surname);
    printf("Score: %d \n", s->score);
    printf("Book Movements: \n");
    printf("%-22s %-12s %-10s %-12s\n", "Tag No", "Student No", "Action", "Date");
    printf("-------------------------------------------------------------- \n");
    for(i=0;i < lib->borrowCount;i++){
        if(lib->borrowRecords[i].studentId==s->studentId){
            printf("%-22s %-12d %-10s %-12s\n", lib->borrowRecords[i].tagNo, lib->borrowRecords[i].studentId, lib->borrowRecords[i].transType == 0 ? "BORROW" : "RETURN", lib->borrowRecords[i].date);
        }
    }
}

void borrowBook(LibraryData *lib) {
    char isbn[MAX_ISBN], dateStr[MAX_DATE];
    int studentId, i, availIdx;
    BookNode *book;
    Student *student;
    BorrowRecord rec;
    printf("Student No: \n");
    scanf("%d", &studentId);
    clearBuffer();
    student=findStudentById(lib, studentId);
    if (student==NULL){
        printf("No registered student found! \n");
        return;
    }
    if(student->score < 0){
        printf("Cannot lend book: Student score is negative! \n");
        return;
    }
    printf("ISBN: \n");
    scanf(" %14s", isbn);
    clearBuffer();
    book=findBookByIsbn(lib, isbn);
    if(book==NULL){
        printf("Book not found! \n");
        return;
    }
    availIdx=-1;
    for(i=0;i < book->quantity && availIdx==-1;i++){
        if(!book->copies[i].isBorrowed){
            availIdx=i;
        }
    }
    if(availIdx==-1){
        printf("OPERATION FAILED: All copies are borrowed! \n");
        return;
    }
    book->copies[availIdx].isBorrowed=1;
    book->copies[availIdx].status.studentId=studentId;
    getCurrentDate(dateStr);
    if(lib->borrowCount >= lib->borrowCapacity){
        lib->borrowCapacity*=2;
        lib->borrowRecords=(BorrowRecord *)realloc(lib->borrowRecords, lib->borrowCapacity*sizeof(BorrowRecord));
    }
    strncpy(rec.tagNo, book->copies[availIdx].tagNo, MAX_TAG-1);
    rec.tagNo[MAX_TAG-1]='\0';
    rec.studentId=studentId;
    rec.transType=0;
    strncpy(rec.date, dateStr, MAX_DATE-1);
    rec.date[MAX_DATE-1]='\0';
    lib->borrowRecords[lib->borrowCount]=rec;
    lib->borrowCount++;
    saveCopies(lib);
    saveBorrowRecords(lib);
    printf(">> Book borrowed: %s -> Student %d  (Date: %s)\n", book->copies[availIdx].tagNo, studentId, dateStr);
}

void returnBook(LibraryData *lib) {
    char tagNo[MAX_TAG], dateStr[MAX_DATE], borrowDate[MAX_DATE];
    int studentId, i, j, found, dateFound, diff;
    BookNode *book;
    Student *student;
    BorrowRecord rec;
    printf("Student No: \n");
    scanf("%d", &studentId);
    clearBuffer();
    student=findStudentById(lib, studentId);
    if(student==NULL){
        printf("No registered student found! \n");
        return;
    }
    printf("Book Tag No: \n");
    scanf(" %39s", tagNo);
    clearBuffer();
    found=0;
    book=lib->bookHead;
    while(book!=NULL && !found){
        for(j=0;j < book->quantity && !found;j++){
            if(strcmp(book->copies[j].tagNo, tagNo)==0 && book->copies[j].isBorrowed && book->copies[j].status.studentId==studentId){
                book->copies[j].isBorrowed=0;
                strncpy(book->copies[j].status.shelf, "ON_SHELF", 11);
                book->copies[j].status.shelf[11]='\0';
                found=1;
            }
        }
        if(!found){
            book=book->next;
        }
    }
    if(!found){
        printf("Book not found with this student! \n");
        return;
    }
    borrowDate[0]='\0';
    dateFound=0;
    for(i=lib->borrowCount-1;i >= 0 && !dateFound;i--){
        if(strcmp(lib->borrowRecords[i].tagNo, tagNo)==0 &&
            lib->borrowRecords[i].studentId==studentId && lib->borrowRecords[i].transType==0){
            strncpy(borrowDate, lib->borrowRecords[i].date, MAX_DATE-1);
            borrowDate[MAX_DATE-1]='\0';
            dateFound=1;
        }
    }
    getCurrentDate(dateStr);
    if(dateFound){
        diff=daysBetween(borrowDate, dateStr);
        if(diff > LATE_DAYS){
            student->score-=LATE_PENALTY;
            printf(">> Late return (%d days)! %d point penalty. New score: %d \n", diff, LATE_PENALTY, student->score);
            saveStudents(lib);
        }
    }
    if(lib->borrowCount >= lib->borrowCapacity){
        lib->borrowCapacity*=2;
        lib->borrowRecords=(BorrowRecord *)realloc(lib->borrowRecords, lib->borrowCapacity*sizeof(BorrowRecord));
    }
    strncpy(rec.tagNo, tagNo, MAX_TAG-1);
    rec.tagNo[MAX_TAG-1]='\0';
    rec.studentId=studentId;
    rec.transType=1;
    strncpy(rec.date, dateStr, MAX_DATE-1);
    rec.date[MAX_DATE-1]='\0';
    lib->borrowRecords[lib->borrowCount]=rec;
    lib->borrowCount++;
    saveCopies(lib);
    saveBorrowRecords(lib);
    printf(">> Book returned: %s  (Date: %s) \n", tagNo, dateStr);
}

void borrowReturnBook(LibraryData *lib){
    int choice;
    printf("1. Borrow Book \n2. Return Book \nYour choice: \n");
    scanf("%d", &choice);
    clearBuffer();
    if(choice==1){
        borrowBook(lib);
    } else if(choice==2){
        returnBook(lib);
    } else{
        printf("Invalid choice! \n");
    }
}

void studentMenu(LibraryData *lib){
    int choice;
    EntityFunc studentCrud[3];
    studentCrud[0]=addStudent;
    studentCrud[1]=deleteStudent;
    studentCrud[2]=updateStudent;
    do{
        printf("\n=== STUDENT OPERATIONS === \n");
        printf("1. Add Student \n");
        printf("2. Delete Student \n");
        printf("3. Update Student \n");
        printf("4. Display Student Info \n");
        printf("5. List Students with Unreturned Books \n");
        printf("6. List Penalized Students \n");
        printf("7. List All Students \n");
        printf("8. Borrow / Return Book \n");
        printf("0. Main Menu \n");
        printf("Your choice: \n");
        scanf("%d", &choice);
        clearBuffer();
        switch(choice){
            case 1:
                studentCrud[0](lib);
                break;
            case 2:
                studentCrud[1](lib);
                break;
            case 3:
                studentCrud[2](lib);
                break;
            case 4:
                displayStudentInfo(lib);
                break;
            case 5:
                listStudentsNotReturned(lib);
                break;
            case 6:
                listPenalizedStudents(lib);
                break;
            case 7:
                listAllStudents(lib);
                break;
            case 8:
                borrowReturnBook(lib);
                break;
            case 0:
                break;
            default:
                printf("Invalid choice! \n");
        }
    }while(choice!=0);
}

BookNode *createBookNode(const char *title, const char *isbn, int quantity){
    int i;
    char tagNo[MAX_TAG];
    BookNode *node=(BookNode *)malloc(sizeof(BookNode));
    strncpy(node->title, title, MAX_TITLE-1);
    node->title[MAX_TITLE-1]='\0';
    strncpy(node->isbn, isbn, MAX_ISBN-1);
    node->isbn[MAX_ISBN-1]='\0';
    node->quantity=quantity;
    node->copies=(BookCopy *)malloc(quantity*sizeof(BookCopy));
    for(i=0;i < quantity;i++){
        sprintf(tagNo, "%s_%d", isbn, i+1);
        strncpy(node->copies[i].tagNo, tagNo, MAX_TAG-1);
        node->copies[i].tagNo[MAX_TAG-1]='\0';
        node->copies[i].isBorrowed=0;
        strncpy(node->copies[i].status.shelf, "ON_SHELF", 11);
        node->copies[i].status.shelf[11]='\0';
    }
    node->next=NULL;
    return node;
}

void insertBookSorted(LibraryData *lib, BookNode *newNode){
    int cmp;
    int isPlaced;
    BookNode *curr, *prev;
    if(lib->bookHead==NULL){
        lib->bookHead=newNode;
        return;
    }
    cmp=myStrCaseCmp(newNode->title, lib->bookHead->title);
    if(cmp < 0 || (cmp==0 && strcmp(newNode->isbn, lib->bookHead->isbn) < 0)){
        newNode->next=lib->bookHead;
        lib->bookHead=newNode;
        return;
    }
    prev=lib->bookHead;
    curr=lib->bookHead->next;
    isPlaced=0;
    while(curr!=NULL && !isPlaced){
        cmp=myStrCaseCmp(newNode->title, curr->title);
        if(cmp < 0 || (cmp==0 && strcmp(newNode->isbn, curr->isbn) < 0)){
            isPlaced=1;
        } else{
            prev=curr;
            curr=curr->next;
        }
    }
    newNode->next=curr;
    prev->next=newNode;
}

void saveBooks(LibraryData *lib){
    FILE *fp;
    BookNode *curr;
    fp=fopen(BOOKS_FILE, "w");
    if(fp==NULL){
        printf("ERROR: %s file could not be opened. \n", BOOKS_FILE);
        return;
    }
    curr=lib->bookHead;
    while(curr!=NULL){
        fprintf(fp, "%s,%s,%d\n", curr->title, curr->isbn, curr->quantity);
        curr=curr->next;
    }
    fclose(fp);
}

void saveCopies(LibraryData *lib){
    FILE *fp;
    BookNode *curr;
    int i;
    fp=fopen(COPIES_FILE, "w");
    if(fp==NULL){
        printf("ERROR: %s file could not be opened. \n", COPIES_FILE);
        return;
    }
    curr=lib->bookHead;
    while(curr!=NULL){
        for(i=0;i < curr->quantity;i++){
            if(curr->copies[i].isBorrowed){
                fprintf(fp, "%s,%d \n", curr->copies[i].tagNo, curr->copies[i].status.studentId);
            } else{
                fprintf(fp, "%s,ON_SHELF \n", curr->copies[i].tagNo);
            }
        }
        curr=curr->next;
    }
    fclose(fp);
}

void loadBooks(LibraryData *lib){
    FILE *fp;
    char line[300], title[MAX_TITLE], isbn[MAX_ISBN];
    char *lastComma;
    int quantity, i;
    BookNode *node;
    fp=fopen(BOOKS_FILE, "r");
    if(fp==NULL){
        return;
    }
    while(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]='\0';
        lastComma=strrchr(line, ',');
        if(lastComma!=NULL){
            quantity=atoi(lastComma+1);
            *lastComma='\0';
            lastComma=strrchr(line, ',');
            if(lastComma!=NULL){
                strncpy(isbn, lastComma+1, MAX_ISBN-1);
                isbn[MAX_ISBN-1]='\0';
                *lastComma='\0';
                strncpy(title, line, MAX_TITLE-1);
                title[MAX_TITLE-1]='\0';
                node=(BookNode *)malloc(sizeof(BookNode));
                strncpy(node->title, title, MAX_TITLE-1);
                node->title[MAX_TITLE-1]='\0';
                strncpy(node->isbn, isbn, MAX_ISBN-1);
                node->isbn[MAX_ISBN-1]='\0';
                node->quantity=quantity;
                node->copies=(BookCopy *)malloc(quantity*sizeof(BookCopy));
                for(i=0;i < quantity;i++){
                    char tagNo[MAX_TAG];
                    sprintf(tagNo, "%s_%d", isbn,i+1);
                    strncpy(node->copies[i].tagNo, tagNo, MAX_TAG-1);
                    node->copies[i].tagNo[MAX_TAG-1]='\0';
                    node->copies[i].isBorrowed=0;
                    strncpy(node->copies[i].status.shelf, "ON_SHELF", 11);
                    node->copies[i].status.shelf[11]='\0';
                }
                node->next=NULL;
                insertBookSorted(lib, node);
            }
        }
    }
    fclose(fp);
}

void loadCopies(LibraryData *lib) {
    FILE *fp;
    char line[200], tagNo[MAX_TAG], status[MAX_TAG], isbnBuf[MAX_ISBN];
    char *tok, *underscore;
    BookNode *book;
    int i, isMatched;
    fp=fopen(COPIES_FILE, "r");
    if(fp==NULL){
        return;
    }
    while(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]='\0';
        tok=strtok(line, ",");
        if(tok!=NULL){
            strncpy(tagNo, tok, MAX_TAG-1);
            tagNo[MAX_TAG-1]='\0';
            tok=strtok(NULL, ",");
            if(tok!=NULL){
                strncpy(status, tok, MAX_TAG-1);
                status[MAX_TAG-1]='\0';
                underscore=strrchr(tagNo, '_');
                if(underscore!=NULL){
                    int len=(int)(underscore-tagNo);
                    if(len >= MAX_ISBN){
                        len=MAX_ISBN-1;
                    }
                    strncpy(isbnBuf, tagNo, len);
                    isbnBuf[len]='\0';
                    book=findBookByIsbn(lib, isbnBuf);
                    if(book!=NULL) {
                        isMatched=0;
                        for(i=0;i < book->quantity && !isMatched;i++){
                            if(strcmp(book->copies[i].tagNo, tagNo)==0){
                                if(strcmp(status, "ON_SHELF")==0){
                                    book->copies[i].isBorrowed=0;
                                    strncpy(book->copies[i].status.shelf, "ON_SHELF", 11);
                                    book->copies[i].status.shelf[11]='\0';
                                } else{
                                    book->copies[i].isBorrowed=1;
                                    book->copies[i].status.studentId=atoi(status);
                                }
                                isMatched=1;
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(fp);
}

BookNode *findBookByIsbn(LibraryData *lib, const char *isbn){
    BookNode *curr=lib->bookHead;
    while(curr!=NULL && strcmp(curr->isbn, isbn)!=0){
        curr=curr->next;
    }
    return curr;
}

BookNode *findBookByTitle(LibraryData *lib, const char *title){
    BookNode *curr=lib->bookHead;
    while(curr!=NULL && myStrCaseCmp(curr->title, title)!=0){
        curr=curr->next;
    }
    return curr;
}

void addBook(LibraryData *lib) {
    char title[MAX_TITLE], isbn[MAX_ISBN];
    int quantity;
    BookNode *node;
    printf("Book Title: \n");
    scanf(" %99[^\n]", title);
    printf("ISBN (13 digits): \n");
    scanf(" %14s", isbn);
    clearBuffer();
    if(findBookByIsbn(lib, isbn)!=NULL){
        printf("This ISBN is already registered! \n");
        return;
    }
    printf("Quantity: \n");
    scanf("%d", &quantity);
    clearBuffer();
    node=createBookNode(title, isbn, quantity);
    insertBookSorted(lib, node);
    saveBooks(lib);
    saveCopies(lib);
    printf(">> Book added. %d copies created. \n", quantity);
}

void deleteBook(LibraryData *lib){
    char isbn[MAX_ISBN];
    BookNode *curr, *prev;
    printf("Book ISBN to delete: \n");
    scanf(" %14s", isbn);
    clearBuffer();
    if(lib->bookHead==NULL){
        printf("No registered books. \n");
        return;
    }
    if(strcmp(lib->bookHead->isbn, isbn)==0){
        curr=lib->bookHead;
        lib->bookHead=curr->next;
        if(curr->copies!=NULL){
            free(curr->copies);
        }
        free(curr);
    } else{
        prev=lib->bookHead;
        curr=prev->next;
        while(curr!=NULL && strcmp(curr->isbn, isbn)!=0){
            prev=curr;
            curr=curr->next;
        }
        if(curr==NULL){
            printf("Book not found. \n");
            return;
        }
        prev->next=curr->next;
        if(curr->copies!=NULL){
            free(curr->copies);
        }
        free(curr);
    }
    saveBooks(lib);
    saveCopies(lib);
    printf(">> Book deleted. \n");
}

void updateBook(LibraryData *lib){
    char isbn[MAX_ISBN];
    BookNode *book;
    printf("Book ISBN to update: \n");
    scanf(" %14s", isbn);
    clearBuffer();
    book=findBookByIsbn(lib, isbn);
    if(book==NULL){
        printf("Book not found. \n");
        return;
    }
    printf("New Title (current: %s): \n", book->title);
    scanf(" %99[^\n]", book->title);
    clearBuffer();
    saveBooks(lib);
    printf(">> Book updated. \n");
}

void displayBookInfo(LibraryData *lib){
    char title[MAX_TITLE];
    BookNode *book;
    int i;
    printf("Book Title: \n");
    scanf(" %99[^\n]", title);
    clearBuffer();
    book=findBookByTitle(lib, title);
    if(book==NULL){
        printf("Book not found.\n");
        return;
    }
    printf("\n=== BOOK INFORMATION === \n");
    printf("Title: %s \n", book->title);
    printf("ISBN: %s \n", book->isbn);
    printf("Quantity: %d \n", book->quantity);
    printf("\n%-22s %-15s \n", "Tag No", "Status");
    printf("--------------------------------------\n");
    for(i=0;i < book->quantity;i++) {
        if (book->copies[i].isBorrowed) {
            printf("%-22s Borrowed - Student: %d \n", book->copies[i].tagNo, book->copies[i].status.studentId);
        } else{
            printf("%-22s ON_SHELF \n", book->copies[i].tagNo);
        }
    }
}

void listBooksOnShelf(LibraryData *lib){
    BookNode *curr=lib->bookHead;
    int i, any;
    printf("\n=== BOOKS ON SHELF === \n");
    while(curr!=NULL){
        any=0;
        for(i=0;i<curr->quantity && !any;i++){
            if(!curr->copies[i].isBorrowed){
                any=1;
            }
        }
        if(any){
            printf("\n%s (ISBN: %s)\n", curr->title, curr->isbn);
            for(i=0;i < curr->quantity;i++){
                if(!curr->copies[i].isBorrowed){
                    printf("  On Shelf: %s \n", curr->copies[i].tagNo);
                }
            }
        }
        curr=curr->next;
    }
}

void listOverdueBooks(LibraryData *lib){
    BookNode *book;
    char today[MAX_DATE], borrowDate[MAX_DATE];
    int i, j, diff, dateFound;
    getCurrentDate(today);
    printf("\n=== OVERDUE BOOKS === \n");
    printf("%-22s %-12s %-14s %-10s\n", "Tag No", "Student No", "Borrow Date", "Days Late");
    printf("-------------------------------------------------------------- \n");
    book=lib->bookHead;
    while(book!=NULL){
        for(j=0;j < book->quantity;j++){
            if(book->copies[j].isBorrowed){
                borrowDate[0]='\0';
                dateFound=0;
                for(i=lib->borrowCount-1;i >= 0 && !dateFound;i--){
                    if(strcmp(lib->borrowRecords[i].tagNo, book->copies[j].tagNo)==0 && lib->borrowRecords[i].studentId==book->copies[j].status.studentId && lib->borrowRecords[i].transType==0){
                        strncpy(borrowDate, lib->borrowRecords[i].date, MAX_DATE-1);
                        borrowDate[MAX_DATE-1]='\0';
                        dateFound=1;
                    }
                }
                if(dateFound){
                    diff=daysBetween(borrowDate, today);
                    if(diff > LATE_DAYS){
                        printf("%-22s %-12d %-14s %-10d \n", book->copies[j].tagNo, book->copies[j].status.studentId, borrowDate, diff);
                    }
                }
            }
        }
        book=book->next;
    }
}

void saveBookAuthors(LibraryData *lib){
    FILE *fp;
    fp=fopen(BOOKAUTHOR_FILE, "wb");
    if(fp==NULL){
        printf("ERROR: %s file could not be opened. \n", BOOKAUTHOR_FILE);
        return;
    }
    fwrite(lib->bookAuthors, sizeof(BookAuthor), lib->bookAuthorCount, fp);
    fclose(fp);
}

void loadBookAuthors(LibraryData *lib){
    FILE *fp;
    long fileSize;
    int count;
    fp=fopen(BOOKAUTHOR_FILE, "rb");
    if(fp==NULL){
        return;
    }
    fseek(fp, 0, SEEK_END);
    fileSize=ftell(fp);
    fseek(fp, 0, SEEK_SET);
    count=(int)(fileSize / sizeof(BookAuthor));
    if(count > lib->bookAuthorCapacity){
        lib->bookAuthorCapacity=count+INIT_CAP;
        lib->bookAuthors=(BookAuthor *)realloc(lib->bookAuthors, lib->bookAuthorCapacity*sizeof(BookAuthor));
    }
    lib->bookAuthorCount=(int)fread(lib->bookAuthors, sizeof(BookAuthor), count, fp);
    fclose(fp);
}

void matchBookAuthor(LibraryData *lib){
    char isbn[MAX_ISBN];
    int authorId, i, isExisting;
    BookAuthor ba;
    printf("Book ISBN: \n");
    scanf(" %14s", isbn);
    clearBuffer();
    if(findBookByIsbn(lib, isbn)==NULL){
        printf("Book not found! \n");
        return;
    }
    printf("Author ID: \n");
    scanf("%d", &authorId);
    clearBuffer();
    if(findAuthorById(lib, authorId)==NULL){
        printf("Author not found! \n");
        return;
    }
    isExisting=0;
    for(i=0;i < lib->bookAuthorCount && !isExisting;i++){
        if(strcmp(lib->bookAuthors[i].isbn, isbn)==0 && lib->bookAuthors[i].authorId==authorId){
            isExisting=1;
        }
    }
    if(isExisting){
        printf("This mapping already exists! \n");
        return;
    }
    if(lib->bookAuthorCount >= lib->bookAuthorCapacity){
        lib->bookAuthorCapacity*=2;
        lib->bookAuthors=(BookAuthor *)realloc(lib->bookAuthors, lib->bookAuthorCapacity*sizeof(BookAuthor));
    }
    strncpy(ba.isbn, isbn, MAX_ISBN-1);
    ba.isbn[MAX_ISBN-1]='\0';
    ba.authorId=authorId;
    lib->bookAuthors[lib->bookAuthorCount]=ba;
    lib->bookAuthorCount++;
    saveBookAuthors(lib);
    printf(">> Book-Author mapped.\n");
}

void updateBookAuthorMapping(LibraryData *lib){
    char isbn[MAX_ISBN];
    int oldId, newId, i, found;
    printf("Book ISBN: \n");
    scanf(" %14s", isbn);
    clearBuffer();
    if(findBookByIsbn(lib, isbn)==NULL){
        printf("Book not found! \n");
        return;
    }
    printf("Old Author ID: \n");
    scanf("%d", &oldId);
    clearBuffer();
    printf("New Author ID: \n");
    scanf("%d", &newId);
    clearBuffer();
    if (findAuthorById(lib, newId)==NULL){
        printf("New author not found!\n");
        return;
    }
    found=0;
    for(i=0;i<lib->bookAuthorCount && !found;i++){
        if (strcmp(lib->bookAuthors[i].isbn, isbn)==0 && lib->bookAuthors[i].authorId==oldId){
            lib->bookAuthors[i].authorId=newId;
            found=1;
        }
    }
    if(!found){
        printf("Mapping not found! \n");
        return;
    }
    saveBookAuthors(lib);
    printf(">> Author updated. \n");
}

void bookMenu(LibraryData *lib){
    int choice;
    EntityFunc bookCrud[3];
    bookCrud[0]=addBook;
    bookCrud[1]=deleteBook;
    bookCrud[2]=updateBook;
    do {
        printf("\n=== BOOK OPERATIONS === \n");
        printf("1. Add Book \n");
        printf("2. Delete Book \n");
        printf("3. Update Book \n");
        printf("4. Display Book Info \n");
        printf("5. List Books on Shelf \n");
        printf("6. List Overdue Books \n");
        printf("7. Match Book-Author \n");
        printf("8. Update Book's Author \n");
        printf("0. Main Menu \n");
        printf("Your choice: \n");
        scanf("%d", &choice);
        clearBuffer();
        switch(choice){
            case 1:
                bookCrud[0](lib);
                break;
            case 2:
                bookCrud[1](lib);
                break;
            case 3:
                bookCrud[2](lib);
                break;
            case 4:
                displayBookInfo(lib);
                break;
            case 5:
                listBooksOnShelf(lib);
                break;
            case 6:
                listOverdueBooks(lib);
                break;
            case 7:
                matchBookAuthor(lib);
                break;
            case 8:
                updateBookAuthorMapping(lib);
                break;
            case 0:
                break;
            default:
                printf("Invalid choice!\n");
        }
    }while(choice!=0);
}

void saveBorrowRecords(LibraryData *lib){
    FILE *fp;
    int i;
    fp=fopen(BORROW_FILE, "w");
    if(fp==NULL){
        printf("ERROR: %s file could not be opened. \n", BORROW_FILE);
        return;
    }
    for(i=0;i < lib->borrowCount;i++){
        fprintf(fp, "%s,%d,%d,%s \n", lib->borrowRecords[i].tagNo, lib->borrowRecords[i].studentId, lib->borrowRecords[i].transType, lib->borrowRecords[i].date);
    }
    fclose(fp);
}

void loadBorrowRecords(LibraryData *lib){
    FILE *fp;
    char line[200];
    char *tok;
    BorrowRecord rec;
    fp=fopen(BORROW_FILE, "r");
    if(fp==NULL){
        return;
    }
    while(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]='\0';
        tok=strtok(line, ",");
        if(tok!=NULL) {
            strncpy(rec.tagNo, tok, MAX_TAG-1);
            rec.tagNo[MAX_TAG-1]='\0';
            tok=strtok(NULL, ",");
            if(tok!=NULL){
                rec.studentId=atoi(tok);
                tok=strtok(NULL, ",");
                if(tok!=NULL){
                    rec.transType=atoi(tok);
                    tok=strtok(NULL, ",");
                    if(tok!=NULL){
                        strncpy(rec.date, tok, MAX_DATE-1);
                        rec.date[MAX_DATE-1]='\0';
                        if(lib->borrowCount >= lib->borrowCapacity){
                            lib->borrowCapacity*=2;
                            lib->borrowRecords=(BorrowRecord *)realloc(lib->borrowRecords, lib->borrowCapacity*sizeof(BorrowRecord));
                        }
                        lib->borrowRecords[lib->borrowCount]=rec;
                        lib->borrowCount++;
                    }
                }
            }
        }
    }
    fclose(fp);
}

void loadAllData(LibraryData *lib){
    loadAuthors(lib);
    loadStudents(lib);
    loadBooks(lib);
    loadCopies(lib);
    loadBookAuthors(lib);
    loadBorrowRecords(lib);
    printf("Data loaded from files. \n");
}

void mainMenu(LibraryData *lib){
    int choice;
    do{
        printf("\n======================================== \n");
        printf("    LIBRARY AUTOMATION SYSTEM \n");
        printf("======================================== \n");
        printf("1. Student Operations \n");
        printf("2. Book Operations \n");
        printf("3. Author Operations \n");
        printf("0. Exit \n");
        printf("Your choice: ");
        scanf("%d", &choice);
        clearBuffer();
        switch(choice){
            case 1:
                studentMenu(lib);
                break;
            case 2:
                bookMenu(lib);
                break;
            case 3:
                authorMenu(lib);
                break;
            case 0:
                printf("Exiting system. Goodbye! \n");
                break;
            default:
                printf("Invalid choice! \n");
        }
    }while(choice!=0);
}

int main(void){
    LibraryData lib;
    initLibrary(&lib);
    loadAllData(&lib);
    mainMenu(&lib);
    freeLibrary(&lib);
    return 0;
}
