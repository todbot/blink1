{-# LANGUAGE CPP, ForeignFunctionInterface, EmptyDataDecls #-}

module Main where

#include <blink1-lib.h>
import Foreign
import Foreign.C.Types
import Foreign.C.String
import System.IO.Unsafe
import Control.Exception
import qualified Data.ByteString.Char8 as B

foreign import ccall "blink1-lib.h blink1_open"
      c_blink1open :: IO (Ptr Blink1Device)

foreign import ccall "blink1-lib.h blink1_enumerate"
  c_blink1_enumerate :: IO (CInt)

foreign import ccall "blink1-lib.h blink1_close"
  c_blink1_close:: Ptr Blink1Device -> IO ()

type Blink1Operation = CUInt -> CUChar -> CUChar -> CUChar -> CUChar -> IO CInt

foreign import ccall "blink1-lib.h blink1_close"
  c_blink1_fadeToRGBN ::  Ptr Blink1Device -> Blink1Operation

data Blink1Device

data Blink1Tool = Blink1Tool !(ForeignPtr Blink1Device)
  deriving (Eq, Ord, Show)

-- closingFinalizer :: FinalizerPtr a -> FinalizerPtr a

blink1open :: Blink1Operation -> IO (Either String CInt)
blink1open fadeMillis r g b led =
  do
   blink1device_ptr <- c_blink1open
   if blink1device_ptr == nullPtr
     then do
       putStrLn "didnt find a device"
       return (Left "no blink1 device found")
     else do
     (bracket (newForeignPtr finalizerFree blink1device_ptr) (c_blink1_close) (\ptr -> c_blink1_fadeToRGBN ptr fadeMillis r g b led)) >>= Right


main :: IO ()
main = do
         _ <- blink1open
         return ()
