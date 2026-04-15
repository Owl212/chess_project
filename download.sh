#!/bin/bash
mkdir -p assets/pieces
cd assets/pieces || exit

UA="Mozilla/5.0 (Windows NT 10.0; Win64; x64)"

/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/4/42/Chess_klt45.svg/100px-Chess_klt45.svg.png" -o wK.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/d/d7/Chess_qlt45.svg/100px-Chess_qlt45.svg.png" -o wQ.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/Chess_rlt45.svg/100px-Chess_rlt45.svg.png" -o wR.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/a/a2/Chess_blt45.svg/100px-Chess_blt45.svg.png" -o wB.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/7/70/Chess_nlt45.svg/100px-Chess_nlt45.svg.png" -o wN.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/4/45/Chess_plt45.svg/100px-Chess_plt45.svg.png" -o wP.png

/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/f/f0/Chess_kdt45.svg/100px-Chess_kdt45.svg.png" -o bK.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/4/47/Chess_qdt45.svg/100px-Chess_qdt45.svg.png" -o bQ.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/f/ff/Chess_rdt45.svg/100px-Chess_rdt45.svg.png" -o bR.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/9/98/Chess_bdt45.svg/100px-Chess_bdt45.svg.png" -o bB.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/e/ef/Chess_ndt45.svg/100px-Chess_ndt45.svg.png" -o bN.png
/c/msys64/usr/bin/curl.exe -sL -A "$UA" "https://upload.wikimedia.org/wikipedia/commons/thumb/c/c7/Chess_pdt45.svg/100px-Chess_pdt45.svg.png" -o bP.png

