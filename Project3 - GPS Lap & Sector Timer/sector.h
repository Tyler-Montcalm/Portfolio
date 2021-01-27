//
// Created by Victor Rasidescu on 2019-12-18.
//

#ifndef LAP_CLASS_SECTOR_H
#define LAP_CLASS_SECTOR_H

#include "map.h"
#include "lap.h"



class sector {
public:

    double Sx_start, Sx_end, Sy_start, Sy_end;    //start X,Y coords and end X,Y coords  (we dont care about Z for start/end points)
    double start_time, end_time, sector_time;
    map *MSc;  // current sector map

    sector(double t);
    ~sector();
    void record_sector(double t);
    void save_sector();

};

void display_sector(sector S_best, sector S_prev);


#endif //LAP_CLASS_SECTOR_H
