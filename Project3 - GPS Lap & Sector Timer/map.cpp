//
// Created by Victor Rasidescu on 2019-12-17.
//

#include "map.h"
#include "stdlib.h"
#include <iostream>

using namespace std;


map::map() {

    Xmap_data = new double[N_max];
    Ymap_data = new double[N_max];
    Zmap_data = new double[N_max];
    time_data = new double[N_max];

    N_iter=0;

}

map::~map() {

    if(Xmap_data!=NULL){delete Xmap_data;}
    else{ cout << "\nError: null Xmap_data pointer";}

    if(Ymap_data!=NULL){delete Ymap_data;}
    else{ cout << "\nError: null Ymap_data pointer";}

    if(Zmap_data!=NULL){delete Zmap_data;}
    else{ cout << "\nError: null Zmap_data pointer";}

}

int map::set_map() {   //call set_map every time to update the map coordinates

    if(N_iter<N_max){
        Xmap_data[N_iter] = gps.location.lng();
        Ymap_data[N_iter] = gps.location.lat();
        Zmap_data[N_iter] = gps.altitude.feet();
        N_iter++;
        return 0;
    }
    else{
        cout << "\nError: too many map points";
        return 1;
    }

}

