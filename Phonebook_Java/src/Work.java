public class Work extends Person{
    public Work(String firstName, String lastName, String phoneNumber, String team){
        super(firstName, lastName, phoneNumber);
        this.team = team;
    }

    public void setTeam(String team){
        this.team = team;
    }

    public String getTeam(){
        return team;
    }

    @Override
    public void print(){
        super.print();
        System.out.print("_" + team);
    }

    private String team;
}
