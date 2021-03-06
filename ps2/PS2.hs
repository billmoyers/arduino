module PS2 (device) where
import Language.Atom
import Data.Word
import Data.Bits
import GHC.Int

data Key = Key { name :: String, press :: [Int8], release :: [Int8] }

del = Key { name = "DEL", press = [0xE0, 0x71], release = [0xE0, 0xF0, 0x71] }

maxScanCodeLenBytes = 8 :: Int
sendFrameSizeBits = 11 :: Int

device :: Int -> Atom ()
device systemClockSpeed = do
	--Operate w/ a 16KHz clock signal.
	let arbitrary = 15
	let period' = systemClockSpeed `quot` (16000*2*arbitrary)

	--Setup periods.
	let clkPeriod = period'
	let clkPhase = 0
	let datPeriod = clkPeriod * 2
	let datPhase = clkPeriod `quot` 2
	let inputPeriod = clkPeriod `quot` 2

	--Setup keyboard scan code lookups.
	pressScanCode <- array "pressScanCode" (press del)
	releaseScanCode <- array "releaseScanCode" (release del)

	--Setup variables.
	let scanCodes' = [0 | i <- [1 .. maxScanCodeLenBytes]] :: [Int8]
	let datSignal' = [False | i <- [1 .. (length scanCodes') * sendFrameSizeBits]]

	scanCodes <- array "scanCodes" scanCodes'
	datSignal <- array "datSignal" datSignal'
	datSignalLength <- int8 "datSignalLength" 0
	datSignalOffset <- int8 "datSignalOffset" 0

	--Initialize w/ idle state
	clk <- bool "clk" True
	dat <- bool "dat" True 

	period clkPeriod $ phase clkPhase $ atom "pulseClk" $ do
		--Signal the clock line.
		clk <== not_ (value clk)
		call "setClk"

	period datPeriod $ phase datPhase $ atom "pulseDat" $ do
		--If there is something to send, signal the data line and increment.
		let dsl = value datSignalLength
		let dso = value datSignalOffset
		cond $ and_ [dsl >. 0, dso <=. dsl]
		dat <== datSignal !. dso
		incr datSignalOffset
		call "setDat"

	--Poll for input.
	period inputPeriod $ phase 0 $ atom "pollInput" $ do
		cond $ value datSignalLength ==. 0
		call "pollInput"
