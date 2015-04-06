<?php
$fp = fopen("iriclib.c", "r");

$funcs = array();
while (! feof($fp)){
	$line = fgets($fp);
	if (preg_match("/^(static |) *([a-zA-Z]+) +([a-zA-Z0-9_]+)\\((.*)\\)/", $line, $matches)){
		$funcname = $matches[3];
		$funclines = array();
		$funclines[] = rtrim($line);
		$nestdepth = 0;
		$nestdepth += substr_count($line, "{");
		$nestdepth -= substr_count($line, "}");
	} else {
		$trimmedline = trim($line);
		if ($trimmedline == "" && $nestdepth == 0){continue;}
		if (substr($trimmedline, 0, 2) == "//" && $nestdepth == 0){continue;}
		$funclines[] = rtrim($line);
		$nestdepth += substr_count($line, "{");
		$nestdepth -= substr_count($line, "}");
		if ($nestdepth == 0){
			$funcs[$funcname] = $funclines;
		}
	}
}
// print_r($funcs);
// exit;
fclose($fp);

// output local functions.
$outputfuncs = array();

foreach ($funcs as $funcname => $funclines){
	if (FALSE !== strpos($funcname, "local_")){
		foreach ($funclines as $l){
			echo $l, "\n";
		}
		echo "\n";
		$outputfuncs[] = $funcname;
	}
}

$fp =fopen("iriclib.h", "r");

while (! feof($fp)){
	$line = fgets($fp);
//	echo $line;
	if (preg_match("/^ *([a-zA-Z]+) +([a-zA-Z0-9_]+)\\((.*)\\)/", $line, $matches)){
		$funcname = $matches[2];
		if (! isset($funcs[$funcname])){
			echo "Warning: No implementation for ", $funcname, "\n";
		} else {
			$funclines = $funcs[$funcname];
			foreach ($funclines as $l){
				echo $l, "\n";
			}
			echo "\n";
			$outputfuncs[] = $funcname;
		}
	}
}
fclose($fp);

foreach ($funcs as $funcname => $funclines){
	if (array_search($funcname, $outputfuncs) === FALSE){
		echo "WARNING Not output function: ", $funcname, "\n";
	}
}

?>
