param(
	[ValidateSet('logic', 'gui', 'all')][string]$Suite = 'all',
	[switch]$Build,
	[string]$Configuration = 'Debug',
	[string[]]$Functions,
	[int]$TimeoutSec = 300
)

# Builds (optionally) and runs the river survey regression tests.
# Each test function runs in its own process, so that a crash in one function
# does not hide the results of the others. Results are written as xUnit XML.

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$startVc = if ($env:IRIC_START_VC) { $env:IRIC_START_VC } else { 'E:\programs\iRIC\start_vc.bat' }
if (-not $env:IRIC_RS_TEST_PROJECT) {
	# Project with two river survey data (see plans/river_survey.md, R-1). It is copied to a temporary folder by the test.
	$env:IRIC_RS_TEST_PROJECT = 'E:\projects\05_iRIC_土木研など\2024\06_開発公営 GUI機能追加\10_作業履歴\20251009_受領データに基づくチェック\check.ipro'
}

function Import-IricEnvironment {
	if (-not (Test-Path -LiteralPath $startVc)) {
		throw "start_vc.bat not found: $startVc (set IRIC_START_VC to override)"
	}
	$lines = Get-Content -LiteralPath $startVc | Where-Object { $_ -notmatch 'devenv(\.exe)?' }
	$tmp = Join-Path ([System.IO.Path]::GetTempPath()) ("iric_env_{0}.bat" -f [guid]::NewGuid().ToString('N'))
	[System.IO.File]::WriteAllLines($tmp, (@('@echo off') + $lines + @('set')), [System.Text.Encoding]::Default)
	try {
		$output = & $env:ComSpec /c "`"$tmp`"" 2>$null
	} finally {
		Remove-Item -LiteralPath $tmp -ErrorAction SilentlyContinue
	}
	foreach ($line in $output) {
		if ($line -match '^([^=]+)=(.*)$') {
			# values of these variables may contain non-ASCII characters, that are broken through cmd.exe
			if ($Matches[1] -like 'IRIC_RS_*') { continue }
			[System.Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], 'Process')
		}
	}
	$env:PATH = (Join-Path $root "libdlls\$Configuration") + ';' + (Join-Path $root "apps\iricgui\$($Configuration.ToLower())") + ';' + $env:PATH
}

function Initialize-IricRoot {
	# iRIC GUI uses the folder of iRIC.exe as the root folder (definition.xml, zip.exe, unzip.exe),
	# and loads solvers from <root>\..\..\solvers (= apps\solvers).
	if (-not $env:IRIC_RS_TEST_ROOT) {
		$env:IRIC_RS_TEST_ROOT = Join-Path $root "apps\iricgui\$($Configuration.ToLower())"
	}
}

$suites = @()
if ($Suite -eq 'logic' -or $Suite -eq 'all') {
	$suites += [pscustomobject]@{ Name = 'logic'; Dir = 'tests\riversurvey_logic'; Project = 'riversurveyLogicTest' }
}
if ($Suite -eq 'gui' -or $Suite -eq 'all') {
	$suites += [pscustomobject]@{ Name = 'gui'; Dir = 'tests\riversurvey_crosssectionwindow'; Project = 'riversurveyCrosssectionWindowTest' }
}

Import-IricEnvironment

if ($Build) {
	foreach ($s in $suites) {
		$dir = Join-Path $root $s.Dir
		Push-Location $dir
		try {
			& qmake -tp vc | Out-Host
			& msbuild "$($s.Project).vcxproj" /m /nologo /v:minimal "/p:Configuration=$Configuration" /p:Platform=x64 "/p:SolutionDir=$root\" | Out-Host
			if ($LASTEXITCODE -ne 0) { throw "Build failed: $($s.Project)" }
		} finally {
			Pop-Location
		}
	}
}

$resultDir = Join-Path $root 'tests\riversurvey_results'
New-Item -ItemType Directory -Path $resultDir -Force | Out-Null
if ($suites | Where-Object { $_.Name -eq 'gui' }) { Initialize-IricRoot }

$summary = @()
foreach ($s in $suites) {
	$exe = Join-Path $root (Join-Path $s.Dir "$($Configuration.ToLower())\$($s.Project).exe")
	if (-not (Test-Path -LiteralPath $exe)) {
		throw "Test executable not found: $exe (run with -Build)"
	}
	$funcs = & $exe -functions | ForEach-Object { $_.Trim() -replace '\(\)$', '' } | Where-Object { $_ -ne '' }
	if ($Functions) {
		$funcs = $funcs | Where-Object { $Functions -contains $_ }
	}
	foreach ($f in $funcs) {
		$xml = Join-Path $resultDir "$($s.Name)_$f.xml"
		$log = Join-Path $resultDir "$($s.Name)_$f.log"
		Remove-Item -LiteralPath $xml, $log -ErrorAction SilentlyContinue
		$psi = New-Object System.Diagnostics.ProcessStartInfo
		$psi.FileName = $exe
		$psi.Arguments = "$f -o `"$xml`",xunitxml -o `"$log`",txt"
		$psi.UseShellExecute = $false
		$psi.WorkingDirectory = Split-Path $exe
		$psi.RedirectStandardError = $true
		$psi.RedirectStandardOutput = $true
		$p = [System.Diagnostics.Process]::Start($psi)
		$stdoutTask = $p.StandardOutput.ReadToEndAsync()
		$stderrTask = $p.StandardError.ReadToEndAsync()
		$timedOut = -not $p.WaitForExit($TimeoutSec * 1000)
		if ($timedOut) {
			Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue
			$p.WaitForExit()
		}
		$code = $p.ExitCode
		$stderr = $stderrTask.Result
		if ($stderr) { Add-Content -LiteralPath $log -Value "`n--- stderr ---`n$stderr" }

		$status = if ($timedOut) { 'TIMEOUT' } elseif ($code -eq 0) { 'PASS' } elseif ($code -gt 0 -and $code -lt 256) { 'FAIL' } else { 'CRASH' }
		$detail = ''
		if ($status -ne 'PASS') {
			$detail = (Get-Content -LiteralPath $log -ErrorAction SilentlyContinue | Where-Object { $_ -match 'FAIL!|Loc:|Assertion|Actual|Expected|stderr' } | Select-Object -First 4) -join ' | '
			if ($status -eq 'CRASH') { $detail = ('exit=0x{0:X8} ' -f $code) + $detail + ' ' + ($stderr -split "`n" | Select-Object -First 2) -join ' ' }
		}
		$summary += [pscustomobject]@{ Suite = $s.Name; Function = $f; Status = $status; Detail = $detail }
		Write-Host ("[{0,-7}] {1}::{2} {3}" -f $status, $s.Name, $f, $detail)
	}
}

$summary | Export-Csv -LiteralPath (Join-Path $resultDir 'summary.csv') -NoTypeInformation -Encoding UTF8
$failed = @($summary | Where-Object { $_.Status -ne 'PASS' })
Write-Host ''
Write-Host ("Total: {0}, Passed: {1}, Failed: {2}" -f $summary.Count, ($summary.Count - $failed.Count), $failed.Count)
if ($failed.Count -gt 0) { exit 1 }
exit 0
