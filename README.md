# VAMeter-Firmware

Firmware source code of [VA Meter](https://docs.m5stack.com/en/products/sku/K136).

## Build

### Fetch Dependencies

```bash
python ./fetch_repos.py
```

### Desktop

#### Tool Chains

```bash
sudo apt install build-essential cmake
```

#### Build

```bash
mkdir build && cd build
```
```bash
cmake ..
```
```bash
make -j8
```

#### Run

```bash
cd desktop && ./app_desktop_build
```

### IDF

#### Tool Chains

[ESP-IDF v5.1.3](https://docs.espressif.com/projects/esp-idf/en/v5.1.3/esp32s3/index.html)

#### Build

```bash
cd platforms/vameter
```

```bash
idf.py build
```

#### Flash

```bash
idf.py -p <YourPort> flash -b 1500000
```

##### Flash AssetPool

```bash
parttool.py --port <YourPort> write_partition --partition-name=assetpool --input "path/to/AssetPool-VAMeter.bin"
```

If you run desktop build before, you can found `AssetPool-VAMeter.bin` at 

`../../build/desktop/AssetPool-VAMeter.bin`. Or download one from the [release](https://github.com/m5stack/VAMeter-Firmware/releases/latest) page.

