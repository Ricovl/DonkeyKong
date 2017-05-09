@echo off
for /r %%a in (*.png) do (
	COPY "%%a" *.*
)

call convpng
del *.png
del convpng.log

for /r %%a in (*.c *.h) do (
	MOVE "%%a" "../src/gfx"
)
pause