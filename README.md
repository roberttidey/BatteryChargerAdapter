# Battery Charger Adapter
12V lead acid batter charger from a laptop power brick

Construction details at

It makes use of the SSD1306BB and I2CtinyBB libraries under

https://github.com/roberttidey/

## Features
- Initial charge at constant current switching to constant voltage
- OLED display showing volt, amps and accumulated charge
- Uses XL4015 buck converter module with Vadj and Iadj
- ATTiny monitors v and I and drives display
- ATTiny controls Hi and Lo charging modes
- Schottky diode pbattery polarity protection guards against back current




