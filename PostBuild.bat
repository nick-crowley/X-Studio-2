//xcopy /E /Y .\Data %1

echo Copying data files...
mkdir %1\Data\Templates

copy .\Data\*.* %1\Data
copy .\Data\Templates\*.* %1\Data\Templates