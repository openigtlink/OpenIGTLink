---
layout: page
title: Specification > Unit
header: Pages
---
{% include JB/setup %}

SENSOR message can handle a part of unites defined in The International System of Unites (SI) in its 8-byte (or 64-bit) field. The field is designed to specifiy a unit consisting of SI-prefix (e.g. milli, micro, kilo etc...) and combination of SI-base and/or SI-derived unites. The bites in the field are assigned as follows:

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
     |PREFIX |   UNIT0   |  EXP0 |   UNIT1   |  EXP1 |   UNIT2   | EXP2
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-...
     0               1               2               3               4 
    
        ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           EXP2 |   UNIT3   |  EXP3 |   UNIT4   |  EXP4 |   UNIT5   |  EXP5 |
        ...-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            4               5               6               7


### PREFIX (4bit)

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI-prefix
</td><td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI-prefix
</td></tr>
<tr>
<td align="left"> 0x0
</td><td align="left"> None
</td><td align="left"> --
</td><td align="left">--
</td></tr>
<tr>
<td align="left"> 0x1
</td><td align="left"> deka (deca) (1e1)
</td><td align="left"> 0x9
</td><td align="left"> deci (1e-1)
</td></tr>
<tr>
<td align="left"> 0x2
</td><td align="left"> hecto (1e2)
</td><td align="left"> 0xA
</td><td align="left"> centi (1e-2)
</td></tr>
<tr>
<td align="left"> 0x3
</td><td align="left"> kilo (1e3)
</td><td align="left"> 0xB
</td><td align="left"> milli (1e-3)
</td></tr>
<tr>
<td align="left"> 0x4
</td><td align="left"> mega (1e6)
</td><td align="left"> 0xC
</td><td align="left"> micro (1e-6)
</td></tr>
<tr>
<td align="left"> 0x5
</td><td align="left"> giga (1e9)
</td><td align="left"> 0XD
</td><td align="left"> nano (1e-9)
</td></tr>
<tr>
<td align="left"> 0x6
</td><td align="left"> tera (1e12)
</td><td align="left"> 0XE
</td><td align="left"> pico (1e-12)
</td></tr>
<tr>
<td align="left"> 0x7
</td><td align="left"> peta (1e15)
</td><td align="left"> 0XF
</td><td align="left"> femto (1e-15)
</td></tr>
</tbody></table>


### UNIT(6bit)

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI base unit name
</td><td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI base unit name
</td></tr>
<tr>
<td align="left"> 0x01
</td><td align="left"> meter
</td><td align="left"> 0x05
</td><td align="left"> kelvin
</td></tr>
<tr>
<td align="left"> 0x02
</td><td align="left"> gram
</td><td align="left"> 0x06
</td><td align="left"> mole
</td></tr>
<tr>
<td align="left"> 0x03
</td><td align="left"> second
</td><td align="left"> 0x07
</td><td align="left"> candela
</td></tr>
<tr>
<td align="left"> 0x04
</td><td align="left"> ampere
</td><td align="left"> --
</td><td align="left"> --
</td></tr>
</tbody></table>


### UNIT(6bit)

SI base units

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI base unit name
</td><td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> SI base unit name
</td></tr>
<tr>
<td align="left"> 0x01
</td><td align="left"> meter
</td><td align="left"> 0x05
</td><td align="left"> kelvin
</td></tr>
<tr>
<td align="left"> 0x02
</td><td align="left"> gram
</td><td align="left"> 0x06
</td><td align="left"> mole
</td></tr>
<tr>
<td align="left"> 0x03
</td><td align="left"> second
</td><td align="left"> 0x07
</td><td align="left"> candela
</td></tr>
<tr>
<td align="left"> 0x04
</td><td align="left"> ampere
</td><td align="left"> --
</td><td align="left"> --
</td></tr>
</tbody></table>


SI derived units

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> Unit name
</td><td align="left" style="background:#eeeeee;"> dimension
</td><td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> Unit name
</td><td align="left" style="background:#eeeeee;"> dimension
</td></tr>
<tr>
<td align="left"> 0x08
</td><td align="left"> radian
</td><td align="left"> meter/meter
</td><td align="left"> 0x12
</td><td align="left"> ohm
</td><td align="left"> meter^2-kilogram/second^3-ampere^2
</td></tr>
<tr>
<td align="left"> 0x09
</td><td align="left"> steradian
</td><td align="left"> meter^2/meter^2
</td><td align="left"> 0x13
</td><td align="left"> siemens
</td><td align="left"> second^3-ampere^2/meter^2-kilogram
</td></tr>
<tr>
<td align="left"> 0x0A
</td><td align="left"> hertz
</td><td align="left"> /second
</td><td align="left"> 0x14
</td><td align="left"> weber
</td><td align="left"> meter^2-kilogram/second^2-ampere
</td></tr>
<tr>
<td align="left"> 0x0B
</td><td align="left"> newton
</td><td align="left"> meter-kilogram/second^2
</td><td align="left"> 0x15
</td><td align="left"> tesla
</td><td align="left"> kilogram/second^2-ampere
</td></tr>
<tr>
<td align="left"> 0x0C
</td><td align="left"> pascal
</td><td align="left"> kilogram/meter-second^2
</td><td align="left"> 0x16
</td><td align="left"> henry
</td><td align="left"> meter^2-kilogram/second^2-ampere^2
</td></tr>
<tr>
<td align="left"> 0x0D
</td><td align="left"> joule
</td><td align="left"> meter^2-kilogram/second^2
</td><td align="left"> 0x17
</td><td align="left"> lumen
</td><td align="left"> candela-steradian
</td></tr>
<tr>
<td align="left"> 0x0E
</td><td align="left"> watt
</td><td align="left"> meter^2-kilogram/second^3
</td><td align="left"> 0x18
</td><td align="left"> lux
</td><td align="left"> candela-steradian/meter^2
</td></tr>
<tr>
<td align="left"> 0x0F
</td><td align="left"> coulomb
</td><td align="left"> second-ampere
</td><td align="left"> 0x19
</td><td align="left"> becquerel
</td><td align="left"> /second
</td></tr>
<tr>
<td align="left"> 0x10
</td><td align="left"> volt
</td><td align="left"> meter^2-kilogram/second^3-ampere
</td><td align="left"> 0x1A
</td><td align="left"> gray
</td><td align="left"> meter^2/second^2
</td></tr>
<tr>
<td align="left"> 0x11
</td><td align="left"> farad
</td><td align="left"> second^4-ampere^2/meter^2-kilogram
</td><td align="left"> 0x1B
</td><td align="left"> sievert
</td><td align="left"> meter^2/second^2
</td></tr>
</tbody></table>

EXP (4-bit)

<table border="1" cellpadding="5" cellspacing="0" align="center">

<tbody><tr>
<td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> Exponent
</td><td align="left" style="background:#eeeeee;"> Value
</td><td align="left" style="background:#eeeeee;"> Exponent
</td></tr>
<tr>
<td align="left"> 0x0
</td><td align="left"> ^0
</td><td align="left"> --
</td><td align="left">--
</td></tr>
<tr>
<td align="left"> 0x1
</td><td align="left"> ^1
</td><td align="left"> 0xF
</td><td align="left"> ^-1
</td></tr>
<tr>
<td align="left"> 0x2
</td><td align="left"> ^2
</td><td align="left"> 0xE
</td><td align="left"> ^-2
</td></tr>
<tr>
<td align="left"> 0x3
</td><td align="left"> ^3
</td><td align="left"> 0xD
</td><td align="left"> ^-3
</td></tr>
<tr>
<td align="left"> 0x4
</td><td align="left"> ^4
</td><td align="left"> 0xC
</td><td align="left"> ^-4
</td></tr>
<tr>
<td align="left"> 0x5
</td><td align="left"> ^5
</td><td align="left"> 0XB
</td><td align="left"> ^-5
</td></tr>
<tr>
<td align="left"> 0x6
</td><td align="left"> ^6
</td><td align="left"> 0XA
</td><td align="left"> ^-6
</td></tr>
<tr>
<td align="left"> 0x7
</td><td align="left"> ^7
</td><td align="left"> 0X9
</td><td align="left"> ^-7
</td></tr>
</tbody></table>









