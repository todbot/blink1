{-# LANGUAGE CPP, ForeignFunctionInterface, EmptyDataDecls #-}

module Main where

#include <blink1-lib.h>
import Foreign
import Foreign.C.Types
import Foreign.C.String
import System.IO.Unsafe
import qualified Data.ByteString.Char8 as B

foreign import ccall "blink1-lib.h blink1_open"
      c_blink1open :: IO (Ptr Blink1Device)

foreign import ccall "blink1-lib.h blink1_enumerate"
  c_blink1_enumerate :: IO (CInt)


data Blink1Device



data Blink1Tool =
  Blink1Tool !(ForeignPtr Blink1Device)


  deriving (Eq, Ord, Show)

blink1open :: IO (Either String Blink1Tool)
blink1open =
  do
   blink1device_ptr <- c_blink1open
   if blink1device_ptr == nullPtr
     then do
       putStrLn "didnt find a device"
       return (Left "no blink1 device found")
     else do
       managed_ptr <- newForeignPtr finalizerFree blink1device_ptr
       putStrLn "found tools"
       return (Right (Blink1Tool managed_ptr))

main :: IO ()
main = do
         _ <- blink1open

         return ()
