//
// Created by Victor Rasidescu on 2019-12-17.
//

#ifndef LAP_CLASS_MAP_H
#define LAP_CLASS_MAP_H





class map{
public:

    double *Xmap_data;   // dynamic array storing the x coordinate of map
    double *Ymap_data;   // dynamic array storing the y coordinate of map
    double *Zmap_data;   // dynamic array storing the z coordinate of map
    double *time_data;   // dynamic array storing the time data of the coordinates


    int N_iter;
    const int N_max = 10000;     // number of max data points for map


    map();
    ~map();
    int set_map();

};








#endif //LAP_CLASS_MAP_H
