<?php
$fp = fopen("iriclib.h", "r");

while (! feof($fp)){
	$line = fgets($fp);
	if (preg_match("/^ *(.+) +(.+)_Mul\\((.*)\\)/", $line, $matches)){
		$rettype  = $matches[1];
		$funcname = $matches[2];
		$argstr   = $matches[3];

		$argstr = str_replace("int fid, ", "", $argstr);
		$argstr = str_replace("int fid", "", $argstr);

		echo $rettype . " " . $funcname . "(" . $argstr . ");\n";
		echo "\n";
	}
}
fclose($fp);
?>
