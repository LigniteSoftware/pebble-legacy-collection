# Better Timezone [![Build Status](https://secure.travis-ci.org/jquery-boilerplate/jquery-boilerplate.svg?branch=master)](https://travis-ci.org/jquery-boilerplate/jquery-boilerplate) ![Bower Version](https://badge.fury.io/bo/jquery-boilerplate.svg)
### A jQuery plugin for making sane timezone selects.

This jQuery plugin creates a simple, configurable dropdown and autocomplete element. For some reason, this hasn't really been solved yet. Timezones are pretty weird and vary year to year, so we've compiled and cross-checked several different data sources:

* IANA timezone database
* Microsoft timezone database
* Unicode Common Locale Data Repository

## Dependencies

* jQuery
* Select2

## Plugin Size

~20kb total

## Usage

1. Include jQuery and Select2:

  ```html
  <link href="https://cdnjs.cloudflare.com/ajax/libs/select2/4.0.0-rc.1/css/select2.min.css" rel="stylesheet" />
  <script src="http://ajax.googleapis.com/ajax/libs/jquery/2.0.0/jquery.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/select2/4.0.0-rc.1/js/select2.full.min.js"></script>
  ```

2. Include plugin's code:

  ```html
  <script src="dist/better-timezone.min.js"></script>
  ```

3. Call the plugin:

  ```javascript
  $("#element").betterTimezone({
    showIANA: false,
    showOptgroups: false // cannot be true when showIANA is false
  });
  ```


## Contributing

Check [CONTRIBUTING.md](https://github.com/clara-labs/better-timezone/blob/master/CONTRIBUTING.md) for more information.

## History

Check [Releases](https://github.com/clara-labs/better-timezone/releases) for detailed changelog.

## License

The MIT License (MIT)
Copyright © 2015 Clara Labs, https://www.claralabs.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
