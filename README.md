# nfc_tag_id_reader_cpp

This project uses the `libnfc` library to read an NFC tag ID using a PN532 reader and publishes that tag ID to a local MQTT broker.

It is designed as a lightweight, standalone C++ application suitable for embedded Linux environments.


## Configuration

All runtime parameters are configured via the file:

```
config/config.yaml
```

Example:

```yaml
nfc_tag_topic: "selrac/nfc_handler/tags"
mqtt:
  server_address: "tcp://localhost:1883"
  client_id: "nfc_tag_id_reader_cpp_client"
nfc:
  debug: true
  read_data: false
```

## Running the Application

After building (and installing), the binary is located at:

```
./dist/default/bin/nfc_tag_id_reader_cpp
```

To run:

```bash
./dist/default/bin/nfc_tag_id_reader_cpp
```

By default, `libnfc` reads its configuration from `/etc/nfc/libnfc.conf`.

---

## Build Instructions

This project uses `CMakePresets.json`. To build and install:

```bash
cmake --preset=Default
cmake --build --preset=Default
cmake --install --preset=Default
```

---

## Example MQTT Output

When an NFC tag is detected, the program publishes a message like:

```json
{
  "id": "ABCD1234",
  "type": "ISO14443A"
}
```

to the configured (in `configuration.yaml`) MQTT topic.

## Third-party dependencies

This project uses the following external libraries, installed and linked dynamically at runtime:

- **libnfc** (to interface with the PN532 NFC reader) - Licensed under [LGPL-3.0](https://www.gnu.org/licenses/lgpl-3.0.html)
- **Eclipse Paho MQTT C++** (for MQTT communication) - Licensed under [EPL-2.0](https://www.eclipse.org/legal/epl-2.0/)
- **yaml-cpp** (for reading the YAML configuration) - Licensed under [MIT](https://opensource.org/licenses/MIT)
- **nlohmann/json** (for serializing tag IDs as JSON) - Licensed under [MIT](https://opensource.org/licenses/MIT)

These libraries are external dependencies and are not bundled with this repository.
Please ensure that they are properly installed and available on your system before building or running the application.
