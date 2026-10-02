# thirty
Attempt at making my own SPL for OrangePi Zero3.
Name comes from Zero3 = 03, when flipped its 30 = thirty, proper creative.

## What works

- MMC0
- I2C
- AXP313A PMIC, though minimally
- DRAM
- UART
- Green LED

## What doesn't work

- Everything else

## How to build

- Head to the blobs folder, add firmware binaries you wish to boot and use, and edit the config.ini.

(note that the maximum name length is 15 characters.)

- Run make

## How to use

### USB Booting

You can't use the spl-bootable.img in USB boot cases, you will need to use sunxi-fel to boot the SPL, then load each binary individually and finally, reset64 to the target firmware binary.

### SD booting

On linux, use DD

```
dd if=spl-bootable.img of=/dev/SDCARDHERE bs=1k seek=128 && sync
```

Then insert the SD card into the OrangePi and boot.

### SPI flash booting

Use sunxi-fel to flash.

```
sudo sunxi-fel -p spiflash-write 0 spl-bootable.img
```

Then re-plug your OrangePi and boot.

## Credits

u-boot, most of the code was taken from there.
