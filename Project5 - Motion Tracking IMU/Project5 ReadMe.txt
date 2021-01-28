This code uses an arduino framework, and an MPU 9250 IMU

The libraries are not of my development, and the code is based on open source code.

The project aims to gather the 3D positioning of an object based on an accellerometer, gyroscope and magnetometer.
Much like a phone does.

It utilizes Quaternions for the majority of the calculations in order to limit gimbal lock errors

The project uses x,y,z measurements of gravity to determine its initial euler angles and then uses accellerometer and gyroscope data
in order to determine its euler angles at new time steps.