//Shaun McCormick
//CSCI230
//This program is a simple library that takes four files, a commands file, books file, readers file, and a master file. This program also executes simple commands.

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;
const int MAX_READERS = 50;
const int MAX_BOOKS = 1000;

enum CommandT{PRINT = 1, CHECKOUT, REPORT, STATS, RETURN, UNKNOWN};

struct ReaderT{ //represents a reader
    string name;
    int pagesRead = 0;
    int booksRead = 0;
    bool hasBook = false;
    size_t book;
};
struct BookT{ //represents a book
    string name;
    string author;
    int pages;
    string currentOwner;
    size_t borrower;
    int timesRead = 0;
    bool present = true;
};

void OpenConfigFile(ifstream & configFile);
bool OpenDataFiles(ifstream & configFile, ifstream & booksFile, ifstream & readersFile, ifstream & commandsFile);
void ReadPeople(ifstream & readerFile, ReaderT readers[], size_t & readersSize);
void ReadLibrary(ifstream & booksFile, BookT books[], size_t & booksSize);
void ProcessCommands(ifstream & commandsFile, ReaderT readers[], BookT books[], size_t booksSize, size_t readersSize);
string SplitCommand(string &line);
size_t FindBook(string name, BookT books[], size_t booksSize);
size_t FindReader(string name, ReaderT readers[], size_t readersSize);
void PrintBooks(BookT books[], size_t booksSize);
void PrintPeople(ReaderT readers[], size_t readersSize);
CommandT CommandToCommandT(string command);
void DoCheckout(string line, BookT books[], ReaderT readers[], size_t booksSize, size_t readersSize);
void DoPrint(string line, BookT book[], ReaderT readers[], size_t readersSize, size_t booksSize);
void DoReport(string line, ReaderT readers[], BookT books[], size_t readersSize);
void DoStats(string line, ReaderT readers[], BookT books[], size_t booksSize);
void DoReturn(string book, BookT books[], ReaderT readers[], size_t booksSize);
void PrintBook(BookT books[], size_t pos);
void PrintPerson(ReaderT readers[], size_t pos);
void SplitBook(string line, string &book, string &author, int &pages);
void SeptCheckout(string line, string &title, string &name);

int main(){
    ifstream configFile, booksFile, readersFile, commandsFile;
    BookT books[MAX_BOOKS];
    ReaderT readers[MAX_READERS];
    size_t booksSize = 0, readersSize = 0;
    OpenConfigFile(configFile); //opens config file
    if(OpenDataFiles(configFile, booksFile, readersFile, commandsFile)){//opens reader, book and commands files
        ReadLibrary(booksFile, books, booksSize); //puts readers from file into array
        ReadPeople(readersFile, readers, readersSize); //puts books from file into array
        ProcessCommands(commandsFile, readers, books, readersSize, booksSize); //executes commands
    } 
    
    return 0; 
}
//Opens file, returns bool based if file opens or fails to open
void OpenConfigFile(ifstream & configFile){
    string name;

    do {
    cout << "Enter the name of the configuration file => ";
    cin >> name;
    cout << endl;
    cout << "Opening " << name  << "." << endl; 
    configFile.open(name.c_str());
    } while (not configFile); 

    return;
}
//Opens all of the needed data files, i.e reader file, book file and commands file
bool OpenDataFiles(ifstream & configFile, ifstream & booksFile, ifstream & readersFile, ifstream & commandsFile){
    string fileName;
    bool truthValue = true;

    getline(configFile, fileName);
    booksFile.open(fileName.c_str());
    if(!booksFile){
        cout << "Could not open books file." << endl;
        truthValue = false;
    }
    else{
        cout << "Reading books from " << fileName << "." << endl;
    }

    getline(configFile, fileName);
    readersFile.open(fileName.c_str());
    if(!readersFile){
        cout << "Could not open readers file." << endl;
        truthValue = false;
    }
    else{
        cout << "Reading people from " << fileName << "." << endl;
    }
    getline(configFile, fileName);
    commandsFile.open(fileName.c_str());
    if(!commandsFile){
        cout << "Could not open commands file." << endl;
        truthValue = false;
    }

    return truthValue;
}
//reads from books file and stores in books array
void ReadLibrary(ifstream & booksFile, BookT books[], size_t & booksSize){
    string temp, book, author;
    int pages;
    getline(booksFile, temp);

    while(booksFile){
        SplitBook(temp, book, author, pages);
        books[booksSize].name = book;
        books[booksSize].author = author;
        books[booksSize].pages = pages;
        booksSize++;
        if(booksSize >= MAX_BOOKS){
            cout << "Error: Can not add title to the library. It is full." << endl;
            return;
        }
        getline(booksFile, temp);
    }

    return;
}
//reads from readers file and stores in readers array
void ReadPeople(ifstream & readersFile, ReaderT readers[], size_t & readersSize){
    string temp;
    int beyondSize = 0;
    getline(readersFile, temp);
    while(readersFile){
        if(readersSize >= MAX_READERS){
            beyondSize++;
            cout << "Error: reader list is full.  Unable to add name " << readersSize + beyondSize << "." << endl;
        }
        else{
            readers[readersSize].name = temp;
            readersSize++;
            getline(readersFile, temp);
        }
    }

    return;
}
//reads from command file and executes commands by calling appropriate functions
void ProcessCommands(ifstream & commandsFile, ReaderT readers[], BookT books[], size_t readersSize, size_t booksSize){
    string line, command;
    CommandT newCom;
    
    getline(commandsFile, line);
    while(commandsFile){
        command = SplitCommand(line);
        newCom = CommandToCommandT(command);
        cout << "Executing " << command; 
        if (line != command){
            cout << " " << line << "." << endl;
        }
        else{
            cout << "." << endl;
        }
        switch(newCom){
            case PRINT: 
                DoPrint(line, books, readers, readersSize, booksSize);
                break;
            case CHECKOUT:
                DoCheckout(line, books, readers, booksSize, readersSize);
                break;
            case REPORT:
                DoReport(line, readers, books, readersSize);
                break;
            case STATS: 
                DoStats(line, readers, books, booksSize);
                break;
            case RETURN: 
                DoReturn(line, books, readers, booksSize);
                break;
            case UNKNOWN:
                cout << "Error: unknown action " << command << "." << endl;
                break;
        }
        cout << endl;
        getline(commandsFile, line);
    }

    return;
}
//turns command into CommandT(an enum)
CommandT CommandToCommandT(string command){
    CommandT newCom = PRINT;

    if(command == "PRINT"){
        newCom = PRINT;
    }
    else if(command == "CHECKOUT"){
        newCom = CHECKOUT;
    }
    else if(command == "STATS"){
        newCom = STATS;
    }
    else if(command == "REPORT"){
        newCom = REPORT;
    }
    else if(command == "RETURN"){
        newCom = RETURN;
    }
    else{
        newCom = UNKNOWN;
    }

    return newCom;
}
//Splits the command in half, basically takes the first command and removes it from line and returns the command.
string SplitCommand(string &line){
    size_t i; 
    string command = "";
    for(i=0; i<line.size(); i++){
        if(isspace(line[i])){
            i++;
            line = line.substr(i, line.size());
            i = line.size();
        }
        else{
            command += line[i];
        }
    }

    return command;
}
//splits line into three parts, book author and pages. converts pages from string to int.
void SplitBook(string line, string &book, string &author, int &pages){
    size_t i, j=0, k=0;
    bool truthValue = false;
    string temp;

    for(i=0; i<line.size(); i++){
        if(ispunct(line[i]) && truthValue){
            k=i-j;
            author = line.substr(j,k);
            k=i+1;
            temp = line.substr(k,line.size());            
            pages = atoi(temp.c_str()); //c-string since atoi wont take a regular c++ string, also i cant use stoi for some reason
            i=line.size();
            truthValue = false;
        }
        if(ispunct(line[i]) && !truthValue){
            book = line.substr(0,i);
            j=i+1; //to skip the punctuation
            truthValue = true;
        }
    }

    return;
}
//High level function for return command, calls lower level functions and returns book if no errors
void DoReturn(string book, BookT books[], ReaderT readers[], size_t booksSize){
    size_t bookPos = FindBook(book, books, booksSize);
    size_t readerPos = 0;
    if(bookPos == MAX_BOOKS){
        cout << "Error: Book does not exist." << endl;
    }
    else{
        if(books[bookPos].present){
            cout << "Error: Book is already checked in." << endl;
        }
        else{
            readerPos = books[bookPos].borrower; //gets readers position from bookT
            readers[readerPos].hasBook = false; //changes so reader does not have book
            books[bookPos].present = true; //changes so book is present
            readers[readerPos].pagesRead += books[bookPos].pages; //increments pages read for reader
            readers[readerPos].booksRead++; //increments books read for reader
            books[bookPos].currentOwner = ""; //changes current owner
            books[bookPos].timesRead++; //increments times read for book
            cout << readers[readerPos].name << " has returned " << book << "." << endl;
        }
    }

    return;
}
//High level function for checkout command, calls lower level functions and checks out book if no errors occur
void DoCheckout(string line, BookT books[], ReaderT readers[], size_t booksSize, size_t readersSize){
    string title, name;
    size_t bookPos = MAX_BOOKS, readerPos = MAX_READERS;
    SeptCheckout(line, title, name);
    readerPos = FindReader(name, readers, readersSize);
    if(readerPos != MAX_READERS){ //IF READER EXISTS
        bookPos = FindBook(title, books, booksSize);
    }
    else{ //IF READER DOES NOT EXIST
        cout << "Error: Reader not found." << endl;
    }
    if(bookPos != MAX_BOOKS && readerPos != MAX_READERS){ //IF BOOK EXISTS AND READER EXISTS
        if(readers[readerPos].hasBook == true){
            cout << "Error: Reader already has a book." << endl;
        }
        else if(books[bookPos].present == false){
            cout << "Error: " << books[bookPos].name << " is already checked out." << endl;
        }
        else{
            cout << "Loaning " << title << " to " << name << "." << endl;
            readers[readerPos].hasBook = true;
            readers[readerPos].book = bookPos;
            books[bookPos].currentOwner = readers[readerPos].name;
            books[bookPos].borrower = readerPos;
            books[bookPos].present = false;
        }
    }
    else if(bookPos == MAX_BOOKS){ //IF BOOK DOES NOT EXISTS 
        cout << "Error: Book not found." << endl;
    }

    return;
}
//high level function for print command, prints all books or readers depending on command given
void DoPrint(string line, BookT books[], ReaderT readers[], size_t readersSize, size_t booksSize){
    cout << "There are ";
    if(line == "BOOKS"){
        cout << booksSize << " books in the library." << endl;
        cout << "They are:" << endl;
        PrintBooks(books, booksSize);
    }
    else{
        cout << readersSize << " people using the library." << endl;
        cout << "They are:" << endl;
        PrintPeople(readers, readersSize);
    }

    return; 
}
//high level function for report command, makes a report of a given person, outputting stats for person and current book if there is one. 
void DoReport(string line, ReaderT readers[], BookT books[], size_t readersSize){
    size_t pos = FindReader(line, readers, readersSize);
    size_t bookPos;

    if (pos != MAX_READERS){
        PrintPerson(readers, pos);
        if(readers[pos].hasBook){
            bookPos = readers[pos].book;
            cout << "\tThe current book is: " << books[bookPos].name << "." << endl;
        }
    }
    else{
        cout << "Error: unknown reader " << line << "." << endl;
    }

    return;
}
//high level function for stats command, outputs stats for a given book
void DoStats(string line, ReaderT readers[], BookT books[], size_t booksSize){
    size_t pos = FindBook(line, books, booksSize);
    size_t readerPos;
    if(pos != MAX_BOOKS){
        PrintBook(books, pos);
        if(!books[pos].present){
            readerPos = books[pos].borrower;
            cout << "\tOn loan to: " << readers[readerPos].name << "." << endl;
        }
    }
    else{
        cout << "Error: unknown book " << line << "." << endl;
    }

    return;
}
//function for checkout command that separates the title and name of reader from line.
void SeptCheckout(string line, string &title, string &name){
    size_t i;

    for(i=0; i<line.size(); i++){
        if(ispunct(line[i])){
            title = line.substr(0, i);
            i++;
            name = line.substr(i, line.size());
        }
    }

    return;
}
//finds reader in array and returns readers position in array, if it does not find reader returns MAX_READERS
size_t FindReader(string name, ReaderT readers[], size_t readersSize){
    size_t i, pos = MAX_READERS;
    
    for(i=0; i<readersSize; i++){
        if(name == readers[i].name){
            pos = i;
            i = readersSize;
        }
    }

    return pos;
}
//finds book in array and returns position of book in array if it is in book, if not returns MAX_BOOKS
size_t FindBook(string name, BookT books[], size_t booksSize){
    size_t i, pos = MAX_BOOKS;

    for(i=0; i<booksSize; i++){
        if(name == books[i].name){
            pos = i;
            i = booksSize;
        }
    }

    return pos;
}
//prints all books in array
void PrintBooks(BookT books[], size_t booksSize){
    size_t i;
    
    for(i=0; i<booksSize; i++){
        cout << books[i].name << endl << "\tby " << books[i].author << "." << endl << "\tPages: " << books[i].pages << "." << endl << "\tTotal Reads: " << books[i].timesRead << "." << endl;
    }

    return;
}
//prints all readers in array
void PrintPeople(ReaderT readers[], size_t readersSize){
    size_t i;

    for(i=0; i<readersSize; i++){
        cout << readers[i].name << endl; 
        cout << "\tBooks read: " << readers[i].booksRead << "." << endl; 
        cout << "\tPages read: " << readers[i].pagesRead << "." << endl;
    }

    return;
}
//prints a specific book in array, at position pos
void PrintBook(BookT books[], size_t pos){
    cout << books[pos].name << endl; 
    cout << "\tby " << books[pos].author << "." << endl; 
    cout << "\tPages: " << books[pos].pages << "." << endl; 
    cout << "\tTotal Reads: " << books[pos].timesRead << "." << endl;

    return;
}
//prints a specific reader in array, at position pos
void PrintPerson(ReaderT readers[], size_t pos){
    cout << readers[pos].name << endl; 
    cout << "\tBooks read: " << readers[pos].booksRead << "." << endl; 
    cout << "\tPages read: " << readers[pos].pagesRead << "." << endl;
    
    return;
}