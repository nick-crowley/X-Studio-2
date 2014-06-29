//xcopy /E /Y .\Data %1

mkdir %1\Data\Templates

copy .\Data\*.* %1\Data
copy .\Data\Templates\*.* %1\Data\Templates

SET ERRORLEV=0
