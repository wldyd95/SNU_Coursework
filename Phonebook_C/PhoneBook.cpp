// CP homework #1
// Physics & Astronomy 2013-10958 Jiyong Yu

#include "Person.h"
#include <vector>
#include <cstring>
#include <regex>

// use pointer of phoneBook to change phoneBook variable
void add(vector<Person*>* phoneBookPoint, Person* p);
void remove(vector<Person*>* phoneBookPoint, int index);
void print(vector<Person*> phoneBook);
char** split(string s);

int main(){
    // Initialization of state
    vector<Person*> phoneBook;
    vector<Person*>* phoneBookPoint = NULL;
    phoneBookPoint = &phoneBook;
    string input;

    // if state is Exit state, we stop the program
    while(true){
        // <Initial state>
        cout << "CP-2013-10958>";
        getline(cin,input); // read input including blank 
        
        if(input.length() == 0){ // length 0 means input is enter(\n)
            cout << "PhoneBook" << endl;
            cout << "1.Add person" << endl;
            cout << "2.Remove person" << endl;
            cout << "3.Print phone book" << endl;
        }

        // <Add state>   
        else if(input == "1") { 
            bool inputMatch = 0; // all input matches regex condition?

            while(inputMatch != 1){ // if input does not match to regex, restart add state                
                cout << "Select Type" << endl;
                cout << "1.Person" << endl;
                cout << "2.Work" << endl;
                cout << "3.Family" << endl;
                cout << "4.Friend" << endl;
                cout << "CP-2013-10958>";
                string input_add;
                getline(cin, input_add);
                
                string name;
                string firstName;
                string lastName;
                string phoneNumber;

                string team; // work
                string birthday; // family
                int age; // friend

                // regex match for input
                regex inputRegex("^[1234]$");
                if(regex_match(input_add, inputRegex) == 0) continue; // return to add state
                inputMatch = 1;

                // regex match for name
                bool nameMatch = 0;
                while(nameMatch != 1){
                    cout << "Name:";
                    getline(cin, name);
                    regex nameRegex("^[a-zA-Z]+\\s[a-zA-Z]+$");
                    nameMatch = regex_match(name, nameRegex);
                    if(nameMatch == 0) continue; // get input again
                }

                // tokenize name by using " "
                char** name_separate = new char*[2];
                name_separate = split(name);
                firstName = name_separate[0];
                lastName = name_separate[1];
                // finish split

                // regex match for phone number
                bool phoneNumberMatch = 0;
                while(phoneNumberMatch != 1){
                    cout << "Phone_number:";
                    getline(cin, phoneNumber);
                    regex phoneNumberRegex("^(010|02)(-\\d{4})(-\\d{4})$");
                    phoneNumberMatch = regex_match(phoneNumber, phoneNumberRegex);
                    if(phoneNumberMatch == 0) continue; // get input again
                }
                
                if(input_add == "1"){ // Person add 
                    Person* ptr = new Person(firstName, lastName, phoneNumber);
                    add(phoneBookPoint, ptr);
                }   

                else if(input_add == "2"){ // Work add
                    // no regex match for team
                    cout << "Team:";
                    getline(cin, team);
                    
                    Person* ptr = new Work(firstName, lastName, phoneNumber, team); 
                    add(phoneBookPoint, ptr);
                }

                else if(input_add == "3"){ // Family add
                    // regex match for birthday
                    bool birthdayMatch = 0;
                    while(birthdayMatch != 1){
                        cout << "Birthday:";
                        getline(cin, birthday);
                        regex birthDayRegex("^\\d{2}(0[1-9]||1[012])(0[1-9]||1[0-9]||2[0-9]||3[0-1])$");
                        birthdayMatch = regex_match(birthday, birthDayRegex);
                        if(birthdayMatch == 0) continue; // get input again
                    }

                    Person* ptr = new Family(firstName, lastName, phoneNumber, birthday);
                    add(phoneBookPoint, ptr);
                }

                else if(input_add == "4"){ // Friend add
                    string age_str;
                    // regex match for age
                    bool ageMatch = 0;
                    while(ageMatch != 1){
                        cout << "Age:";
                        getline(cin, age_str);
                        regex ageRegex("^\\d+$");
                        ageMatch = regex_match(age_str, ageRegex);
                        if(ageMatch == 0) continue; // get input again
                    }

                    age = atoi(age_str.c_str());
                    Person*ptr = new Friend(firstName, lastName, phoneNumber, age);
                    add(phoneBookPoint, ptr);
                }

            }
            // all input matches regex
            cout << "Successfully added new person." << endl;
            continue; // return to initial state
        }

        // <Remove state>
        else if(input == "2") {
            bool match = 0;
            while(match != 1){
                cout << "Enter the index of person:";
                // index regex match
                string index_str;
                getline(cin, index_str);
                regex indexRegex("^[1-9]\\d*$");
                if(regex_match(index_str, indexRegex) == 0) continue;

                int index = atoi(index_str.c_str());
                remove(phoneBookPoint, index);
                match = 1;
            }
            continue; // return to initial state
        }

        // <Print state>
        else if(input == "3") {
            print(phoneBook);
        }

        // <Exit state>
        else if(input == "exit"){
            return 0; // stop program
        }

        // strange input
        else{ 
            continue; // return to initial state
        }
    }
}

void add(vector<Person*>* phoneBookPoint, Person* p){
    (*phoneBookPoint).push_back(p);
}

void remove(vector<Person*>* phoneBookPoint, int index){
    int l = (*phoneBookPoint).size();

    if(l == 0) cout << "Person does not exist!" << endl;
    // empty phoneBook

    else{ // non-empty phoneBook
        if(index < 1 || index > l) cout << "Person does not exist!" << endl;
        else {
            (*phoneBookPoint).erase((*phoneBookPoint).begin() + (index-1) );
            cout << "A person is sucessfully deleted from the Phone Book!" << endl;
        } 
    }
}

void print(vector<Person*> phoneBook){
    cout << "Phone Book Print" << endl;

    int l = (phoneBook).size();
    int i;
    for(i = 1; i <= l; i++){
        cout << i << ".";
        Person* ptr = ((phoneBook)[i-1]);
        ptr -> print();
        cout << endl;
    }
}

// separate name to firstName & lastName by using strtok
char** split(string s){
    char** separate = new char*[2];

    char* str = new char[s.size() +1];
    strcpy(str, s.c_str());
    char* token = NULL;
    token = strtok(str, " ");

    int i = 0;
    while(token != NULL){
        separate[i] = token;
        i++;
        token = strtok(NULL," ");
    }

    return separate;
}
