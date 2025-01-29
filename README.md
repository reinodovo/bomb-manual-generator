# Bomb Manual Generator

This library uses [Inja](https://pantor.github.io/inja/) to create KTANE bomb pages from templates.

### Usage

Run `pio run` to create the executable.

Then run it with `.pio/build/manual/program {code} {module1_path} {module2_path} ...`


### Guidelines

For your module to comply with the generator's protocol you must follow some guidelines:

- A native platformio env called `manual`
- All module templates should be located in `./manual/templates` and will be rendered in alphabetical order.
- The manual code must be read as a command line argument.
- Call `manual::init`, which will return a json object that can be further populated (except for the `metadata` field).
- Call `manual::save` with the populated json object.
- Keep `.manual/data` empty, because json data will be saved here.