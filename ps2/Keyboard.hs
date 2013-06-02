module Main where

import Language.Atom
import PS2

main = do
	(sch, _, _, _, _) <- compile atomName defaults { cCode = prePostCode } (device clockSpeed)
	putStrLn $ reportSchedule sch
	where
		atomName = "Keyboard"
		prePostCode _ _ _ = ("#include \"before.c\"", "#include \"after.c\"")
		--Arduino Uno @ 16MHz
		clockSpeed = 16000000

