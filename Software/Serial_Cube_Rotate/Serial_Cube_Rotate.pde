/**
 * <p>The ToxiclibsSupport class of the toxi.processing package provides various
 * shortcuts to directly use toxiclibs geometry datatypes with Processing style
 * drawing operations. Most of these are demonstrated in this example.</p>
 *
 * <p>UPDATES:
 * <ul>
 * <li>2010-12-30: added sphere/cylinder resolution modulation</li>
 * </ul></p>
 */
 
/* 
 * Copyright (c) 2010 Karsten Schmidt
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * http://creativecommons.org/licenses/LGPL/2.1/
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

import toxi.geom.*;
import toxi.geom.mesh.*;
import toxi.math.waves.*;
import toxi.processing.*;
import processing.serial.*;

String myString = null;
Serial myPort;  // The serial port

ToxiclibsSupport gfx;

boolean waitFlag = true;

void setup() {
  size(600,600,P3D);
  gfx=new ToxiclibsSupport(this);
  
  background(255);
  noStroke();
  fill(200,200,200);
  
  // Print a list of connected serial devices in the console
  printArray(Serial.list());
  // Depending on where your GridEYE falls on this list, you
  // may need to change Serial.list()[0] to a different number
  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.clear();
  // Throw out the first chunk in case we caught it in the 
  // middle of a frame
  myString = myPort.readStringUntil(13);
  myString = null;  
}

void draw() {
  
  Quaternion RotQ = new Quaternion(1,0,0,0);
  float qMatrix[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  PMatrix M1 = getMatrix();
  
  // When there is a sizeable amount of data on the serial port
  // read everything up to the first linefeed
  if(myPort.available() > 30){
  myString = myPort.readStringUntil(13);
  
  // generate an array of strings that contains each of the comma
  // separated values
  String inQuat[] = splitTokens(myString, ",");  
  
  // build a Quaternion from inQuat[] array
  RotQ = new Quaternion(float(inQuat[0]), float(inQuat[1]),float(inQuat[2]),float(inQuat[3]));
  
  RotQ.toMatrix4x4().toFloatArray(qMatrix);
  
  M1.set(
  qMatrix[0], 
  qMatrix[1], 
  qMatrix[2], 
  qMatrix[3], 
  qMatrix[4], 
  qMatrix[5], 
  qMatrix[6], 
  qMatrix[7], 
  qMatrix[8], 
  qMatrix[9], 
  qMatrix[10], 
  qMatrix[11], 
  qMatrix[12], 
  qMatrix[13], 
  qMatrix[14], 
  qMatrix[15]
  );
  
  AABB cube;

  background(255);
  
  // Set some mood lighting
  ambientLight(128, 128, 128);
  directionalLight(128, 128, 128, 0, 0, 1);
  lightFalloff(1, 0, 0);
  lightSpecular(0, 0, 0);
  
  // Get to the middle of the screen
  translate(width/2,height/2,0);
  
  // Do some rotates to get oriented "behind" the device
  rotateY(PI);
  rotateX(-PI/2);
  
  // Apply the Matrix that we generated from our IMU Quaternion
  applyMatrix(M1);
  
  // Draw the Cube from a 3D Bounding Box
  cube=new AABB(new Vec3D(0,0,0),new Vec3D(100,100,100));
  gfx.box(cube);
  }else{
    if(waitFlag){
    textSize(32);
    text("Waiting for quaternions to chew on...", 10, 30); 
    waitFlag = false;}
  }

}