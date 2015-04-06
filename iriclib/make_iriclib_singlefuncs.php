<?php
$fp = fopen("iriclib.h", "r");

while (! feof($fp)){
	$line = fgets($fp);
	if (preg_match("/^ *(.+) +(.+)_Mul\\((.*)\\)/", $line, $matches)){
		// echo $line;
		//	print_r($matches);
		$rettype  = str_replace(" IRICLIBDLL", "", $matches[1]);
		$funcname = $matches[2];
		$argstr   = $matches[3];

		$argstrs = explode(",", $argstr);
		$args = array();
		foreach ($argstrs as $tmparg){
			$tmparg = trim($tmparg);
			$arg = array();
			if (strpos($tmparg, "*") === FALSE){
				$words = explode(" ", $tmparg);
				$tmpwords = array();
				foreach ($words as $w){
					if ($w){
						$tmpwords[] = $w;
					}
				}
				$arg['type'] = $tmpwords[0];
				$arg['name'] = $tmpwords[1];
			} else {
				if (preg_match("/^(.+) *\\* *([a-zA-Z_]+)/", $tmparg, $matches)){
					// print_r($matches);
					$arg['type'] = str_replace(" ", "", $matches[1]) . "*";
					$arg['name'] = $matches[2];
				} else {
					echo "HANDLE Error: ", $tmparg, "\n";
				}
			}
			// print_r($arg);
			$args[] = $arg;
		}
		echo $rettype . " " . $funcname . "(";
		$argfrags = array();
		for ($i = 0; $i < count($args); ++$i){
			if ($i == 0){continue;}
			$arg = $args[$i];
			$argfrags[] = $arg['type'] . " " . $arg['name'];
		}
		echo implode(", ", $argfrags);
		echo ")\n";
		echo "{\n";
		echo "\t";
		if ($rettype != "void"){
			echo "return ";
		}
		echo $funcname . "_Mul(";
		$argfrags = array();
		$argfrags[] = "lastfileid";
		for ($i = 0; $i < count($args); ++$i){
			if ($i == 0){continue;}
			$arg = $args[$i];
			$argfrags[] = $arg['name'];
		}
		echo implode(", ", $argfrags);
		echo ");\n";
		echo "}\n";
		echo "\n";
	}
}
fclose($fp);
?>
