// 2019 Spring Computer Programming
// Project1 done by 2013-10958 Jiyong yu, Dept of Physics & Astronomy, SNU

#include <iostream>
#include <cstring>
#include <vector>
#include <ctime>

using namespace std;

#define energy_max (1000)
#define default_car_speed (80)
#define default_airplnae_speed (900)
#define default_submarine_speed (20)
#define road_oxygen (100)

#define unit_road (50)
#define unit_sky (1000)
#define unit_ocean (10)

class Vehicle{
    public:
        Vehicle(int speed, int energy){
            this -> speed = speed;
            this -> energy = energy;
        }
    
    int getSpeed(){
        return speed;
    }

    void setSpeed(int speed){
        this -> speed = speed;
    }

    int getEnergy(){
        return energy;
    }

    // Energy maximum value is 1000
    void setEnergy(int energy){
       if(energy >= energy_max) this -> energy = energy_max;
       else this -> energy = energy;
    }

    private:
        int speed;
        int energy;
};

class Car : public Vehicle{
    public:
        Car(int speed, int energy, int temparature, int humidity) : Vehicle(speed, energy){
            this -> temparature = temparature;
            this -> humidity = humidity;
        }

        int getTemparature(){
            return temparature;
        }

        void setTemparature(int temparature){
            this -> temparature = temparature;
        }

        int getHumidity(){
            return humidity;
        }
        
        void setHumidity(int humidity){
            this -> humidity = humidity;
        }

        int getOxygen_rate(){ // car oxygen_rate is always 100
            return road_oxygen;
        }

        // increase energy as 200 using solar panel (only operates if humidity < 50)
        void solar_panel_recharge(int *energy){
            if(humidity < 50) {
                *energy += 200;
                if(*energy > energy_max) *energy = energy_max;
                this -> setEnergy(*energy);
            }
        }

    private:
        int temparature;
        int humidity;
};

class Airplane : public Vehicle{
    public:
        Airplane(int speed, int energy, int temparature, int humidity, int altitude, int oxygen_rate, int air_density) 
        : Vehicle(speed, energy){
            this -> temparature = temparature;
            this -> humidity = humidity;
            this -> altitude = altitude;
            this -> oxygen_rate = oxygen_rate;
            this -> air_density = air_density;
        }

        int getTemparature(){
            return temparature;
        }

        void setTemparature(int temparature){
            this -> temparature = temparature;
        }

        int getHumidity(){
            return humidity;
        }
        
        void setHumidity(int humidity){
            this -> humidity = humidity;
        }

        int getAltitude(){
            return altitude;
        }

        void setAltitude(int altitude){
            this -> altitude = altitude;
        }

        int getOxygen_rate(){
            return oxygen_rate;
        }

        void setOxygen_rate(int oxygen_rate){
            this -> oxygen_rate = oxygen_rate;
        }

        int getAir_density(){
            return air_density;
        }

        void setAir_density(int air_density){
            this -> air_density = air_density;
        }

    private:
        int temparature;
        int humidity;
        int altitude;
        int oxygen_rate;
        int air_density;

};

class Submarine : public Vehicle{
    public:
        Submarine(int speed, int energy, int temparature, int depth, int oxygen_rate, int water_flow)
        :Vehicle(speed, energy){
            this -> temparature = temparature;
            this -> depth = depth;
            this -> oxygen_rate = oxygen_rate;
            this -> water_flow = water_flow;
        }

        int getTemparature(){
            return temparature;
        }

        void setTemparature(int temparature){
            this -> temparature = temparature;
        }
        
        int getDepth(){
            return depth;
        }

        void setDepth(int depth){
            this -> depth = depth;
        }

        int getOxygen_rate(){
            return oxygen_rate;
        }

        void setOxygen_rate(int oxygen_rate){
            this -> oxygen_rate = oxygen_rate;
        }

        int getWater_flow(){
            return water_flow;
        }

        void setWater_flow(int water_flow){
            this -> water_flow = water_flow;
        }

        // always light on in submarine mode, reduces energy 30 per 10km
        void light(int *energy){
            *energy -= 30;
            if(*energy < 0) *energy = 0;
            this -> setEnergy(*energy);
        }

    private:
        int temparature;
        int depth;
        int oxygen_rate;
        int water_flow;
};

void energyLoss_temp(int*, int); // energy loss due to temparature
void energyLoss_humidity(int*, int); // energy loss due to humidity
void energyLoss_X(int*); // energy loss due to unexpected event X
void speedLoss_air_density(int*, int); // speed loss due to air density
void speedLoss_water_flow(int*, int); // speed loss due to water flow
void oxygenLoss_altitude(int*, int); // oxygen loss due to altitude
void oxygenLoss_depth(int*, int); // oxygen loss due to depth
void oxygenLoss_Y(int*); // oxygen loss due to unexpected event Y

template <typename V> // V can be car, airplane, submarine
void message_arrive(int, int, V, vector<char>, string); // arrive message print

template <typename V>
void message_energy_fail(int, int, V, vector<char>, string);

template <typename V>
void message_oxygen_fail(int, int, V, vector<char>, string);

void message_initial(Car, vector<char>, string); // initial message print
void message_car(int, int, Car, vector<char>, string); // car move message print
void message_airplane(int, int, Airplane, vector<char>, string); // airplane move message print
void message_submarine(int, int, Submarine, vector<char>, string); // submarine move message print
bool message_X(int, char, int, int*, int, vector<char>, string);// unexpected event X message print
bool message_Y_car(int, char, int, int, int, bool*, vector<char>, string); // unexpected evnet Y(for car) message print
bool message_Y(int, char, int, int, int*, vector<char>, string); // unexpected event Y(for airplane and submarine) message print

void update_blackBox(vector<string>*, vector<int>*, vector<int>*, vector<int>*, string, int, int, int); // update black box
void blackBoxPrint(vector<string>, vector<int>, vector<int>, vector<int>); // black box print function
void vectorPrint(vector<char>); // graphic progress print
void vectorSwap(vector<char>*, int); // swap ith element with (i+1)th element 

int main(){
    // Test case for grading
    string TC1 = "[R100T10H10],[S3000T10H5A2000D30],[O50T0D100W100]";
    string TC2 = "[R1000T20H49]";
    string TC3 = "[R1000T20H49],[S1000T20H49A1000D0],[R150T20H49]";
    string TC4 = "[R5050T10H10]";
    string TC5 = "[R50T10H10],[S4000T10H5A2000D30],[R50T10H10],[O90T0D100W100],[R50T10H10]";
    string TC6 = "[R5050T10H10]";
    string TC7 = "[R500T20H20],[S3000T10H5A2000D30],[O80T0D100W100]";
    string TC8 = "[O100T0D100W100],[R5050T10H10]";
    string TC9 = "";
    string TC10 = "";

    cout << "PJ1.YJY.2013-10958" << endl;

    cout << "Mode Select(1 for EXTRA, 2 for NORMAL) : "; // EXTRA case done, we will use Mode1(EXTRA)
    string mode_extra; // 1 : EXTRA, 2 : NORMAL
    getline(cin, mode_extra);

    // Start Test Case 
    while(true){
        cout << "Choose the number of the test case (1~10) : ";
        string TC_number;
        string TC;
        getline(cin, TC_number); 

        // Test case numbering
        switch (atoi(TC_number.c_str()))
        {
        case 0: // If Test case number is 0, exit program
            return 0;
        case 1:
            TC = TC1;
            cout << "Test case #1." << endl;
            cout << endl;
            break;
        
        case 2:
            TC = TC2;
            cout << "Test case #2." << endl;
            cout << endl;
            break;
        
        case 3:
            TC = TC3;
            cout << "Test case #3." << endl;
            cout << endl;
            break;

        case 4:
            TC = TC4;
            cout << "Test case #4." << endl;
            cout << endl;
            break;

        case 5:
            TC = TC5;
            cout << "Test case #5." << endl;
            cout << endl;
            break;

        case 6:
            TC = TC6;
            cout << "Test case #6." << endl;
            cout << endl;
            break;

        case 7:
            TC = TC7;
            cout << "Test case #7." << endl;
            cout << endl;
            break;

        case 8:
            TC = TC8;
            cout << "Test case #8." << endl;
            cout << endl;
            break;

        case 9:
            TC = TC9;
            cout << "Test case #9." << endl;
            cout << endl;
            break;

        case 10:
            TC = TC10;
            cout << "Test case #10." << endl;
            cout << endl;
            break;
        }

        // Tokenize test case string
        char* pch;
        const char* delimiter_sequence = ",";
        vector<char*> mode_sequence; // total mode seqeunce of Test case
        
        char TC_char[3000]; 
        strcpy(TC_char, TC.c_str());
        pch = strtok(TC_char, delimiter_sequence);
        while(pch != NULL){
            mode_sequence.push_back(pch);
            pch = strtok(NULL, delimiter_sequence);
        }

        // For extra credit, make graphic progress (@ : vehicle, = : road, ^ : sky, ~ : ocean)
        vector<char> graphic_progress;
        graphic_progress.push_back('|');
        graphic_progress.push_back('@');

        // save each mode element after tokenizing test case string
        vector<char*> mode_each[mode_sequence.size()];

        for(int i = 0; i < mode_sequence.size(); i++){
            // Tokenize each mode string
            const char* delimeter = "[]RTHSADOWXY";// whole set of delimeter (road, sky, ocean, unexpected factor)
            pch = strtok(mode_sequence[i], delimeter);

            while(pch != NULL){
                mode_each[i].push_back(pch);
                pch = strtok(NULL,  delimeter);
            }

            char mode_current = *(mode_sequence[i]+1);

            // For extra credit : make whole course for total graphic progress
            // Classify by mode identifier (Road : Car, Sky : Airplane, Ocean : Submarine)
            switch (mode_current)
            {
            case 'R':{
                int distance_remain = atoi(mode_each[i][0]);
                int unit_number = distance_remain/unit_road;

                for(int i = 0; i < unit_number; i++){
                    graphic_progress.push_back('=');
                }
            }
            break;

            case 'S':{
                int distance_remain = atoi(mode_each[i][0]);
                int unit_number = distance_remain/unit_sky;

                for(int i = 0; i < unit_number; i++){
                    graphic_progress.push_back('^');
                }
            }
            break;

            case 'O':{
                int distance_remain = atoi(mode_each[i][0]);
                int unit_number = distance_remain/unit_ocean;

                for(int i = 0; i < unit_number; i++){
                    graphic_progress.push_back('~');
                }
            }
            break;
            }
        } 
        // total graphic progress done
        graphic_progress.push_back('|');
        
        // global variables initialization - car, airplane, submarine share this value
        int distance = 0;
        int energy = energy_max;
        int oxygen_rate = road_oxygen;
        bool arrive = false;

        // Black box : contains mode, energy, oxygen, speed 
        vector<string> blackBox_mode;
        vector<int> blackBox_energy;
        vector<int> blackBox_oxygen;
        vector<int> blackBox_speed;

        // Flag : test case done completelly?
        int TC_done = 0;
        // Flag : solar panel enable (if solar panel destroyed once, it can't be repaired)
        bool solar_panel_enable = true;
        // represents current position of vehicle
        int current_position = 1;

        // If one test case done, exit this loop
        for(int i = 0; i < mode_sequence.size() * !TC_done; i++){
            //Initialization (Car starts on the road with speed 0)
            if(i == 0){
                int speed_init = 0;
                int energy_init = energy_max;
                int temparature_init = atoi(mode_each[i][1]);
                int humidity_init = atoi(mode_each[i][2]);
                Car car(speed_init, energy_init, temparature_init, humidity_init);
                message_initial(car, graphic_progress, mode_extra);

                cout << "Next Move? (1,2)" << endl;
                cout << "CP-2013-10958>";
            }

            string move_mode; // first move mode vs second move mode

            char mode_current = *(mode_sequence[i]+1);
            // Classify by mode identifier (Road : Car, Sky : Airplane, Ocean : Submarine)
            switch (mode_current)
            {
            case 'R': // Road mode
            { 
                int speed = default_car_speed;
                int distance_remain = atoi(mode_each[i][0]);
                int temparature = atoi(mode_each[i][1]);
                int humidity = atoi(mode_each[i][2]);
                oxygen_rate = road_oxygen; // in road , oxygen level is fixed to 100
                Car car(speed, energy, temparature, humidity);
                // solar panel recharge immediately after car mode (if solar_panel_enable is true)
                if(solar_panel_enable) car.solar_panel_recharge(&energy); 

                while(distance_remain){
                    getline(cin, move_mode);
                    if(move_mode == "1"){ // first move mode
                        // if non-zero energy remains, go one unit
                        energyLoss_temp(&energy, temparature);
                        energyLoss_humidity(&energy, humidity);
                        distance += unit_road;
                        distance_remain -= unit_road;
                        car.setEnergy(energy); // update energy of car
                        vectorSwap(&graphic_progress, current_position++);

                        // record black box
                        if(!distance_remain || !energy){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Car", energy, oxygen_rate, speed);
                        }

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(unit_road, distance, car, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy){ // energy failure, car stop
                            message_energy_fail(unit_road, distance, car, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else{
                            message_car(unit_road, distance, car, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y_car(distance_remain, mode_next, distance, energy, oxygen_rate, &solar_panel_enable,graphic_progress, mode_extra);

                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }

                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }
                    }

                    else if(move_mode == "2"){ // second move mode
                        int unit_number = distance_remain/unit_road;
                        int save_remain = distance_remain;
                        for(int i = 0; i < unit_number; i++){ // move units of road step by step
                            if(!energy){ // energy failure, car stop
                                message_energy_fail(i * unit_road, distance, car, graphic_progress, mode_extra);
                                TC_done = 1;
                                break;
                            }

                            energyLoss_temp(&energy, temparature);
                            energyLoss_humidity(&energy, humidity);
                            distance += unit_road;
                            distance_remain -= unit_road;
                            car.setEnergy(energy); // update energy of car
                            vectorSwap(&graphic_progress, current_position++);
                        }

                        // record black box
                        if(!distance_remain || !energy){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Car", energy, oxygen_rate, speed);
                        }

                        if(TC_done) break;

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(save_remain, distance, car, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy){ // energy failure, car stop
                            message_energy_fail(save_remain, distance, car, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else{
                            message_car(save_remain, distance, car, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y_car(distance_remain, mode_next, distance, energy, oxygen_rate, &solar_panel_enable, graphic_progress, mode_extra);
                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }

                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }
                    }
                }
                break;
            }

            case 'S': // Sky mode
            {
                int speed = default_airplnae_speed;
                int distance_remain = atoi(mode_each[i][0]);
                int temparature = atoi(mode_each[i][1]);
                int humidity = atoi(mode_each[i][2]);
                int altitude = atoi(mode_each[i][3]);
                int air_density = atoi(mode_each[i][4]);

                speedLoss_air_density(&speed, air_density);
                Airplane airplane(speed, energy, temparature, humidity, altitude, oxygen_rate, air_density);

                while(distance_remain){
                    getline(cin, move_mode);
                    if(move_mode == "1"){ // first move mode
                        // if non-zero energy and non-zero oxygen remains, go one unit
                        energyLoss_temp(&energy, temparature);
                        energyLoss_humidity(&energy, humidity);
                        oxygenLoss_altitude(&oxygen_rate, altitude);
                        distance += unit_sky;
                        distance_remain -= unit_sky;
                        airplane.setEnergy(energy); // update energy of airplane
                        airplane.setOxygen_rate(oxygen_rate); // update oxygen_rate of airplane
                        vectorSwap(&graphic_progress, current_position++);
                        
                        // record black box
                        if(!distance_remain || !energy || !oxygen_rate){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Airplane", energy, oxygen_rate, speed);
                        }

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(unit_sky, distance, airplane, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy || !oxygen_rate){ // energy failure or oxygen failure, airplane stop 
                            if(!energy) message_energy_fail(unit_sky, distance, airplane, graphic_progress, mode_extra); 
                            // energy failure prevails oxygen failure
                            else message_oxygen_fail(unit_sky, distance, airplane, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }
                                                
                        else{
                            message_airplane(unit_sky, distance, airplane, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y(distance_remain, mode_next, distance, energy, &oxygen_rate, graphic_progress, mode_extra);
                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }
                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }
                    }

                    else if(move_mode == "2"){ // second move mode
                        int unit_number = distance_remain/unit_sky;
                        int save_remain = distance_remain;
                        for(int i = 0; i < unit_number; i++){ // move units of sky step by step
                            if(!energy || !oxygen_rate){ // energy failure or oxygen failure, airplane stop
                                // energy failure prevails oxygen failure
                                if(!energy) message_energy_fail(i * unit_sky, distance, airplane, graphic_progress, mode_extra);
                                else message_oxygen_fail(i * unit_sky, distance, airplane, graphic_progress, mode_extra);
                                TC_done = 1;
                                break;
                            }

                            energyLoss_temp(&energy, temparature);
                            energyLoss_humidity(&energy, humidity);
                            oxygenLoss_altitude(&oxygen_rate, altitude);
                            distance += unit_sky;
                            distance_remain -= unit_sky;
                            airplane.setEnergy(energy); // update energy of airplane
                            airplane.setOxygen_rate(oxygen_rate); // update oxygen_rate of airplane
                            vectorSwap(&graphic_progress, current_position++);
                        }
                        
                        // record black box
                        if(!distance_remain || !energy || !oxygen_rate){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Airplane", energy, oxygen_rate, speed);
                        }

                        if(TC_done) break;

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(save_remain, distance, airplane, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy || !oxygen_rate){ // energy failure or oxygen failure, airplane stop 
                            if(!energy) message_energy_fail(save_remain, distance, airplane, graphic_progress, mode_extra); 
                            // energy failure prevails oxygen failure
                            else message_oxygen_fail(save_remain, distance, airplane, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }
                                                
                        else{
                            message_airplane(save_remain, distance, airplane, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y(distance_remain, mode_next, distance, energy, &oxygen_rate, graphic_progress, mode_extra);
                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }
                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }
                    }
                }
                break;
            }
            
            case 'O': // Ocean mode
            {
                int speed = default_submarine_speed;
                int distance_remain = atoi(mode_each[i][0]);
                int temparature = atoi(mode_each[i][1]);
                int depth = atoi(mode_each[i][2]);
                int water_flow = atoi(mode_each[i][3]);

                speedLoss_water_flow(&speed, water_flow);
                Submarine submarine(speed, energy, temparature, depth, oxygen_rate, water_flow);

                while(distance_remain){
                    getline(cin, move_mode);
                    if(move_mode == "1"){ // first move mode
                        // if non-zero energy and non-zero oxygen remains, go one unit
                        energyLoss_temp(&energy, temparature);
                        submarine.light(&energy); // energy loss due to light
                        oxygenLoss_depth(&oxygen_rate, depth);      
                        distance += unit_ocean;
                        distance_remain -= unit_ocean;
                        submarine.setEnergy(energy); // update energy of submarine
                        submarine.setOxygen_rate(oxygen_rate); // update oxygen_rate of submarine
                        vectorSwap(&graphic_progress, current_position++);

                        // record black box
                        if(!distance_remain || !energy || !oxygen_rate){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Submarine", energy, oxygen_rate, speed);
                        }

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(unit_ocean, distance, submarine, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy || !oxygen_rate){ // energy failure or oxygen failure, submarine stop 
                            if(!energy) message_energy_fail(unit_ocean, distance, submarine, graphic_progress, mode_extra); 
                            // energy failure prevails oxygen failure
                            else message_oxygen_fail(unit_ocean, distance, submarine, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }
                        
                        else{
                            message_submarine(unit_ocean, distance, submarine, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y(distance_remain, mode_next, distance, energy, &oxygen_rate, graphic_progress, mode_extra);
                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }
                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }
                    }

                    else if(move_mode == "2"){ // second move mode
                        int unit_number = distance_remain/unit_ocean;
                        int save_remain = distance_remain;
                        for(int i = 0; i < unit_number; i++){ // move units of ocean step by step
                            if(!energy || !oxygen_rate){ // energy failure or oxygen failure, submarine stop
                                if(!energy) message_energy_fail(i * unit_ocean, distance, submarine, graphic_progress, mode_extra);
                                else message_oxygen_fail(i * unit_ocean, distance, submarine, graphic_progress, mode_extra);
                                TC_done = 1;
                                break;
                            }

                            energyLoss_temp(&energy, temparature);
                            submarine.light(&energy); // energy loss due to light
                            oxygenLoss_depth(&oxygen_rate, depth);      
                            distance += unit_ocean;
                            distance_remain -= unit_ocean;
                            submarine.setEnergy(energy); // update energy of submarine
                            submarine.setOxygen_rate(oxygen_rate); // update oxygen_rate of submarine
                            vectorSwap(&graphic_progress, current_position++);
                        }

                        // record black box
                        if(!distance_remain || !energy || !oxygen_rate){
                            update_blackBox(&blackBox_mode, &blackBox_energy, &blackBox_oxygen, &blackBox_speed, 
                            "Submarine", energy, oxygen_rate, speed);
                        }

                        if(TC_done) break;

                        arrive = (!distance_remain) && (i == mode_sequence.size() - 1); // if last sequence of mode, arrival success
                        if(arrive){ // arrive succesfully, arrive prevails failure
                            message_arrive(save_remain, distance, submarine, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }

                        else if(!energy || !oxygen_rate){ // energy failure or oxygen failure, submarine stop 
                            if(!energy) message_energy_fail(save_remain, distance, submarine, graphic_progress, mode_extra); 
                            // energy failure prevails oxygen failure
                            else message_oxygen_fail(save_remain, distance, submarine, graphic_progress, mode_extra);
                            TC_done = 1;
                            break;
                        }
                        
                        else{
                            message_submarine(save_remain, distance, submarine, graphic_progress, mode_extra);
                            // EXTRA case X and Y
                            if(mode_extra == "1" && i < mode_sequence.size() - 1){ // X or Y can't be placed after last sequence
                                char mode_next = *(mode_sequence[i+1] + 1);
                                bool stop = 
                                message_X(distance_remain, mode_next, distance, &energy, oxygen_rate, graphic_progress, mode_extra) ||
                                message_Y(distance_remain, mode_next, distance, energy, &oxygen_rate, graphic_progress, mode_extra);
                                if(stop == true){ // stop by event X or Y
                                    TC_done = 1;
                                    break;
                                }
                            }
                            cout << "Next Move? (1,2)" << endl;
                            cout << "CP-2013-10958>";
                        }                        
                    }
                }
                break;
            }
            }
        }

        // print black box element
        blackBoxPrint(blackBox_mode, blackBox_energy, blackBox_oxygen, blackBox_speed);
        cout << "--------------------" << endl; // one test case done, get next case
    }
    return 0;
}

void energyLoss_temp(int *energy, int temparature){
    if(temparature >= 40) *energy -= 10;
    else if(temparature == 0) *energy -= 8;
    else *energy -= 5;
    if(*energy < 0) *energy = 0; // minimum value is 0
} 

void energyLoss_humidity(int *energy, int humidity){
    if(humidity < 50) *energy -= 5;
    else *energy -= 8;
    if(*energy < 0) *energy = 0; // minimum value is 0
} 

void energyLoss_X(int *energy){
    *energy -= 100;
    if(*energy < 0) *energy = 0; // minimum value is 0
}

void speedLoss_air_density(int *speed, int air_density){
    if(air_density >= 70) *speed -= 500;
    else if(air_density >= 50) *speed -= 300;
    else if(air_density >= 30) *speed -= 200;
} 

void speedLoss_water_flow(int *speed, int water_flow){
    if(water_flow >= 70) *speed -= 10;
    else if(water_flow >= 50) *speed -= 5;
    else if(water_flow >= 30) *speed -= 3;
} 

void oxygenLoss_altitude(int *oxygen_rate, int altitude){
    *oxygen_rate -= 10 * (altitude/1000);
    if(*oxygen_rate < 0) *oxygen_rate = 0; // minimum value is 0
}

void oxygenLoss_depth(int *oxygen_rate, int depth){
    *oxygen_rate -= 5 * (depth/50);
    if(*oxygen_rate < 0) *oxygen_rate = 0; // minimum value is 0
} 

void oxygenLoss_Y(int *oxygen_rate){
    *oxygen_rate -= 30;
    if(*oxygen_rate < 0) *oxygen_rate = 0; // minimum value is 0
}


template <typename V>
void message_arrive(int next_move, int distance, V v, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Final Status:" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Energy: " << v.getEnergy() << endl;
    cout << "Oxygen Level: " << v.getOxygen_rate() << endl; 
    if(mode_extra == "1") vectorPrint(graphic_progress);
    cout << endl;
    cout << "!FINISHED : " << "Arrived" << endl;
}

template <typename V>
void message_energy_fail(int next_move, int distance, V v, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Final Status:" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Energy: " << v.getEnergy() << endl;
    cout << "Oxygen Level: " << v.getOxygen_rate() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
    cout << endl;
    cout << "!FINISHED : " << "Energy failure" << endl; 
}

template <typename V>
void message_oxygen_fail(int next_move, int distance, V v, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Final Status:" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Energy: " << v.getEnergy() << endl;
    cout << "Oxygen Level: " << v.getOxygen_rate() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
    cout << endl;
    cout << "!FINISHED : " << "Oxygen failure" << endl; 
}

void message_initial(Car car, vector<char> graphic_progress, string mode_extra){
    cout << "Current Status: Car" << endl;
    cout << "Distance: 0 km" << endl;
    cout << "Speed: " << car.getSpeed() <<" km/hr" << endl;
    cout << "Energy: " << car.getEnergy() << endl;
    cout << "Temperature: " << car.getTemparature() << " C" << endl;
    cout << "Humidity: " << car.getHumidity() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
}

void message_car(int next_move, int distance, Car car, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Current Status: Car" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Speed: " << car.getSpeed() <<" km/hr" << endl;
    cout << "Energy: " << car.getEnergy() << endl;
    cout << "Temperature: " << car.getTemparature() << " C" << endl;
    cout << "Humidity: " << car.getHumidity() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
}

void message_airplane(int next_move, int distance, Airplane airplane, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Current Status: Airplane" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Speed: " << airplane.getSpeed() <<" km/hr" << endl;
    cout << "Energy: " << airplane.getEnergy() << endl;
    cout << "Oxygen Level: " << airplane.getOxygen_rate() << endl;
    cout << "Temperature: " << airplane.getTemparature() << " C" << endl;
    cout << "Humidity: " << airplane.getHumidity() << endl;
    cout << "Altitude: " << airplane.getAltitude() << " m" << endl;
    cout << "Air Density: " << airplane.getAir_density() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
}

void message_submarine(int next_move, int distance, Submarine submarine, vector<char> graphic_progress, string mode_extra){
    cout << "Successfully moved to next " << next_move << " km" << endl;
    cout << "Current Status: Submarine" << endl;
    cout << "Distance: " << distance << " km"  << endl;
    cout << "Speed: " << submarine.getSpeed() <<" km/hr" << endl;
    cout << "Energy: " << submarine.getEnergy() << endl;
    cout << "Oxygen Level: " << submarine.getOxygen_rate() << endl;
    cout << "Temperature: " << submarine.getTemparature() << " C" << endl;
    cout << "Depth: " << submarine.getDepth() << " m" <<endl;
    cout << "Water Flow: " << submarine.getWater_flow() << endl;
    if(mode_extra == "1") vectorPrint(graphic_progress);
}

// return true if vehicle stop after event X
bool message_X(int distance_remain, char mode_next, int distance ,int *energy, int oxygen_rate, 
vector<char> graphic_progress, string mode_extra){ 
    // Road : reindeer, Sky : eagle, Ocean : shark --> 20 % chance of complete stop (if not stop, lose 100 energy)
    if(!distance_remain && mode_next == 'X'){
        srand((unsigned)time(NULL)); // initialize seed for random generator
        double x = (double)rand() / RAND_MAX; // random real number (0~1)

        if(x >= 0 && x <= 0.2) {// 20 % chance of complete stop
            cout << "Successfully moved to next 0 km" << endl;
            cout << "Final Status :" << endl;
            cout << "Distance: " << distance << " km"  << endl;
            cout << "Energy: " << *energy << endl;
            cout << "Oxygen Level: " << oxygen_rate << endl;
            if(mode_extra == "1") vectorPrint(graphic_progress);
            cout << endl;
            cout << "!FINISHED : " << "Vehicle stop" << endl;
            return true; // vehicle stop by 20 % chance
        }

        else{ // 80 % chance of survival
            energyLoss_X(energy); // lose energy 100
            if(!*energy){ // energy failure after event X
                cout << "Successfully moved to next 0 km" << endl;
                cout << "Final Status :" <<endl;
                cout << "Distance: " << distance << " km"  << endl;
                cout << "Energy: " << *energy << endl;
                cout << "Oxygen Level: " << oxygen_rate << endl;
                if(mode_extra == "1") vectorPrint(graphic_progress);
                cout << endl;
                cout << "!FINISHED : " << "Energy failure" << endl;
                return true; // vehicle stop by energy failure
            }

            return false;
        }
    }
    return false;
}

// return true if vehicle stop after event Y
bool message_Y_car(int distance_remain, char mode_next, int distance, int energy, int oxygen_rate, bool *solar_panel_enable, 
vector<char> graphic_progress, string mode_extra){
    // 35 % chance of complete stop (if not, 50 % chance of solar panel destoy)
    if(!distance_remain && mode_next == 'Y'){
        srand((unsigned)time(NULL)); // initialize seed for random generator
        double x = (double)rand() / RAND_MAX; // random real number (0~1)
        
        if(x >= 0 && x <= 0.35) {// 35 % chance of complete stop
            cout << "Successfully moved to next 0 km" << endl;
            cout << "Final Status :" << endl;
            cout << "Distance: " << distance << " km"  << endl;
            cout << "Energy: " << energy << endl;
            cout << "Oxygen Level: " << oxygen_rate << endl;
            if(mode_extra == "1") vectorPrint(graphic_progress);
            cout << endl;
            cout << "!FINISHED : " << "Vehicle stop" << endl;
            return true; // vehicle stop by 35 % chance
        }

        else{ // 65 % chance of survival
            double y = (double)rand() / RAND_MAX; // random real number (0~1)
            if(y >= 0 && y <= 0.5) *solar_panel_enable = false; // 50 % chance of solar panel destroy
            return false;
        }
    }
    return false;
}

// return true if vehicle stop after event Y
bool message_Y(int distance_remain, char mode_next, int distance, int energy, int *oxygen_rate, 
vector<char> graphic_progress, string mode_extra){
    // 35 % chance of complete stop (if not, 50 % chance of solar panel destoy)
    if(!distance_remain && mode_next == 'Y'){
        srand((unsigned)time(NULL)); // initialize seed for random generator
        double x = (double)rand() / RAND_MAX; // random real number (0~1)
        
        if(x >= 0 && x <= 0.35) {// 35 % chance of complete stop
            cout << "Successfully moved to next 0 km" << endl;
            cout << "Final Status :" << endl;
            cout << "Distance: " << distance << " km"  << endl;
            cout << "Energy: " << energy << endl;
            cout << "Oxygen Level: " << *oxygen_rate << endl;
            if(mode_extra == "1") vectorPrint(graphic_progress);
            cout << endl;
            cout << "!FINISHED : " << "Vehicle stop" << endl;
            return true; // vehicle stop by 35 % chance
        }

        else{ // 65 % chance of survival
            double y = (double)rand() / RAND_MAX; // random real number (0~1)
            if(y >= 0 && y <= 0.5) oxygenLoss_Y(oxygen_rate); // 50 % chance of 30 oxygen loss
            if(!*oxygen_rate){ // oxygen failure after event Y
                cout << "Successfully moved to next 0 km" << endl;
                cout << "Final Status :" << endl;
                cout << "Distance: " << distance << " km"  << endl;
                cout << "Energy: " << energy << endl;
                cout << "Oxygen Level: " << *oxygen_rate << endl;
                if(mode_extra == "1") vectorPrint(graphic_progress);
                cout << endl;
                cout << "!FINISHED : " << "Oxygen failure" << endl;
                return true; // vehicle stop by energy failure
            }
            return false;
        }
    }
    return false;
}

void update_blackBox(vector<string> *blackBox_mode, vector<int> *blackBox_energy, vector<int> *blackBox_oxygen, vector<int> *blackBox_speed, string mode, int energy, int oxygen_rate, int speed){
    (*blackBox_mode).push_back(mode);
    (*blackBox_energy).push_back(energy);
    (*blackBox_oxygen).push_back(oxygen_rate);
    (*blackBox_speed).push_back(speed);
}

void blackBoxPrint(vector<string> mode, vector<int> energy, vector<int> oxygen, vector<int> speed){
    int l = mode.size();
    cout << "Blackbox:" << endl;

    // print mode
    cout << "Mode:";
    for(int i = 0; i < l-1; i++){
        cout << " " << mode[i] << " >";
    }
    cout << " " << mode[l-1] << endl;
    
    // print energy
    cout << "Energy Level:";
    for(int i = 0; i < l-1; i++){
        cout << " " << energy[i] << " >";
    }
    cout << " " << energy[l-1] << endl;

    // print oxygen rate
    cout << "Oxygen Level:";
    for(int i = 0; i < l-1; i++){
        cout << " " << oxygen[i] << " >";
    }
    cout << " " << oxygen[l-1] << endl;

    // print speed
    cout << "Speed:";
    for(int i = 0; i < l-1; i++){
        cout << " " << speed[i] << " >";
    }
    cout << " " << speed[l-1] << endl;
}

void vectorPrint(vector<char> graphic_progress){
    for(int i = 0; i < graphic_progress.size(); i++){
        cout << graphic_progress[i];
    }
    cout << endl;
} 

void vectorSwap(vector<char>* graphic_progress, int current_position){
    char temp = (*graphic_progress)[current_position];
    (*graphic_progress)[current_position] = (*graphic_progress)[current_position + 1];
    (*graphic_progress)[current_position + 1] = temp;
}
