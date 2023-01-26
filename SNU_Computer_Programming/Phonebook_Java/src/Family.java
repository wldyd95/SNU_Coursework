import java.util.Date;

public class Family extends Person{
    public Family(String firstName, String lastName, String phoneNumber, String birthday){
        super(firstName, lastName, phoneNumber);
        this.birthday = birthday;
    }

    public void setBirthday(String birthday){
        this.birthday = birthday;
    }

    public String getBirthday(){
        return birthday;
    }

    public int dDay(){
        int dayEachMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        // number of day for each 12 month
        int dayAccum[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        // accumulated day for each 12 month

        // calculate current month and day
        String inDate = new java.text.SimpleDateFormat("yyyyMMdd").format(new java.util.Date());
        int month_cur = Integer.parseInt(inDate.substring(4,6));
        int day_cur = Integer.parseInt(inDate.substring(6,8));

        // calculate birthday's month and day
        int month_birth = Integer.parseInt(birthday.substring(2,4));
        int day_birth = Integer.parseInt(birthday.substring(4,6));

        // compare current time and birthday
        int cur = dayAccum[month_cur - 1] + day_cur;
        int birth = dayAccum[month_birth - 1] + day_birth;

        if(cur <= birth){ // birthday is yet today
            return (birth - cur);
        }

        else{ // birthday is past today
            return 365-(cur-birth);
        }
    }

    @Override
    public void print(){
        super.print();
        System.out.print("_"+ birthday + "_" + dDay());
    }

    private String birthday;
}
