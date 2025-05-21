@cd /d "%~d0"
pandoc --standalone --embed-resources --metadata title=XXH64-PRNG -V maxwidth:60em -o README.html README.md
@pause
