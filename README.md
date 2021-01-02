# Homekit Thermistor Temp

A [Homekit SDK](https://github.com/espressif/esp-homekit-sdk) based temperature sensor, using a 10k Thermistor e.g. [this from Adafruit](https://www.adafruit.com/product/372).

## Circuit

Wire the thermistor to ADC6 (GPIO34) on an ESP32 board via a 10k resistor

```
        o vcc
        │
       ┌┴┐
       │ │ 10k
       └┬┘
ADC6    │
o───────┤
        │
       ┌┴┐╱
       │╱│ Thermistor
      ╱└┬┘
        │
       ═╧═
```

## Compiling

### Prerequisites

- [ESP-IDF](https://github.com/espressif/esp-idf)
- [Homekit SDK](https://github.com/espressif/esp-homekit-sdk) 
- NodeMCU ESP32

Set the environment variable HOMEKIT_PATH ```HOMEKIT_PATH``` to the base path of the Homekit SDK.


## Building

As long as you have the ESP-IDF and Homekit SDK installed and a NodeMCU ESP32 connected, building and running this should be pretty straight formward, just do ```idf.py build flash monitor``` in the root folder.

The Homekit accessory needs to be provisioned onto your WIFI-net according to the instructions [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html#provisioning-tools). After that it should be possible to add the accessory to the Home app.

## Contributing

Contributions are always welcome!

When contributing to this repository, please first discuss the change you wish to make via the issue tracker, email, or any other method with the owner of this repository before making a change.

Please note that we have a code of conduct, you are required to follow it in all your interactions with the project.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/frklan/[TBD]/tags).

## Authors

* **Fredrik Andersson** - [frklan](https://github.com/frklan)

## License

This project is licensed under the CC BY-NC-SA License - see the [LICENSE](LICENSE) file for details

For commercial/proprietary licensing, please contact the project owner
