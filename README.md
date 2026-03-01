# Bomb Manual Generator

This library is a helper so that modules can generate data for all possible manual codes.

### Usage

- All module templates should be located in `./manual/templates` and will be rendered in alphabetical order.
- All module templates should be compatible with jinja2
- Have a function that receives a uint16_t code, generates the json for that code (by calling `manual::init` and then filling additional information) and then returns that json.
- Call `manual::run` passing the above-mentioned function.

Following this steps will fill `./manual/data` with that module's information for every possible manual code.