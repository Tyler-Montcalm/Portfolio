#include <iostream>
#include "lap.h"
#include "sector.h"


using namespace std;



/////// dummy place-holder functions
bool outer_map_button_pressed; //return true if pressed false if not
bool inner_map_button_pressed; //return true if pressed false if not
bool race_mode_on; //return true if pressed false if not
bool Record_sector_location_button_pressed;
///////////////////




////SETUP
void setup()
{
int lap_count = 0;    // lap count starts at zero at system startup
const int N_max_sectors = 5;   //maximum number of sectors in a lap
int N_sectors =0;

map *M[2];    // M[0] = Calibration map outer side of track, M[1] = Calibration map inner side of track
M[0] = new map();  //creates new outer reference map
M[1] = new map();  //creates new inner reference map

lap *L[3];    //L[0] = best lap  L[1] = previous lap, L[2] = current lap

sector *S[N_max_sectors];  // reference sector data:  S[0]: reference sector 1, S[1]: reference sector 2, S[2]: ref. sector 3....
sector *S_prev[N_max_sectors]; // S_prev[0]: previous sector 1 data, S_prev[1]: previous sector 2 data .....
sector *S_best[N_max_sectors];  // S_prev[0]: best sector 1 data, S_prev[1]: best sector 2 data .....

for(int i=0; i<N_max_sectors; i++){
S[i] = new sector(millis());
S_prev = new sector(millis());
s_best = new sector(millis());
}






int main() {

    double X_coord = gps.location.lng();
    double Y_coord = gps.location.lat();
    double Z_coord = gps.altitude.feet();
    double t = millis();



    if (outer_map_button_pressed==true) {
        while (outer_map_button_pressed) {
            M[0]->set_map();   // loops new data points into the array to log the track

        }
        L[0] = new lap(*M[0], t);
        L[1] = new lap(*M[0], t);
        L[2] = new lap(*M[0], t);
    }




    if (inner_map_button_pressed==true) {
        while (inner_map_button_pressed) {
            M[1]->set_map();   // loops new data points into the array to log the track

            if (Record_sector_location_button_pressed==true) {  // setting sector start and end locations on track map

                S[0]->Sx_start = M[0]->Xmap_data[0];  //defaults the first sector to start at map start location
                S[0]->Sy_start = M[0]->Ymap_data[0];

                for (int i = 1; i < N_max_sectors; i++) {
                    S[i]->Sx_end = gps.location.lng();   // sets sector end coordinates where button was pressed
                    S[i]->Sy_end = gps.location.lat();

                    S[i+1]->Sx_start = S[i]->Sx_end;    //defaults next sector start at previous sector end;
                    S[i+1]->Sy_start = S[i]->Sy_end;

                    N_sectors++;
                }
            }
        }
    }





    if (race_mode_on==true) {
        // LAP COUNTING + DATA LOGGING

        // if car at current GPS coordinates + tolerance radius of lap start position, and lap start was over 10 seconds ago, save and start new lap
        if (sqrt(((gps.location.lng() - (L[2]->Lx)) ^ 2) + ((gps.location.lat() - (L[2]->Ly)) ^ 2)) <= 5 && L[2]->MLc->time_data[L[2]->MLc->N_iter] > 20) {
            lap_count++;
            L[2]->save_lap();
            display_lap(*L[0], L[2], lap_count);   // TODO: display_lap function to be made in lap.cpp
        }

        if ((L[2]->lap_time) < (L[0]->lap_time)) {   // L[0]: best lap data, L[2]: current lap.
            L[0] = L[2];    // if L[2] was faster than L[0], set L[2] data as fastest lap.
        }

        L[2] = L[1];   // set current ended lap, as new previous lap.
        L[2]->~lap();  // destruct the old "current lap"
        L[2] = new lap(*M[0], millis());  // creates a new "current lap"


        for (int i = 0; i < N_sectors; i++) {    // checking if vehicle is within a sector's end point zone to stop and record the sector
            if (sqrt(((gps.location.lng() - S[i]->Sx_end) ^ 2) + ((gps.location.lat() - S[i]->Sy_end) ^ 2)) <= 5 && S[i]->sector_time > 10) {
                S_prev[i]->record_sector(millis());
                S_prev[i]->save_sector;
                display_sector(*S_best[i], *S_prev[i]);    //// TODO: display_lap function to be made in sector.cpp

                if (S_prev[i]->sector_time < S_best[i]->sector_time) {
                    S_best[i] = S_prev[i];
                }

                S_prev[i]->~sector();   //delete previous sector
                S_prev[i+1] new sector(millis());  // create the next sector which begins at the end of the last one
                if(i==(N_sectors-1)) {S_prev[0] = new sector(millis());}    // this for loop is needed for the case where the last sector must start a new first sector

            }

        }

    }








    return 0;
}
