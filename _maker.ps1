$CC='gcc'
$OutDir = 'x64'
$OutDepDir = 'x64'
$LibPath = 'libraries'
$Headers = @(
	"$LibPath/flecs"
	#"$LibPath/glm"
	"$LibPath/reactphysics3d/sources/include"
	"$LibPath/imgui"
	"$LibPath/headers"
	"/usr/include/freetype2/"
	"/usr/include/libpng16/"
)
$Flags = @(
	"-g"
	"-ggdb"
	"-fpermissive"
	"-std=c++17"
	"-lstdc++"
	"-lc"
	"-lm"
	"-lglfw"
	"-lvulkan"
	"-ldl"
	"-lpthread"
	"-lX11"
	"-lXxf86vm"
	"-lXrandr"
	"-lXi"
	"-lfreetype"
	"-lpng"
	"-lz"
)
$Script:ToClean = @()

function Get-StringHash
{
    param($String)
    $md5 = New-Object -TypeName System.Security.Cryptography.MD5CryptoServiceProvider
    $utf8 = New-Object -TypeName System.Text.UTF8Encoding
    $hash = [System.BitConverter]::ToString($md5.ComputeHash($utf8.GetBytes($String)))
    return $hash
}

function Get-DependenciesFrom
{
	param($Path, $Recurse=$false)
	$d = if ($Recurse) {
        Get-ChildItem -Recurse $Path
    }
    else
    {
        Get-ChildItem $Path
    }
	$d | Where-Object -Property Extension -imatch '^.c(pp)?$' | ForEach-Object {
		$hash = Get-StringHash $_.FullName
		@{
			Hash="$hash"
			Src=$_.FullName
			Dst="$OutDepDir/$hash.o"
		}
	}
}
function Write-MakeSingleRule
{
    param($Target, $Dependencies, $Flags, $Link=$false, $AddToClean=$true)
    $f = $Flags -join ' '
    $d = $Dependencies -join ' '
	$l = if($Link) {''} else { '-c'}
	if ($AddToClean){
		$Script:ToClean += @($Target)
	}
    Write-Output "$($Target): $d"
    Write-Output "`t$CC $l -o `$@ `$< $f"
}
function Write-MakeAllRule
{
    param($Target, $Dependencies, $Flags, $Link=$false, $AddToClean=$true)
    $f = $Flags -join ' '
    $d = $Dependencies -join ' '
	$l = if($Link) {''} else { '-c'}
	if ($AddToClean){
		$Script:ToClean += @($Target)
	}
    Write-Output "$($Target): $d"
    Write-Output "`t$CC $l -o `$@ `$^ $f"
}

$Dependencies = @(
	(Get-DependenciesFrom -Path "$LibPath/flecs" -Recurse $true),
	(Get-DependenciesFrom -Path "$LibPath/headers" -Recurse $true),
	(Get-DependenciesFrom -Path "$LibPath/reactphysics3d" -Recurse $true),
	(Get-DependenciesFrom -Path "$LibPath/imgui" -Recurse $false),
	(Get-DependenciesFrom -Path "$LibPath/imgui/backends" -Recurse $false)
)
$Dependencies = $Dependencies | ForEach-Object { $_ | ForEach-Object { $_ } }

$Headers = @($Headers | ForEach-Object { "-I$_"  })
$f = @($Headers) + $Flags

$MainD = Get-ChildItem | Where-Object -Property Extension -imatch '^.c(pp)?$' | ForEach-Object {
	"$OutDir/$($_.BaseName).o"
}
$MainD += @($Dependencies | ForEach-Object { $_.Dst })

Write-MakeAllRule -Target "x64/out" -Dependencies $MainD -Flags $f -Link $true

Write-MakeSingleRule -Target "$OutDir/%.o" -Dependencies "%.cpp" -Flags $f

$Dependencies | ForEach-Object {
    Write-MakeSingleRule -Target $_.Dst -Dependencies $_.Src -Flags $f
}

Write-Output "clean:"
Write-Output "`trm -r $OutDir/*"