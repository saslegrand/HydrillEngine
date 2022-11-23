IF exist Dependencies\ ( echo Dependencies already extracted ) ELSE ( powershell Expand-Archive Dependencies.zip)
IF exist Dependencies.zip (del /f /q Dependencies.zip)

rmdir /s /q "Visual/x64"
rmdir /s /q "Binaries"
rmdir /s /q "Source/Scripting/include/Generated"
rmdir /s /q "Source/Engine/include/Generated"

start Visual/HydrillProject.sln
start HydrillScripting.sln