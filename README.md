# MultiMultiGas

DFRobot sells a series of I2C/UART-based gas sensors that come calibrated and are much more accurate than cheaper sensors like the analog MQ-series. These sensors can have the lower 2 bits of their I2C addresses set using switches and the middle bits set via software, so it's possible to use all of them on one I2C bus. You can query the sensor at a given address to find out what kind of gas it reports on.

This library encapsulates the DFRobot library to provide a simple interface for querying a specific gas sensor (if available) without having to worry about gas type to I2C address mapping.
