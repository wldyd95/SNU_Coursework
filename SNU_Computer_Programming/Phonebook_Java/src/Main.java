import java.util.ArrayList;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        // Initialization of state
        ArrayList<Person> phoneBook = new ArrayList<>();
        String input;
        Scanner scan = new Scanner(System.in);

        // if state is Exit state, we stop the program
        while(true){
            //<Initial state>
            System.out.print("CP-2013-10958>");
            input = scan.nextLine();

            if(input.length() == 0){ // length 0 means input is enter(\n)
                System.out.println("PhoneBook");
                System.out.println("1.Add person");
                System.out.println("2.Remove person");
                System.out.println("3.Print phone book");
            }

            // <Add state>
            else if(input.equals("1")){
                boolean inputMatch = false; // all input matches regex condition?

                while(inputMatch != true){ // if input does not match to regex, restart add state
                    System.out.println("Select Type");
                    System.out.println("1.Person");
                    System.out.println("2.Work");
                    System.out.println("3.Family");
                    System.out.println("4.Friend");
                    System.out.print("CP-2013-10958>");
                    String input_add;
                    input_add = scan.nextLine();

                    String name = "";
                    String firstName;
                    String lastName;
                    String phoneNumber = "";

                    String team; // work
                    String birthday = ""; // family
                    int age; // friend

                    // regex match for input
                    String inputRegex = "^[1234]$";
                    if(!input_add.matches(inputRegex)) continue; // return to add state
                    inputMatch = true;

                    // regex match for name
                    boolean nameMatch = false;
                    while(nameMatch != true) {
                        System.out.print("Name:");
                        name = scan.nextLine();
                        String nameRegex = "^[a-zA-Z]+\\s[a-zA-Z]+$";
                        nameMatch = name.matches(nameRegex);
                        if (!nameMatch) continue; // get input again
                    }

                    // tokenize by using " "
                    String[] name_seprate = name.split("\\s");
                    firstName = name_seprate[0];
                    lastName = name_seprate[1];
                    // finish split

                    // regex match for phoneNumber
                    boolean phoneNumberMatch = false;
                    while(phoneNumberMatch != true){
                        System.out.print("Phone_number:");
                        phoneNumber = scan.nextLine();
                        String phoneNumberRegex = "^(010|02)(-\\d{4})(-\\d{4})$";
                        phoneNumberMatch = phoneNumber.matches(phoneNumberRegex);
                        if(!phoneNumberMatch) continue; // get input again
                    }


                    if(input_add.equals("1")){ // Person add
                        Person p = new Person(firstName, lastName, phoneNumber);
                        add(phoneBook, p);
                    }

                    else if(input_add.equals("2")){ // Work add
                        System.out.print("Team:");
                        // no regex for team
                        team = scan.nextLine();

                        Work w = new Work(firstName, lastName, phoneNumber, team);
                        add(phoneBook, w);
                    }

                    else if(input_add.equals("3")){ // Family add
                        // regex match for birthday
                        boolean birthdayMatch = false;
                        while(birthdayMatch != true){
                            System.out.print("Birthday:");
                            birthday = scan.nextLine();
                            String birthdayRegex = "^\\d{2}(0[1-9]||1[012])(0[1-9]||1[0-9]||2[0-9]||3[0-1])$";
                            birthdayMatch = birthday.matches(birthdayRegex);
                            if(!birthdayMatch) continue; // get input again
                        }

                        Family f = new Family(firstName, lastName, phoneNumber, birthday);
                        add(phoneBook, f);
                    }

                    else if(input_add.equals("4")) { // Friend add
                        String age_str="";
                        // regex match for age
                        boolean ageMatch = false;
                        while(ageMatch != true){
                            System.out.print("Age:");
                            age_str = scan.nextLine();
                            String ageRegex = "^\\d+$";
                            ageMatch = age_str.matches(ageRegex);
                            if (!ageMatch) continue; // return to add state
                        }

                        age = Integer.parseInt(age_str);
                        Friend f = new Friend(firstName, lastName, phoneNumber, age);
                        add(phoneBook, f);
                    }
                }
                // all input matches regex
                System.out.println("Successfully added new person.");
                continue; // return to initial state
            }

            // <Remove state>
            else if(input.equals("2")){
                boolean match = false;
                while(match != true){
                    System.out.print("Enter the index of person:");
                    // index regex match
                    String index_str;
                    index_str = scan.nextLine();
                    String indexRegex = "^[1-9]\\d*$";
                    if(!index_str.matches(indexRegex)) continue;

                    int index = Integer.parseInt(index_str);
                    remove(phoneBook, index);
                    match = true;
                }
                continue; // return to initial state
            }

            // <Print state>
            else if(input.equals("3")){
                print(phoneBook);
            }

            // <Exit state>
            else if(input.equals("exit")){
                return; // stop program
            }

            // strange input
            else{
                continue; // return to initial state
            }
        }
    }

    public static void add(ArrayList<Person> phoneBook, Person p){
        phoneBook.add(p);
    }

    public static void remove(ArrayList<Person> phoneBook, int index){
        int l = phoneBook.size();

        if(l == 0) System.out.println("Person does not exist!");
        // empty phoneBook

        else{ // non-empty phoneBook
            if(index < 1 || index > l) System.out.println("Person does not exist!");
            else{
                phoneBook.remove(index-1);
                System.out.println("A person is successfully deleted from the Phone Book!");
            }
        }
    }

    public static void print(ArrayList<Person> phoneBook){
        System.out.println("Phone Book Print");

        int l = phoneBook.size();
        int i;
        for(i = 1; i <= l; i++){
            System.out.print(i + ".");
            Person p = phoneBook.get(i-1);
            p.print();
            System.out.println();
        }
    }
}
