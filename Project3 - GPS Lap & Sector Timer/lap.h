//
// Created by Victor Rasidescu on 2019-12-17.
//

#ifndef LAP_CLASS_LAP_H
#define LAP_CLASS_LAP_H

#include "map.h"



class lap{
public:
    double Lx, Ly, Lz, start_time, end_time, lap_time, lap_count;
    map *MLc;  // Current lap map---- will output this to make M[2] = lap[2]->Mc
    const double GPS_tol = 5; // tolerance area (m) around a GPS point for vehicle position


    lap(map M, double t, int lap_count);
    ~lap();
    void save_lap();
    int record_lap();     // X, Y, Z GPS, system time
};

void display_sector(lap L_best, lap S, int lap_count){



#endif //LAP_CLASS_LAP_H
