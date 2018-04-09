SparkFun Qwiic VR IMU with the BNO080
========================================

![SparkFun Inertial Measurement Unit - BNO080](https://cdn.sparkfun.com//assets/parts/1/2/7/3/3/14586-VR_IMU__Qwiic__-_BNO080-01.jpg)

[*SparkX IMU BNO080 (SPX-14586)*](https://www.sparkfun.com/products/14586)


The BNO080/BNO085 IMU has a combination triple axis accelerometer/gyro/magnetometer packaged with an ARM Cortex M0+ running powerful algorithms. This enables the BNO080 Inertial Measurement Unit (IMU) to produce accurate rotation vector headings with an error of 5 degrees or less. It's what we've been waiting for: all the sensor data is combined into meaningful, accurate IMU information.

This IC was designed to be implemented in Android based cellular phones to handle all the computations necessary for virtual reality goggles using only your phone. The sensor is quite powerful but with power comes a complex interface. We've written an I<sup>2</sup>C based library that provides the rotation vector (the reading most folks want from an IMU) as well as raw acceleration, gyro, and magnetometer readings. The sensor is capable of communicating over SPI and UART as well!

In addition the BNO080 IMU provides a built-in step counter, tap detector, activity classifier (are you running, walking, or sitting still?), and a shake detector. We are duly impressed.

Repository Contents
-------------------

* **/Documents** - Datasheets
* **/Firmware** - Just the test routine for the testjig. Please see the [BNO080/BNO085 library](https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library) for a large number of examples.
* **/Hardware** - Eagle design files (.brd, .sch)
* **/Software** - Processing Example showing how to manipulate graphics using quaternions

Library
--------------
* **[Arduino Library](https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library)** - Library for reading Quaternions, raw accel/gyro/mag, reading activity classifier, advanced configuration, etc.

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact techsupport@sparkfun.com.

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anyThing Development Board derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.