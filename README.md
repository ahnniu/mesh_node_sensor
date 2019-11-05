# Bluetooth Mesh Node - Sensor

## Overview

This sensor sample demonstrates Bluetooth Mesh Sensor Model.

It's powered by [Zephyr](https://zephyrproject.org) with version > 2.0.0

## Features

### Sensor Properties List

| Name | Type | ID |  Mesh Characteristics |
| ------------------------------------------------------------ | -------------------------------------------------- | ------ | ------------------------------------------------------------ |
| [Present Ambient Temperature](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Specifications/Mesh/Xml/Properties/org.bluetooth.property.present_ambient_temperature.xml) | org.bluetooth.property.present_ambient_temperature | 0x004F | [Temperature 8](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Specifications/Mesh/Xml/Characteristics/org.bluetooth.characteristic.temperature_8.xml) |

**org.bluetooth.property.present_ambient_temperature.xml**

```xml
<!--
Copyright 2017 Bluetooth SIG, Inc. All rights reserved.
-->
<Property name="Present Ambient Temperature" type="org.bluetooth.property.present_ambient_temperature" group="org.bluetooth.property_group.ambient_temperature" property-id="0x004F" last-modified="2017-07-11" approved="Yes">
  <Description>
This property represents an ambient air temperature as measured by a temperature sensor. This property does not distinguish between indoor or outdoor temperature. This context is derived from the location of the element when this property is used, for example by using the device location properties. This property can be used if a temperature sensor can be used both indoors and outdoors.
  </Description>
  <ReferencedCharacteristic name="Temperature 8" type="org.bluetooth.characteristic.temperature_8"/>
</Property>
```

**org.bluetooth.characteristic.temperature_8.xml**

```xml
<!--
Copyright 2017 Bluetooth SIG, Inc. All rights reserved.
-->
<Characteristic xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://schemas.bluetooth.org/Documents/characteristic.xsd" name="Temperature 8" type="org.bluetooth.characteristic.temperature_8" uuid="2B0D" last-modified="2017-07-11" approved="Yes">
  <InformativeText>
    <Abstract>
The Temperature 8 characteristic is used to represent a measure of temperature with a unit of 0.5 degree Celsius.
    </Abstract>
  </InformativeText>
  <Value>
    <Field name="Temperature 8">
      <InformativeText>Unit is degree Celsius with a resolution of 0.5.</InformativeText>
      <Format>sint8</Format>
      <Unit>
org.bluetooth.unit.thermodynamic_temperature.degree_celsius
      </Unit>
      <Minimum>-64.0</Minimum>
      <Maximum>63.5</Maximum>
      <DecimalExponent>0</DecimalExponent>
      <BinaryExponent>-1</BinaryExponent>
      <Multipler>1</Multipler>
      <Description>A value of 0xFF represents 'value is not known'.</Description>
    </Field>
  </Value>
</Characteristic>
```
