$ErrorActionPreference = "Stop"
try {
    # Downloading from Lichess raw github, they are SVGs but I can name them .png or .svg
    # Wait, github raw returns SVG correctly. Let's use it as SVG!
    $pieces = @{
        'wK'='wK'; 'wQ'='wQ'; 'wR'='wR'; 'wB'='wB'; 'wN'='wN'; 'wP'='wP';
        'bK'='bK'; 'bQ'='bQ'; 'bR'='bR'; 'bB'='bB'; 'bN'='bN'; 'bP'='bP'
    }

    New-Item -ItemType Directory -Force -Path "assets\pieces" 

    foreach ($p in $pieces.GetEnumerator()) {
        $url = "https://raw.githubusercontent.com/lichess-org/lila/master/public/piece/cburnett/" + $p.Value + ".svg"
        $outfile = "assets\pieces\" + $p.Key + ".svg"
        Invoke-WebRequest -Uri $url -OutFile $outfile
        Write-Host "Downloaded $outfile"
    }
} catch {
    Write-Error $_
}
