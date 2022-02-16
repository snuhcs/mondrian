# Offloading System

## Requirements
* Download `yolov4-640.tflite` to `EdgeServer/src/main/assets/` directory.

## Network Protocol
* Topic.REGISTER
  * Header : `null`
    * Message : `EDGE` or `SENSOR`
      * Src, Dst : Each device, `null`

* Topic.SCHEDULE
  * Header : `null`
    * Message : Target Sensor IP
      * Src, Dst : `null`, `null`

* Topic.WEBRTC
  * Header : `ICE`
    * Message : ICE message
      * Src, Dst : Sensor <=> Edge
  * Header : `SDP`
    * Message : SDP message
      * Src, Dst : Sensor <=> Edge

* Topic.TASK
  * Header : `null`
    * Message : `NO_INFERENCE` or `OBJECT_DETECTION` or `FACE_DETECTION`
      * Src, Dst : Sensor, `null`
