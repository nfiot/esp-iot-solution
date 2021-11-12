**Don't Need Patch in This version, but some IDF files need to be modified**

## Update IDF to Latest Version

1. Install latest `master` branch [ESP-IDF](https://github.com/espressif/esp-idf)
2. CD in to ESP-IDF folder
3. Run command below

```
git pull
git checkout d5f58ab13551cd8
git submodule update --init --recursive
```

## Modify some IDF Files

1. **Fix ESP32-S2 PRE-ECO Chip Bug**, add `ets_delay_us(10)` before `spi_hal_user_start(hal)` in function `spi_new_trans` of file `esp-idf/components/driver/spi_master.c`:

   ```c
   ets_delay_us(10);
   //Kick off transfer
   spi_hal_user_start(hal);
   ```