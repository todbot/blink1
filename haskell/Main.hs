module Main where

import Control.Exception
import qualified Data.ByteString.Char8 as B
import GHC.Float
import Blink1Tool
import Foreign
import Foreign.C.Types
import Foreign.C.String
import System.IO.Unsafe
import Control.Exception




setcolor :: [Int] -> ForeignPtr Blink1Device -> IO ()
setcolor (x:xs) device = do
  result <- blink1tool 10 r g b 0 device
  case result of
    Left msg    -> putStrLn msg
    Right(-1)   -> c_blink1_enumerate  >>= \numBlink1s -> putStrLn (" error : blink1 didnt work , enumeration is "++ (show numBlink1s))
    Right status -> putStrLn ("setting to "++(show r) ++ ", " ++ (show g) ++ ", " ++ (show b)) >> setcolor xs device
  where
    r = ((calc x sin) )
    g = ((calc x cos) )
    b = ((calc (x+45)) sin)
calc :: Int -> (Float -> Float) -> Word8
calc a f =  fromIntegral (float2Int  (f (fromIntegral a) *100))

main::IO()
main= do
  device <- blink1device
  case device of
       Left msg -> putStrLn msg
       Right device -> setcolor [1..] device
