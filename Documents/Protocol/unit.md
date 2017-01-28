[Back to Index](/Documents/Protocol/index.md)

UNIT Field for SENSOR Message
=============================

- Protocol Version: 3.0
- Release Date: January 20, 2017

Summary
=======

SENSOR message can handle a part of units defined in The International System
of Unites (SI) in its 8-byte (or 64-bit) field. The field is designed to
specify a unit consisting of SI-prefix (e.g. milli, micro, kilo etc...) and
a combination of SI-base and/or SI-derived units. The bites in the field are
assigned as follows:

~~~~
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
   |PREFIX |   UNIT0   |  EXP0 |   UNIT1   |  EXP1 |   UNIT2   | EXP2
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
   0               1               2               3               4 
  
      ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         EXP2 |   UNIT3   |  EXP3 |   UNIT4   |  EXP4 |   UNIT5   |  EXP5 |
      ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          4               5               6               7
~~~~

PREFIX (4bit)
-------------

 Value        | SI-Prefix
--------------|-----------------
 0x0          | None
 0x1          | deka (deca)(1e1)
 0x2          | hecto (1e2)
 0x3          | kilo (1e3)
 0x4          | mega (1e6)
 0x5          | giga (1e9)
 0x6          | tera (1e12)
 0x7          | peta (1e15)
 --           | --
 0x9          | deci (1e-1)
 0xA          | centi (1e-2)
 0xB          | milli (1e-3)
 0xC          | micro (1e-6)
 0XD          | nano (1e-9)
 0XE          | pico (1e-12)
 0XF          | femto (1e-15)

UNIT(6bit)
----------

- SI Base Units

 Value        | SI Base Unit Name
--------------|-------------------
 0x01         | meter
 0x02         | gram
 0x03         | second
 0x04         | ampere
 0x05         | kelvin
 0x06         | mole
 0x07         | candela


- SI-Derived Units

 Value        | Unit Name       | Dimension
--------------|-----------------|---------------------------
 0x08         | radian          | meter/meter
 0x09         | steradian       | meter^2/meter^2
 0x0A         | hertz           | /second
 0x0B         | newton          | meter-kilogram/second^2
 0x0C         | pascal          | kilogram/meter-second^2
 0x0D         | joule           | meter^2-kilogram/second^2
 0x0E         | watt            | meter^2-kilogram/second^3
 0x0F         | coulomb         | second-ampere
 0x10         | volt            | meter^2-kilogram/second^3-ampere
 0x11         | farad           | second^4-ampere^2/meter^2-kilogram
 0x12         | ohm             | meter^2-kilogram/second^3-ampere^2
 0x13         | siemens         | second^3-ampere^2/meter^2-kilogram
 0x14         | weber           | meter^2-kilogram/second^2-ampere
 0x15         | tesla           | kilogram/second^2-ampere
 0x16         | henry           | meter^2-kilogram/second^2-ampere^2
 0x17         | lumen           | candela-steradian
 0x18         | lux             | candela-steradian/meter^2
 0x19         | becquerel       | /second
 0x1A         | gray            | meter^2/second^2
 0x1B         | sievert         | meter^2/second^2

- EXP (4-bit)

 Value        | Exponent
--------------|---------------
 0x0          | ^0
 0x1          | ^1
 0x2          | ^2
 0x3          | ^3
 0x4          | ^4
 0x5          | ^5
 0x6          | ^6
 0x7          | ^7
 --           | --
 0xF          | ^-1
 0xE          | ^-2
 0xD          | ^-3
 0xC          | ^-4
 0XB          | ^-5
 0XA          | ^-6
