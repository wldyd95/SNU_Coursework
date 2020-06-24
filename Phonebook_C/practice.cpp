#include "Person.h"
#include <vector>
#include <cstring>
#include <regex>
#include <ctime>

int main(){
    // calculate current month and day
    time_t curr_time;
    struct tm *curr_tm;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);

    int month_cur = (curr_tm -> tm_mon) + 1;
    int day_cur = curr_tm -> tm_mday;

    cout << month_cur << endl;
    cout << day_cur << endl;

    return 0;
}
