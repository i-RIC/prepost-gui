param(
	[Parameter(Mandatory = $true)][string]$Command,
	[string]$StartVc = $(if ($env:IRIC_START_VC) { $env:IRIC_START_VC } else { 'E:\programs\iRIC\start_vc.bat' })
)

# Runs $Command inside cmd.exe after applying the environment defined in start_vc.bat.
# The line that launches devenv.exe is removed so that only the environment is set up.

if (-not (Test-Path -LiteralPath $StartVc)) {
	throw "start_vc.bat not found: $StartVc (set IRIC_START_VC to override)"
}

$lines = Get-Content -LiteralPath $StartVc | Where-Object { $_ -notmatch 'devenv(\.exe)?' }
$tmp = Join-Path ([System.IO.Path]::GetTempPath()) ("iric_env_{0}.bat" -f [System.Guid]::NewGuid().ToString('N'))
$content = @('@echo off') + $lines + @('', $Command, 'exit /b %ERRORLEVEL%')
[System.IO.File]::WriteAllLines($tmp, $content, [System.Text.Encoding]::Default)
try {
	& $env:ComSpec /c "`"$tmp`""
	$code = $LASTEXITCODE
} finally {
	Remove-Item -LiteralPath $tmp -ErrorAction SilentlyContinue
}
exit $code
