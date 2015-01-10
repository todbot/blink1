#Haskell Bindings

Hasnt been tested on a windows system however in theory should work

prerequisites: library should be installed to the system at default locations, this is where Haskells FFI will look

may leak handles, I create a managed pointer to handle deallocation but do not explicitly call the close function. future function would be to create the necessary finalizer that calls close before deallocating memory

included in the directory is a Main.hs which can be easily run and compiled in place by calling the included compileMain, this should be worked into cabal and this will follow shortly

Main.hs sends a series of colors to the first available device in an infite loop

## Exported functions


```
  c_blink1open :: IO (Ptr Blink1Device)

--
 --Scan USB for blink(1) devices.
 -- @return number of devices found
--
  c_blink1_enumerate :: IO (CInt)


  c_blink1_close:: Ptr Blink1Device -> IO ()


-- Fade blink1 to given RGB color over specified time.
-- @param dev blink1 device to command
-- @param fadeMillis time to fade in milliseconds
-- @param r red part of RGB color
-- @param g green part of RGB color
-- @param b blue part of RGB color
--@return -1 on error, 0 on success
  c_blink1_fadeToRGBN ::  Ptr Blink1Device -> CUShort -> CUChar -> CUChar -> CUChar -> CUChar





  ```

## utility functions
```
-- returns either an error msg or a managed pointer
blink1device :: IO (Either String (ForeignPtr Blink1Device))
-- fademillis r g b lednum device
blink1tool ::  Word16 -> Word8 -> Word8 -> Word8 -> Word8-> (ForeignPtr Blink1Device) -> IO (Either String Int)

```


#Use
generally call blink1device to get a managed pointer , then use it pointer to call blinktool
if you want to set up more bindings you will need to study the withForeignPtr call
see source code for blink1tool and chapter 17 of Real world Haskell

thanks all

G
