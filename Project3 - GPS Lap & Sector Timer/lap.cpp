

#include "lap.h"
#include "map.h"
#include "iostream"
#include "fstream"
using namespace std;



lap::lap(map M, double t, int lap_count) {   // generates regular laps

    Lx = M.Xmap_data[0];    // set lap X coord. start at first Map data recorded entry point
    Ly = M.Ymap_data[0];    // set lap Y coord. start at first Map data recorded entry point
    Lz = M.Zmap_data[0];    // set lap Z coord. start at first Map data recorded entry point
    start_time = t;    //lap start = system clock
    end_time = 10e100;
    lap_time = 10e100;
    this->lap_count = lap_count;
    MLc = new map(); // current lap map see lap.h

}

lap::~lap() {
    if(MLc!=NULL){
        delete MLc;
    }
    else{
        return;
    }
}

int lap::record_lap() {
    if ((MLc->N_iter) < (MLc->N_max)) {
        MLc->Xmap_data[MLc->N_iter] = gps.location.lng();
        MLc->Ymap_data[MLc->N_iter] = gps.location.lat();
        MLc->Zmap_data[MLc->N_iter] = gps.altitude.feet();
        MLc->time_data[MLc->N_iter] = millis() - (start_time);    //time data is current lap time (current system time - system time at start of lap)
        MLc->N_iter++;

        return 0;
    }
    else{
        cout << "\nLap full";
        save_lap();             // saves lap and returns true to int main() to stop recording loop
        return 1;
    }
}
void lap::save_lap() {     //TODO: add a way to append the current lap count to the string variable file name for saving different files for each lap


    char file_name[] = "lap.csv";
    //char lap_number = static_cast<char>(lap_count);   // no idea what im doing here pls halp.
    //char file_name[] = 'lap'+lap_number+'.csv';

    ofstream fout(file_name);

    lap_time = MLc->time_data[MLc->N_iter];

    fout << "Lap time:," << lap_time<< "\n\n";
    fout << "X-Coord, Y-Coord, Z-Coord, Time-stamp\n";

    for(int i=0; i<(MLc->N_iter); i++)
        fout << MLc->Xmap_data[i] << "," << MLc->Ymap_data[i] << "," << MLc->Zmap_data[i] << "," << MLc->time_data[i] << "\n";

}



void display_sector(lap L_best, lap S, int lap_count){

    //// ayyy tyler, tyler, imma let you finish, but can you please make this non-class member function display the time
    //// difference between the best lap time - current lap time?
    //// display lap count (incremented in main() already) display time delta between best and current lap AND current lap time separately
    ////
    //// ex: LCD.display = Lap_count
    ///      LCD.Display = L->lap_time;
    ///      LCD.display = (L_best->lap_time) - (L->lap_time);

    //P.S. i have marked where this function gets called in main() by a TODO: marker.
    // pls do same for the lap class display_lap()
}