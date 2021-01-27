//
// Created by Victor Rasidescu on 2019-12-18.
//

#include "sector.h"
#include "map.h"
#include "lap.h"

using namespace std;


sector::sector(double t) {

    Sx_start = 0;
    Sy_start = 0;
    Sx_end = 10e100;
    Sy_end = 10e100;
    start_time = t;    //system clock time
    end_time = 10e100;
    sector_time = 10e100;

}

sector::~sector() {

}

void sector::record_sector(double t) {

    sector_time = start_time - t;  //start time was initialized

}

void sector::save_sector() {
    ofstream fout("sector.txt");

    fout << sector_time

}

void display_sector(sector S_best, sector S_prev){

    //// ayyy tyler, tyler, imma let you finish, but can you please make this non-class member function display the time
    //// difference between the best sector time - current sector time?
    //// ex: LCD.display = (S_best->sector_time) - (S->sector_time);
    //P.S. i have marked where this function gets called in main() by a TODO: marker.
    // pls do same for the lap class display_lap()
}