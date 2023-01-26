public class Friend extends Person{
    public Friend(String firstName, String lastName, String phoneNumber, int age){
        super(firstName, lastName, phoneNumber);
        this.age = age;
    }

    public void setAge(int age){
        this.age = age;
    }

    public int getAge(){
        return age;
    }

    @Override
    public void print(){
        super.print();
        System.out.print("_" + age);
    }

    private int age;
}
