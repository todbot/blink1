{-# LANGUAGE CPP, ForeignFunctionInterface, EmptyDataDecls #-}

module Main where

#include <blink1-lib.h>
import Foreign
import Foreign.C.Types
import Foreign.C.String
import System.IO.Unsafe
import Control.Exception
import qualified Data.ByteString.Char8 as B

foreign import ccall unsafe "blink1-lib.h blink1_open"
  c_blink1open :: IO (Ptr Blink1Device)

foreign import ccall unsafe "blink1-lib.h blink1_enumerate"
  c_blink1_enumerate :: IO (CInt)

foreign import ccall unsafe "blink1-lib.h blink1_close"
  c_blink1_close:: Ptr Blink1Device -> IO ()

foreign import ccall "blink1-lib.h blink1_fadeToRGBN"
  c_blink1_fadeToRGBN ::  Ptr Blink1Device -> CUShort -> CUChar -> CUChar -> CUChar -> CUChar -> IO CInt

data Blink1Device

data Blink1Tool = Blink1Tool !(ForeignPtr Blink1Device)
  deriving (Eq, Ord, Show)

blink1tool :: Word16 -> Word8 -> Word8 -> Word8 -> Word8-> IO (Either String Int)
blink1tool fadeMillis r g b led =
  do
   blink1device_ptr <- c_blink1open
   if blink1device_ptr == nullPtr
     then do
       return (Left "no blink1 device found")
     else do
            (bracket (c_blink1open) (c_blink1_close) (\ptr -> c_blink1_fadeToRGBN ptr (fromIntegral fadeMillis) (fromIntegral r) (fromIntegral g) (fromIntegral b) (fromIntegral led))) >>= return . Right . fromIntegral


main :: IO ()
main = do
         result <- blink1tool 300 255 255 0 0
         case result of
           Left  msg     -> putStrLn msg
           Right status  -> putStrLn ( "status was "++(show status))
