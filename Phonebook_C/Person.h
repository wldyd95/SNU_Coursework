// use header guard
#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>
#include <ctime>
using namespace std;

class Person
{
    public:
        Person(string firstName, string lastName, string phoneNumber){
            this->firstName = firstName;
            this->lastName = lastName;  
            this->phoneNumber = phoneNumber;
        }

        void setFirstName(string firstName){
            this->firstName = firstName;
        }

        string getFirstName(){
            return firstName;
        }

        void setLastName(string lastName){
            this->lastName = lastName;
        }

        string getLastName(){
            return lastName;
        }

        void setPhoneNumber(string phoneNumber){
            this->phoneNumber = phoneNumber;
        }
        // 02-XXXX-XXXX or 010-XXXX-XXXX

        virtual void print(){ // use virtual function for overriding
            cout << firstName << " " << lastName << "_" << phoneNumber;
        };


    private:
        string firstName;
        string lastName;
        string phoneNumber;
};

class Work : public Person{
    public:
        Work(string firstName, string lastName, string phoneNumber, string team):Person(firstName, lastName, phoneNumber){
            this->team = team;
        }

        void setTeam(string team){
            this->team = team;
        }

        string getTeam(){
            return team;
        }

        virtual void print(){
            Person::print();
            cout << "_" << team;
        }

    private:
        string team;
};


class Friend : public Person{
    public:
        Friend(string firstName, string lastName, string phoneNumber, int age):Person(firstName, lastName, phoneNumber){
            this->age = age;
        }

        void setAge(int age){
            this->age = age;
        }

        int getAge(){
            return age;
        }

        virtual void print(){
            Person::print();
            cout << "_" << age;
        }

    private:
        int age; // age of friend
};

class Family : public Person{
    public:
        Family(string firstName, string lastName, string phoneNumber, string birthday):Person(firstName, lastName, phoneNumber){
            this->birthday = birthday;
        }

        void setBirthday(string birthday){
            this->birthday = birthday;
        }

        string getBirthday(){
            return birthday;
        }

        int dDay(){
            int dayEachMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            // number of day for each 12 month
            int dayAccum[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
            // accumulated day for each 12 month

            // calculate current month and day
            time_t curr_time;
            struct tm *curr_tm;
            curr_time = time(NULL);
            curr_tm = localtime(&curr_time);

            int month_cur = (curr_tm -> tm_mon) + 1;
            int day_cur = curr_tm -> tm_mday;

            // calculate birthday's month and day
            string month_str = birthday.substr(2,2);
            string day_str = birthday.substr(4,2);

            int month_birth = atoi(month_str.c_str());
            int day_birth = atoi(day_str.c_str());

            // compare current time and birthday
            int cur = dayAccum[month_cur-1] + day_cur;
            int birth = dayAccum[month_birth - 1] + day_birth;

            if(cur <= birth){ // birthday is yet today
                return (birth - cur);
            } 

            else{ // birthday is past today
                return 365-(cur-birth);
            }
        }

        virtual void print(){
            Person::print();
            cout << "_" << birthday << "_" << dDay();
        }

    private:
        string birthday;

};


#endif