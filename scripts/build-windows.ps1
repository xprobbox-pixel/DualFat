param([string]$InnoCompiler = "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe")
$ErrorActionPreference = 'Stop'
$project = Split-Path $PSScriptRoot -Parent
$build = Join-Path $project 'build-windows'
$dist = Join-Path $project 'dist'
function Check-Exit { if ($LASTEXITCODE -ne 0) { throw "Command failed: $LASTEXITCODE" } }
cmake -S $project -B $build -A x64 -DCOBALT_EDITOR_TESTS=ON
Check-Exit
cmake --build $build --config Release --parallel 4
Check-Exit
ctest --test-dir $build -C Release --output-on-failure
Check-Exit
if (!(Test-Path $InnoCompiler)) { throw "Install Inno Setup 6, or supply -InnoCompiler." }
New-Item -ItemType Directory -Force $dist | Out-Null
& $InnoCompiler "/DBuildRoot=$build\CobaltDualFilter_artefacts\Release" "/DOutputRoot=$dist" "$project\installer\DUALFAT.iss"
Check-Exit

Copy-Item "$project\README.txt" "$dist\README.txt"
